#include "Cache.hpp"
#include <tlm>
#include <string>

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

cache::cache(sc_module_name name) :
    sc_channel(name),
    PROCESS_soc("PROCESS_soc"),
    offset(0, SC_NS)
{
    PROCESS_soc.register_b_transport(this, &cache::b_transport_proc);

    dram = new DRAM_data("DRAM");
    cache_DRAM_port.bind(*dram);

    SC_THREAD(write);
    sensitive << start_event;
    // dont_initialize();
    SC_THREAD(read);
    sensitive << start_read;
    // dont_initialize();

    /* ----------------------------------- */
    max_x = DATA_HEIGHT;
    max_y = DATA_WIDTH;
    /* ----------------------------------- */

    start_address_address = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT;

    for(int i = 0; i < 9; i++)
        write_en[i] = false;

    // done_pb_cache.write(true);
    cout << "Cache::Napravljen je Cache modul!" << endl;
}

// Implementacija funkcije za kompresiju stapica podataka
void cache::compress_data_stick(dram_word* data, dram_word* cache_mem_pos, unsigned char* compressed_stick_index, unsigned char &compressed_stick_lenght, unsigned char &cache_line_len)
{
    unsigned char length = 0;                   // ukupan broj nenultih elemenata
    unsigned char cache_line_offset = 0;        // na koju liniju kesa treba da se upise podatak

    // cout << "Cache::indeksi koji se salju WMEM su: " << endl;

    for(int i = 0; i < (DATA_DEPTH / 5 + 1); i++)
    {
        if(data[i] & (MASK_NON_ZEROS << 60))
        {
            for(int j = 0; j < 5; j++)          //  4|3|2|1|0, 0|0|0|0|5
            {
                if(data[i] & (MASK_DATA << (BIT_WIDTH * j)))
                {
                    compressed_stick_index[length] = i * 5 + j;
                    length++;
                }
            }

            removeZeros(data[i]);
            *(cache_mem_pos + cache_line_offset) = data[i];
            cache_line_offset++;
        }
    }

    // cout << "---------" << endl;

    cache_line_len = cache_line_offset;
    compressed_stick_lenght = length;
}

void cache::removeZeros(sc_dt::uint64 &data)
{
    sc_dt::uint64 temp_data[5];
    sc_dt::uint64 len = data & (MASK_NON_ZEROS << 60);
    int cnt;

    cnt = 0;

    for(int i = 0; i < 5; i++)
    {
        if(data & (MASK_DATA << (BIT_WIDTH * i)))
            temp_data[cnt++] = (data & (MASK_DATA << (BIT_WIDTH * i))) >> (BIT_WIDTH * i);
    }

    data = 0;

    for(int i = 0; i < cnt; i++)
        data |= (MASK_DATA & temp_data[i]) << (BIT_WIDTH * i);

    data |= len;
}


/*

Racunanje write trouhput-a:

1. Odrediti kasnjenja za write i read
2. Sta se desava ako imamo cache miss
3. Vreme treba da tece globalno
4. Read i Wrute mogu da rade paralelno, pa treba izbeci dvostruko racunanje vremena


*/

// proces
void cache::write()
{

    /*

    Kasnjenje Write funkcije
    Inicijalno_kasnjenje / 3 + broj_stanja * Tclk + 13 * Tclk

    */

    dram_word* data;
    unsigned char compressed_stick_index[DATA_DEPTH]; // Niz u kom se nalaze indeksi nenultih elemenata unutar jednog stika
    unsigned char compressed_stick_length;            // Ovde ce se upisati duzina kompresovanog niza podataka
    unsigned char cache_line_len;

    // Inicijalno
    data = new dram_word[DATA_HEIGHT];
    cache_DRAM_port->read_cache_DRAM(data, start_address_address, offset); // Citam podatke iz DRAM-a

    /* Upisivanje podataka u start_address */

    // cout << endl;
    for(int i = 0; i < DATA_HEIGHT; i++)
    {
        start_address[i] = data[i];
        // cout << start_address[i] << endl;
    }
    // cout << endl;
    delete data;

    /* Kraj upisivanja podataka u start_address */

    cout << "WRITE::Vreme koje je proteklo da se procita start_address: " << offset << endl;


    /* Prvi segment */
    // Prvo treba napuniti linije kesa
    int cnt = 0;
    for(int i = 0; i < CACHE_SIZE; i++)
    {
        data = new dram_word;
        cache_DRAM_port->read_cache_DRAM(data, start_address[i / (CACHE_SIZE / 2)] + cnt * (DATA_DEPTH / 5 + 1), offset);
        cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
        compress_data_stick(data, cache_mem + i * (DATA_DEPTH / 5 + 1), compressed_stick_index, compressed_stick_length, cache_line_len);
        delete data;
        cache_line_length[i] = cache_line_len; // Upisi i duzinu linije kesa u neku memoriju
        address_hash[i] = (i / (CACHE_SIZE / 2)) * max_y + cnt;
        if(cnt == 0)
        {
            amount_hash[i] = 2 * W_kn;
        }
        else
        {
            if(cnt == DATA_WIDTH - 1)
            {
                amount_hash[i] = 2 * W_kn;
            }
            else
            {
                amount_hash[i] = 3 * W_kn;
            }
        }

        cout << "WRITE::amount_hash je: " << to_string(amount_hash[i]) << endl;
        cnt = (cnt + 1) % (CACHE_SIZE / 2); // 0, 1, 2, 3, 4, 0, 1, 2, ...
        write_en[i] = false;

        WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                          address_hash[i], i);
    }
    /* Kraj prvog segmenta */

    /* Drugi segment */
    // Ciranje ostatka dvoreda
    for(unsigned int y_i = CACHE_SIZE / 2; y_i < max_y; y_i++)
    {
        for(unsigned int d = 0; d < 2; d++)
        {
            unsigned char full = 0;


            cout << endl << endl;
            cout << "---------------------" << endl;
            cout << "WRITE::Sadrzaj amount_hash:" << endl;

            for(int i = 0; i < CACHE_SIZE; i++)
                cout << to_string(amount_hash[i]) << endl;

            cout << "---------------------" << endl;
            cout << endl << endl;

            // Proveravamo da li ima slobodnih mesta za upis u kes
            for(int i = 0; i < CACHE_SIZE; i++)
            {
                full += (unsigned char)write_en[i];
            }

            if(!full)
            {
                cout << "WRITE::Write ceka na slobodno mesto!" << endl;
                wait(write_enable); // Ovo treba da se desava samo ako je cache pun
            }


            // Trazi koja je prva slobodna linija
            unsigned char free_cache_line;
            for(int i = 0; i < CACHE_SIZE; i++)
            {
                if(write_en[i])
                {
                    free_cache_line = i;
                    break;
                }
            }

            cout << "WRITE::Write cita podatak: " << "(" << d << ", " << y_i << ")" << endl;

            data = new dram_word;
            cache_DRAM_port->read_cache_DRAM(data, start_address[d] + y_i * (DATA_DEPTH / 5 + 1), offset);
            cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
            compress_data_stick(data, cache_mem + free_cache_line * (DATA_DEPTH / 5 + 1), compressed_stick_index, compressed_stick_length, cache_line_len);
            delete data;
            cache_line_length[free_cache_line] = cache_line_len;
            address_hash[free_cache_line] = d * max_y + y_i;
            write_en[free_cache_line] = false;

            switch(y_i)
            {
                case 0:
                case Y_LIMIT:
                    {
                        amount_hash[free_cache_line] = 2 * W_kn;
                    }
                    break;

                default:
                    {
                        amount_hash[free_cache_line] = 3 * W_kn;
                    }
                    break;

            }

            cout << "WRITE::amount_hash je: " << to_string(amount_hash[free_cache_line]) << endl;

            // Iscitaj koji stick zeli read da procita
            unsigned int temp_x = stick_address_cache >> 32;
            unsigned int temp_y = stick_address_cache & MASK_ADDRESS;

            // Ako je upisa stick koji read zeli, onda odblokiraj read
            if(temp_x * max_y + temp_y == d * max_y + y_i)
                read_enable.notify();

            cout << "WRITE::adresa koja se salje WMEM-u je: " << to_string(address_hash[free_cache_line]) << endl;
            WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                              address_hash[free_cache_line], free_cache_line);
        }
    }
    /* Kraj drugog segmenta */

    /* Treci segment */
    // Citanje ostatka ulaza
    for(unsigned int x_i = 0; x_i < max_x - 1; x_i++)
    {
        for(unsigned int y_i = 0; y_i < max_y; y_i++)
        {
            for(unsigned int d = 0; d < 3; d++)
            {

                if(!((x_i == max_x - 2) && (d == 2)))
                {
                    unsigned char full = 0;

                    // Proveravamo da li ima slobodnih mesta za upis u kes
                    for(int i = 0; i < CACHE_SIZE; i++)
                    {
                        full += (unsigned char)write_en[i];
                    }

                    if(!full)
                    {
                        cout << "WRITE::Write ceka na slobodno mesto!" << endl;
                        wait(write_enable); // Ovo treba da se desava samo ako je cache pun
                    }


                    // Trazi koja je prva slobodna linija
                    unsigned char free_cache_line;
                    for(int i = 0; i < CACHE_SIZE; i++)
                    {
                        if(write_en[i])
                        {
                            free_cache_line = i;
                            break;
                        }
                    }

                    cout << "WRITE::Write cita podatak: " << "(" << x_i + d << ", " << y_i << ")" << endl;
                    data = new dram_word;
                    cache_DRAM_port->read_cache_DRAM(data, start_address[x_i + d] + y_i * (DATA_DEPTH / 5 + 1), offset);
                    cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
                    compress_data_stick(data, cache_mem + free_cache_line * (DATA_DEPTH / 5 + 1), compressed_stick_index, compressed_stick_length, cache_line_len);
                    delete data;
                    *(cache_line_length + free_cache_line) = cache_line_len;
                    address_hash[free_cache_line] = (x_i + d) * max_y + y_i;
                    write_en[free_cache_line] = false;

                    switch(y_i)
                    {
                        case 0:
                        case Y_LIMIT:
                            {
                                amount_hash[free_cache_line] = 2 * W_kn;
                            }
                            break;

                        default:
                            {
                                amount_hash[free_cache_line] = 3 * W_kn;
                            }
                            break;

                    }

                    cout << "WRITE::amount_hash je: " << to_string(amount_hash[free_cache_line]) << endl;
                    // Iscitaj koji stick zeli read da procita
                    unsigned int temp_x = stick_address_cache >> 32;
                    unsigned int temp_y = stick_address_cache & MASK_ADDRESS;

                    // Ako je upisa stick koji read zeli, onda odblokiraj read
                    if(temp_x * max_y + temp_y == (x_i + d) * max_y + y_i)
                        read_enable.notify();

                    cout << "WRITE::adresa koja se salje WMEM-u je: " << to_string(address_hash[free_cache_line]) << endl;
                    WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                                      address_hash[free_cache_line], free_cache_line);
                }
            }
        }
    }
    /* Kraj treceg segmenta */
}

// proces
void cache::read()
{

    /*

    Kasnjenje Read funkcije
    broj_stanja * Tclk + stick_length * clk

    */


    bool flag = true;

    while(true)
    {
        if(flag)
        {
            done_pb_cache.write(false);
            flag = false;
        }

        cout << "READ::Read se zaustavio!" << endl;
        wait(); // ceka dok ne dobije neki podatak
        cout << "READ::Read je nastavio sa radom!" << endl;

        // Izdvoji x i y adrese
        unsigned int x = stick_address_cache >> 32;
        unsigned int y = stick_address_cache & MASK_ADDRESS;

        cout << "READ::Od read se traze podaci: (" << x << ", " << y << ")" << endl;

        // Trazi na kojoj liniji kesa se nalazi podatak adresiran sa x i y
        /* --------------------------------------------- */

        unsigned int cache_line = INVALID_ADDRESS;
        for(int i = 0; i < CACHE_SIZE; i++)
        {
            if(address_hash[i] == x * max_y + y)
                cache_line = i;
        }

        offset += sc_time(2 * CLK_PERIOD, SC_NS);

        // Proveri da li si nasao ipak taj podatak
        if(cache_line == (unsigned int)INVALID_ADDRESS)
        {
            cout << "READ::Invalidna adresa!" << endl;
            wait(read_enable);                          // write proces mora da digne event ako je upisao podataka sa adresom x * x_max + y, u stick_address_cache se nalaze koje x i y trazim
                                                        // Write mora da javi read gde je upisao taj podatak
            cache_line = write_cache_line;              // ovu promenljivu odredjuje write
        }

        offset += sc_time(1 * CLK_PERIOD, SC_NS);


        WMEM_cache_port->valid_cache_line(cache_line);
        cout << "READ::Linija kesa na kojoj se nalazi trazeni podatak: " << cache_line << endl;

        /* --------------------------------------------- */

        // Umanji za jedan iskoristivost podatka
        cout << "READ::Pre ovog read, amount_hash je: " << to_string(amount_hash[cache_line]) << endl;
        amount_hash[cache_line]--;
        cout << "READ::Nakon ovog read, amount_hash je: " << to_string(amount_hash[cache_line]) << endl;

        /* --------------------------------------------- */

        offset += sc_time(1 * CLK_PERIOD, SC_NS);

        // Proveri da li moze write da upisuje novu liniju
        if(amount_hash[cache_line] == 0)
        {
            write_en[cache_line] = true;
            write_enable.notify();
        }

        // Upisi lokaciju prvog elementa niza
        dram_word* stick_data_cache = cache_mem + cache_line * (DATA_DEPTH / 5 + 1);
        type* data_stick;

        int cnt = 0;
        unsigned char stick_length = 0;
        cout << "READ::PB je procitao sledece podatke:" << endl;
        for(int i = 0; i < cache_line_length[cache_line]; i++)
        {
            for(int j = 0; j < (int)(stick_data_cache[i] >> 60); j++)
            {
                cache_mem_read_line[cnt++] = (type)((stick_data_cache[i] & (MASK_DATA << (BIT_WIDTH * j))) >> (BIT_WIDTH * j));
                cout << cache_mem_read_line[cnt - 1] << endl;

            }
            // cout << std::hex << stick_data_cache[i] << endl;
            stick_length += (unsigned char)(stick_data_cache[i] >> 60);
        }


        offset += sc_time(stick_length * CLK_PERIOD, SC_NS);

        data_stick = cache_mem_read_line;
        cache_pb_port->write_cache_pb(&data_stick, stick_length, offset);

        bool done = done_pb_cache.read();
        done_pb_cache.write(!done);

    }
}

/* ----------------------------------------------------------------------------------- */
/* Implementacija PB <-> Cache interfejsa */

void cache::read_pb_cache(const uint64 &stick_address, sc_time &offset_pb)
{
    offset += offset_pb;
    stick_address_cache = stick_address; // Upisi x i y koje PB trazi
    start_read.notify();
}

/* ----------------------------------------------------------------------------------- */

void cache::b_transport_proc(pl_t& pl, sc_time& offset)
{

    uint64 address = pl.get_address();
    tlm_command cmd = pl.get_command();

    switch(cmd)
    {
        case TLM_READ_COMMAND:
        {
            switch(address)
            {
                case START_ADDRESS_ADDRESS: // Ova situacija je samo za debug
                {
                    unsigned int* data = reinterpret_cast<unsigned int*>(pl.get_data_ptr());
                    *data = start_address_address;

                    pl.set_response_status(TLM_OK_RESPONSE);

                    // offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }

                case ACK:
                {
                    unsigned char* data = pl.get_data_ptr();
                    *data = (unsigned char)ack;
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(50 * CLK_PERIOD, SC_NS);

                    break;
                }
                default:

                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Cache::Error while trying to read data" << endl;

                    break;
            }

            break;
        }

        case TLM_WRITE_COMMAND:
        {
            switch(address)
            {
                case START_ADDRESS_ADDRESS:
                {
                    start_address_length = pl.get_data_length();
                    start_address_address = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    cout << "Cache::" << start_address_address << endl;
                    cout << start_address_length << endl;

                    ack = true;

                    offset += sc_time(50 * CLK_PERIOD, SC_NS); // Samo adresa se salje, a ne cela tabela

                    break;
                }
                case START:
                {
                    start_event.notify();
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(50 * CLK_PERIOD, SC_NS);

                    break;
                }
                case MAX_X:
                {
                    max_x = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(50* CLK_PERIOD, SC_NS);

                    break;
                }
                case MAX_Y:
                {
                    max_y = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(50 * CLK_PERIOD, SC_NS);

                    break;
                }
                default:

                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Cache::Error while trying to write data" << endl;

                    break;
            }

            break;

        }
        default:

            pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            cout << "Cache::Wrong command!" << endl;

            break;


    }
}
