#include "Cache.hpp"
#include <tlm>

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

cache::cache(sc_module_name name) :
    sc_module(name),
    //DRAM_soc("DRAM_soc"),
    PROCESS_soc("PROCESS_soc")
{
    //PB_soc.register_b_transport(this, &cache::b_transport_pb);
    PROCESS_soc.register_b_transport(this, &cache::b_transport_proc);

    SC_THREAD(write);
    sensitive << start_event;
    dont_initialize();
    SC_THREAD(read);
    sensitive << stick_address_cache;
    dont_initialize();

    for(int i = 0; i < 9; i++)
        write_en[i] = false;

    ack = false;
    cout << "Registrovane su b_transport metode za cache modul!" << endl;
}

// Implementacija funkcije za kompresiju stapica podataka
void cache::compress_data_stick(type* data_stick, type* cache_mem_pos, unsigned char* compressed_stick_index, unsigned char &compressed_stick_lenght)
{
    unsigned char length = 0;
    for(int i = 0; i < 64; i++)
    {
        if(data_stick[i] != 0)
        {
            // [0, 1, 1, 0, 0, 0, -1] -> 1, 1, -1 | 1, 2, 6
            *(cache_mem_pos + length) = data_stick[i];
            compressed_stick_index[length] = i;
            length++;
        }
    }

    compressed_stick_lenght = length;
}

// proces
void cache::write()
{

    // Inicijalno

    DRAM_cache_port->read_DRAM_cache(start_address, start_address_address); // Citam podatke iz DRAM-a
    unsigned char compressed_stick_index[64]; // Niz u kom se nalaze indeksi nenultih elemenata unutar jednog stika
    unsigned char compressed_stick_length;
    type* stick_data;

    // Prvo treba napuniti linije kesa
    for(int i = 0; i < CACHE_SIZE; i++)
    {
        int cnt = 0;
        DRAM_cache_port->read_DRAM_cache(stick_data, start_address[i / 3] + cnt); // 0, 1, 2, 3 | 0 red -> 0 + 0, 0 + 1, 0 + 2
        cnt = (cnt + 1) % 3;
        compress_data_stick(stick_data, cache_mem + i * 64, compressed_stick_index, compressed_stick_length); // Treba sacuvati i u kesu taj length !!!!!!!!!!!!!!!!!!!!!
        address_hash[i] = (i / 3) * max_x + cnt; // (x, y)
        amount_hash[i] = cnt + 1;
        WMEM_cache_port->write_WMEM_cache(compressed_stick_index, compressed_stick_length);
    }

    // Sad za ostatak

    for(unsigned int x_i = 3; x_i < max_x; x_i++)
    {
        for(unsigned int y_i = 3; y_i < max_y; y_i++)
        {
            for(unsigned int d = 0; d < 3; d++)
            {

                // Ceka da se oslobodi neka linija kesa
                wait(write_enable);

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

                DRAM_cache_port->read_DRAM_cache(stick_data, start_address[x_i + d] + y_i);
                compress_data_stick(stick_data, cache_mem + free_cache_line * 64, compressed_stick_index, compressed_stick_length); // Treba sacuvati i u kesu taj length !!!!!!!!!!!!!!!!!!!!!
                address_hash[free_cache_line] = (x_i + d) * max_x + y_i;

                switch(y_i)
                {
                    case 0:
                    case Y_LIMIT1:
                    case Y_LIMIT3:
                        {
                        amount_hash[free_cache_line] = 1;
                        }
                        break;

                    case 1:
                    case Y_LIMIT2:
                    case Y_LIMIT4:
                        {
                            amount_hash[free_cache_line] = 2;
                        }
                        break;

                    default:
                        {
                            amount_hash[free_cache_line] = 3;
                        }
                        break;

                }

                // Iscitaj koji stick zeli read da procita
                unsigned int temp_x = (stick_address_cache.read() & 0xffffffff00000000) >> 32;
                unsigned int temp_y = stick_address_cache.read() & 0x00000000ffffffff;

                // Ako je upisa stick koji read zeli, onda odblokiraj read
                if(temp_x * max_x + temp_y == (x_i + d) * max_x + y_i)
                    read_enable.notify();

                WMEM_cache_port->write_WMEM_cache(compressed_stick_index, compressed_stick_length);

            }
        }
    }
}

// proces
void cache::read()
{
    while(true)
    {
        done_pb_cache.write(false);
        wait(); // ceka dok ne dobije neki podatak

        // Izdvoji x i y adrese
        unsigned int x = (stick_address_cache.read() & 0xffffffff00000000) >> 32;
        unsigned int y = stick_address_cache.read()  & 0x00000000ffffffff;

        // Trazi na kojoj liniji kesa se nalazi podatak adresiran sa x i y
        unsigned int cache_line = INVALID_ADDRESS;
        for(int i = 0; i < CACHE_SIZE; i++)
        {
            if(address_hash[i] == x * max_x + y)
                cache_line = i;
        }

        // Proveri da li si nasao ipak taj podatak
        if(cache_line == INVALID_ADDRESS)
            wait(read_enable); // write proces mora da digne event ako je upisao podataka sa adresom x * x_max + y, u stick_address_cache se nalaze koje x i y trazimo

        // Umanji za jedan iskoristivost podatka
        amount_hash[cache_line]--;

        // Proveri da li moze write da upisuje novu liniju
        if(amount_hash[cache_line] == 0)
        {
            write_en[cache_line] = true;
            write_enable.notify();
        }

        // Upisi lokaciju prvog elementa niza
        stick_data_cache = cache_mem + cache_line * 64;
        done_pb_cache.write(true);

    }
}

/* ----------------------------------------------------------------------------------- */
/* Implementacija PB <-> Cache interfejsa */

void cache::write_pb_cache(const uint64 &stick_address)
{
    stick_address_cache.write(stick_address); // Upisi x i y koje PB trazi
}

void cache::read_pb_cache(type* stick_data, unsigned int &stick_lenght)
{
    stick_data = stick_data_cache;
    stick_lenght = stick_lenght_cache;
}

/* ----------------------------------------------------------------------------------- */

void cache::b_transport_proc(pl_t& pl, sc_time& offset)
{

    uint64 address = pl.get_address();
    tlm_command cmd = pl.get_command();
    // unsigned char* data = pl.get_data_ptr();

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
                    // unsigned char*
                    // reinterpret_cast<unsigned char*> (data);
                    pl.set_response_status(TLM_OK_RESPONSE);

                    // offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }

                case ACK:
                {
                    unsigned char* data = pl.get_data_ptr();
                    *data = (unsigned char)ack;
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }
                default:

                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Error while trying to read data" << endl;

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

                    cout << start_address_address << endl;
                    cout << start_address_length << endl;

                    ack = true;

                    offset += sc_time(2 * CLK_PERIOD, SC_NS); // Samo adresa se salje, a ne cela tabela

                    break;
                }
                case START:
                {
                    start_event.notify();
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }
                case MAX_X:
                {
                    max_x = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }
                case MAX_Y:
                {
                    max_y = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    offset += sc_time(CLK_PERIOD, SC_NS);

                    break;
                }
                default:

                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Error while trying to write data" << endl;

                    break;
            }

            break;

        }
        default:

            pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            cout << "Wrong command!" << endl;

            break;


    }
}
