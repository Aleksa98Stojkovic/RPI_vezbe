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

    write_en = MASK_WRITE_EN;

    // done_pb_cache.write(true);
    cout << "Cache::Napravljen je Cache modul!" << endl;
}

void cache::write_cache(dram_word* data, dram_word* cache_line)
{
    for(int i = 0; i < DATA_DEPTH / 5 + 1; i++)
    {
        *(cache_line + i) = data[i];
    }
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

    // Inicijalno
    data = new dram_word[DATA_HEIGHT];
    cache_DRAM_port->read_cache_DRAM(data, start_address_address, offset); // Citam podatke iz DRAM-a

    /* Upisivanje podataka u start_address */

    for(int i = 0; i < DATA_HEIGHT; i++)
    {
        start_address[i] = data[i];
    }
    delete data;

    /* Kraj upisivanja podataka u start_address */

    cout << "WRITE::Vreme koje je proteklo da se procita start_address: " << offset << endl;


    // Citanje ostatka ulaza
    int cache_init = 0;
    for(unsigned int x_i = 0; x_i < DATA_HEIGHT - 2; x_i++)
    {
        for(unsigned int y_i = 0; y_i < DATA_WIDTH; y_i++)
        {
            for(unsigned int d = 0; d < 3; d++)
            {

                if(cache_init < CACHE_SIZE)
                {
                    data = new dram_word;
                    cache_DRAM_port->read_cache_DRAM(data, start_address[x_i + d] + y_i * (DATA_DEPTH / 5 + 1), offset);
                    write_cache(data, cache_mem + cache_init * (DATA_DEPTH / 5 + 1));
                    cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
                    delete data;
                    address_hash[cache_init] = (x_i + d) * max_y + y_i;
                    if((y_i == 0) || (y_i == DATA_WIDTH - 1))
                    {
                        amount_hash[cache_init] = 1 * W_kn;
                    }
                    else
                    {
                        if((y_i == 1) || (y_i == DATA_WIDTH - 2))
                        {
                            amount_hash[cache_init] = 2 * W_kn;
                        }
                        else
                        {
                            amount_hash[cache_init] = 3 * W_kn;
                        }
                    }

                    cout << "WRITE::amount_hash je: " << to_string(amount_hash[cache_init]) << endl;
                    write_en &= ~(1 << cache_init);

                    cout << "WRITE::cache_init je: " << cache_init << endl;
                    cache_init++;
                }
                else
                {

                    if(!write_en)
                    {
                        cout << "WRITE::Write ceka na slobodno mesto!" << endl;
                        wait(write_enable); // Ovo treba da se desava samo ako je cache pun
                    }

                    // Trazi koja je prva slobodna linija
                    unsigned char free_cache_line;
                    for(int i = 0; i < CACHE_SIZE; i++)
                    {
                        if(write_en & (1 << i))
                        {
                            free_cache_line = i;
                            break;
                        }
                    }

                    cout << "WRITE::Write cita podatak: " << "(" << x_i + d << ", " << y_i << ")" << endl;
                    data = new dram_word;
                    cache_DRAM_port->read_cache_DRAM(data, start_address[x_i + d] + y_i * (DATA_DEPTH / 5 + 1), offset);
                    cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
                    write_cache(data, cache_mem + free_cache_line * (DATA_DEPTH / 5 + 1));
                    delete data;
                    address_hash[free_cache_line] = (x_i + d) * max_y + y_i;
                    write_en &= ~(1 << free_cache_line);

                    switch(y_i)
                    {
                        case 0:
                        case Y_LIMIT:
                            {
                                amount_hash[free_cache_line] = 1 * W_kn;
                            }
                            break;

                        case 1:
                        case Y_LIMIT - 1:
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

                }
            }
        }
    }
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
            while(1);
        }

        offset += sc_time(1 * CLK_PERIOD, SC_NS);

        /* --------------------------------------------- */

        // Umanji za jedan iskoristivost podatka
        amount_hash[cache_line]--;
        cout << "READ::Amount_hash je: " << to_string(amount_hash[cache_line]) << endl;

        /* --------------------------------------------- */

        offset += sc_time(1 * CLK_PERIOD, SC_NS);

        // Proveri da li moze write da upisuje novu liniju
        if(amount_hash[cache_line] == 0)
        {
            write_en |= (1 << cache_line);
            write_enable.notify();
        }

        // Upisi lokaciju prvog elementa niza
        dram_word* stick_data_cache = cache_mem + cache_line * (DATA_DEPTH / 5 + 1);
        vector<type> data_stick;
        int j_len;

        cout << "READ::PB je procitao sledece podatke:" << endl;
        for(int i = 0; i < DATA_DEPTH / 5 + 1; i++)
        {
            if(i == DATA_DEPTH / 5)
                j_len = DATA_DEPTH % 5;
            else
                j_len = 5;

            for(int j = 0; j < j_len; j++)
            {
                data_stick.push_back((type)((stick_data_cache[i] & (MASK_DATA << (BIT_WIDTH * j))) >> (BIT_WIDTH * j)));
                cout << (type)((stick_data_cache[i] & (MASK_DATA << (BIT_WIDTH * j))) >> (BIT_WIDTH * j)) << endl;
            }

        }


        offset += sc_time((DATA_DEPTH / 5 + 1) * CLK_PERIOD, SC_NS);

        cache_pb_port->write_cache_pb(data_stick, offset);
        data_stick.clear();

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
