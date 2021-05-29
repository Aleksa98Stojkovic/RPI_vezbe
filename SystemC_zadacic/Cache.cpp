#include "Cache.hpp"
#include <tlm>
#include <string>

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

cache::cache(sc_module_name name) :
    sc_channel(name),
    Cache_DRAM_soc("DRAM_soc"),
    PROCESS_soc("PROCESS_soc")
{

    PROCESS_soc.register_b_transport(this, &cache::b_transport_proc);

    SC_THREAD(write);
    sensitive << start_event;
    // dont_initialize();
    SC_THREAD(read);
    sensitive << start_read;
    // dont_initialize();

    /* ----------------------------------- */
    max_x = DATA_HEIGHT;
    max_y = DATA_WIDTH;
    start_address_address = DATA_DEPTH * DATA_HEIGHT * DATA_WIDTH;
    /* ----------------------------------- */

    for(int i = 0; i < 9; i++)
        write_en[i] = false;

    // done_pb_cache.write(true);
    cout << "Cache::Napravljen je Cache modul!" << endl;
}

// Implementacija funkcije za kompresiju stapica podataka
void cache::compress_data_stick(type* data_stick, type* cache_mem_pos, unsigned char* compressed_stick_index, unsigned char &compressed_stick_lenght)
{
    unsigned char length = 0;
    for(int i = 0; i < DATA_DEPTH; i++)
    {
        if(data_stick[i] != 0)
        {
            cache_mem_pos[length] = data_stick[i];
            compressed_stick_index[length] = i;
            length++;
        }
    }

    compressed_stick_lenght = length;
}

void cache::copy_cache_line(type* cache_mem_read_line, unsigned char &stick_lenght, type* cache_mem_start_address)
{
    for(int i = 0; i < stick_lenght; i++)
    {
        cache_mem_read_line[i] = cache_mem_start_address[i];
    }
}

// proces
void cache::write()
{

    // Promenljive neophodne za tlm transakcije sa DRAM memorijom
    pl_t pl;
    uint64 tlm_address;
    sc_time offset(0, SC_NS); // Za pocetak je inicijalizovano na 0 ns
    tlm_command cmd;
    unsigned char* tlm_data;

    // Inicijalno

    /* TLM transakcija */
    cmd = TLM_READ_COMMAND;
    pl.set_data_ptr(tlm_data);
    pl.set_command(cmd);
    tlm_address = start_address_address; // start_address_address = DRAM_TABLE
    for(int i = 0; i < DATA_HEIGHT; i++)
    {
        pl.set_address(tlm_address + i);
        cout << "Cache::tlm_address: " << tlm_address + i << endl;
        Cache_DRAM_soc->b_transport(pl, offset); // Citam podatke iz DRAM-a
        // cout << "Cache::Ajde vise" << endl;
        if(pl.get_response_status() == TLM_OK_RESPONSE)
            start_address[i] = *reinterpret_cast<unsigned int*>(tlm_data);
        else
            cout << "Cache::Transakcija neuspesna!" << endl;
    }

    cout << "Cache::Ovde crkavam" << endl;

    for(int i = 0; i < DATA_HEIGHT; i++)
        cout << "Cache::start_address je: " << start_address[i] << endl;




    /* Kraj TLM transakcije */

    unsigned char compressed_stick_index[DATA_DEPTH]; // Niz u kom se nalaze indeksi nenultih elemenata unutar jednog stika
    unsigned char compressed_stick_length;            // Ovde ce se upisati duzina kompresovanog niza podataka
    type* stick_data;                                 // Pokazivac na stapic podataka

    // Prvo treba napuniti linije kesa
    int cnt = 0;
    for(int i = 0; i < CACHE_SIZE; i++)
    {

        /* TLM transakcija */
        cmd = TLM_READ_COMMAND;
        // cout << "Cache::Stigao sam dovde" << endl;
        tlm_address = start_address[i / (CACHE_SIZE / 2)] + cnt * DATA_DEPTH; // start_address_address = DRAM_TABLE
        // cout << "Cache::Stigao sam dovde" << endl;
        pl.set_address(tlm_address);
        pl.set_data_ptr(tlm_data);
        pl.set_command(cmd);
        Cache_DRAM_soc->b_transport(pl, offset);                              // Citam podatke iz DRAM-a
        if(pl.get_response_status() == TLM_OK_RESPONSE)
            stick_data = reinterpret_cast<type*>(tlm_data);
        else
            cout << "Cache::Transakcija neuspesna!" << endl;
        /* Kraj TLM transakcije */

        compress_data_stick(stick_data, cache_mem + i * DATA_DEPTH, compressed_stick_index, compressed_stick_length);
        cache_line_length[i] = compressed_stick_length;
        address_hash[i] = (i / (CACHE_SIZE / 2)) * max_y + cnt;
        if(cnt == 0)
        {
            amount_hash[i] = 2 * W_kn;  // U hardveru petlja 'kn' je u potpunosti ramotana pa ne treba *kn, ovo je samo zbog softverskog modela
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

        cnt = (cnt + 1) % (CACHE_SIZE / 2);
        write_en[i] = false;

        WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                          address_hash[i], i);
    }


    // Sad za ostatak dvoreda
    for(unsigned int y_i = CACHE_SIZE / 2; y_i < max_y; y_i++)
    {
        for(unsigned int d = 0; d < 2; d++)
        {
            unsigned char full = 0;

            // Proveravamo da li ima slobodnih mesta za upis u kes
            for(int i = 0; i < CACHE_SIZE; i++)
            {
                full += (unsigned char)write_en[i];
            }

            if(!full)
            {
                cout << "Cache::Write ceka na slobodno mesto!" << endl;
                wait(write_enable);
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

            cout << "Cache::Write cita podatak: " << "(" << d << ", " << y_i << ")" << endl;

            /* TLM transakcija */
            cmd = TLM_READ_COMMAND;
            tlm_address = start_address[d] + y_i * DATA_DEPTH; // start_address_address = DRAM_TABLE
            pl.set_address(tlm_address);
            pl.set_data_ptr(tlm_data);
            pl.set_command(cmd);
            Cache_DRAM_soc->b_transport(pl, offset);           // Citam podatke iz DRAM-a
            if(pl.get_response_status() == TLM_OK_RESPONSE)
                stick_data = reinterpret_cast<type*>(tlm_data);
            else
                cout << "Cache::Transakcija neuspesna!" << endl;
            /* Kraj TLM transakcije */

            compress_data_stick(stick_data, cache_mem + free_cache_line * DATA_DEPTH, compressed_stick_index, compressed_stick_length);
            *(cache_line_length + free_cache_line) = compressed_stick_length;
            address_hash[free_cache_line] = d * max_y + y_i;
            write_en[free_cache_line] = false;

            switch(y_i)
            {
                case 0:
                case Y_LIMIT1:
                case Y_LIMIT2:
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

            // Iscitaj koji stick zeli read da procita
            unsigned int temp_x = stick_address_cache >> 32;
            unsigned int temp_y = stick_address_cache & 0x00000000ffffffff;

            // Ako je upisa stick koji read zeli, onda odblokiraj read
            if(temp_x * max_y + temp_y == d * max_y + y_i)
                read_enable.notify();

            cout << "CACHE::adresa koja se salje WMEM-u je: " << to_string(address_hash[free_cache_line]) << endl;
            WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                              address_hash[free_cache_line], free_cache_line);
        }
    }


    // Sada normalno
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
                        cout << "Cache::Write ceka na slobodno mesto!" << endl;
                        wait(write_enable);
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

                    cout << "Cache::Write cita podatak: " << "(" << x_i + d << ", " << y_i << ")" << endl;

                    /* TLM transakcija */
                    cmd = TLM_READ_COMMAND;
                    tlm_address = start_address[x_i + d] + y_i * DATA_DEPTH; // start_address_address = DRAM_TABLE
                    pl.set_address(tlm_address);
                    pl.set_data_ptr(tlm_data);
                    pl.set_command(cmd);
                    Cache_DRAM_soc->b_transport(pl, offset);                 // Citam podatke iz DRAM-a
                    if(pl.get_response_status() == TLM_OK_RESPONSE)
                        stick_data = reinterpret_cast<type*>(tlm_data);
                    else
                        cout << "Cache::Transakcija neuspesna!" << endl;
                    /* Kraj TLM transakcije */

                    compress_data_stick(stick_data, cache_mem + free_cache_line * DATA_DEPTH, compressed_stick_index, compressed_stick_length);
                    *(cache_line_length + free_cache_line) = compressed_stick_length;
                    address_hash[free_cache_line] = (x_i + d) * max_y + y_i;
                    write_en[free_cache_line] = false;

                    switch(y_i)
                    {
                        case 0:
                        case Y_LIMIT1:
                        case Y_LIMIT2:
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

                    // Iscitaj koji stick zeli read da procita
                    unsigned int temp_x = stick_address_cache >> 32;
                    unsigned int temp_y = stick_address_cache & 0x00000000ffffffff;

                    // Ako je upisa stick koji read zeli, onda odblokiraj read
                    if(temp_x * max_y + temp_y == (x_i + d) * max_y + y_i)
                        read_enable.notify();

                    cout << "CACHE::adresa koja se salje WMEM-u je: " << to_string(address_hash[free_cache_line]) << endl;
                    WMEM_cache_port->write_cache_WMEM(compressed_stick_index, compressed_stick_length,
                                                      address_hash[free_cache_line], free_cache_line);
                }
            }
        }
    }
}

// proces
void cache::read()
{
    bool flag = true;

    while(true)
    {
        if(flag)
        {
            done_pb_cache.write(false);
            flag = false;
        }


        cout << "Cache::Read se zaustavio!" << endl;
        wait(); // ceka dok PB ne zatrazi neki podatak
        cout << "Cache::Read je nastavio sa radom!" << endl;

        // Izdvoji x i y adrese
        unsigned int x = stick_address_cache >> 32;
        unsigned int y = stick_address_cache & 0x00000000ffffffff;

        cout << "Cache::Od read se traze podaci: (" << x << ", " << y << ")" << endl;

        // Trazi na kojoj liniji kesa se nalazi podatak adresiran sa x i y
        /* --------------------------------------------- */
        unsigned int cache_line = INVALID_ADDRESS;
        for(int i = 0; i < CACHE_SIZE; i++)
        {
            if(address_hash[i] == x * max_y + y)
                cache_line = i;
        }


        // Proveri da li si nasao ipak taj podatak
        if(cache_line == (unsigned int)INVALID_ADDRESS)
        {
            cout << "Cache::Invalidna adresa!" << endl;
            wait(read_enable);                          // write proces mora da digne event ako je upisao podataka sa adresom x * x_max + y
            cache_line = write_cache_line;              // ovu promenljivu odredjuje write
        }

        cout << "Cache::Linija kesa na kojoj se nalazi trazeni podatak: " << cache_line << endl;
        /* --------------------------------------------- */

        // Umanji za jedan iskoristivost podatka
        amount_hash[cache_line]--;
        cout << "Cache::Nakon ovog read, amount_hash je: " << to_string(amount_hash[cache_line]) << endl;
        /* --------------------------------------------- */

        // Proveri da li moze write da upisuje novu liniju
        if(amount_hash[cache_line] == 0)
        {
            write_en[cache_line] = true;
            write_enable.notify();
        }

        // Upisi lokaciju prvog elementa niza
        type* stick_data_cache = cache_mem + cache_line * DATA_DEPTH;
        unsigned char stick_length = cache_line_length[cache_line];
        copy_cache_line(cache_mem_read_line, stick_length, stick_data_cache);
        stick_data_cache = cache_mem_read_line;
        cache_pb_port->write_cache_pb(&stick_data_cache, stick_length);         // Ovako saljemo podatke ka PB

        bool done = done_pb_cache.read();
        done_pb_cache.write(!done);                                             // Ne koristimo bukvalno vrednost done signala, vec nam sluzi za generisanje dogadjaja

    }
}

/* ----------------------------------------------------------------------------------- */
/* Implementacija PB <-> Cache interfejsa */

void cache::write_pb_cache(const uint64 &stick_address)
{
    stick_address_cache = stick_address;
    start_read.notify();
}

/* ----------------------------------------------------------------------------------- */

void cache::b_transport_proc(pl_t& pl, sc_time& offset)
{

    // Kada s pise, onda se pise sve odjednom, a ne posebno da se upisuje u svaki registar

    uint64 address = pl.get_address();
    tlm_command cmd = pl.get_command();

    if(cmd == TLM_WRITE_COMMAND)
    {

        switch(address)
        {
            case START_ADDRESS_ADDRESS:
            {
                start_address_length = pl.get_data_length();
                start_address_address = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                pl.set_response_status(TLM_OK_RESPONSE);

                break;
            }
            case START:
            {
                start_event.notify();
                pl.set_response_status(TLM_OK_RESPONSE);

                break;
            }
            case MAX_X:
            {
                max_x = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                pl.set_response_status(TLM_OK_RESPONSE);

                break;
            }
            case MAX_Y:
            {
                max_y = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                pl.set_response_status(TLM_OK_RESPONSE);

                break;
            }
            case RELU:
            {
                relu = (bool)(*(reinterpret_cast<unsigned int*>(pl.get_data_ptr())));
                pl.set_response_status(TLM_OK_RESPONSE);

                break;
            }
            default:

                pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                cout << "Cache::Error while trying to write data" << endl;

                break;
        }

        offset += sc_time(55 * CLK_PERIOD, SC_NS); // Kasnjenje komunikacije softvera i IP je oko 50 clk, a jos 5 clk treba da se upisu podaci
    }
    else
    {
        pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
        cout << "Cache::Wrong command!" << endl;
    }
}
