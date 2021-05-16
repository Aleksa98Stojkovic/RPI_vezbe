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
    SC_THREAD(read);
    sensitive << stick_address_cache;

    for(int i = 0; i < 9; i++)
        write_en[i] = false;

    start = false;
    ack = false;
    cout << "Registrovane su b_transport metode za cache modul!" << endl;
}


// proces
void cache::write()
{

}

// proces
void cache::read()
{
    while(true)
    {
        done_pb_cache.write(false);
        wait(); // ceka dok ne dobije neki podatak

        // Izdvoji x i y adrese
        unsigned int x = (stick_address_cache & 0xffffffff00000000) >> 32;
        unsigned int y = stick_address_cache  & 0x00000000ffffffff;

        // Trazi na kojoj liniji kesa se nalazi podatak adresiran sa x i y
        unsigned int cache_line = INVALID_ADDRESS;
        for(int i = 0; i < CACHE_SIZE; i++)
        {
            if(address_hash[i] == x * max_x + y)
                cache_line = i;
        }

        // Proveri da li si nasao ipak taj podatak
        if(cache_line == INVALID_ADDRESS)
            wait(cache_line_ready); // write proces mora da digne event ako je upisao podataka sa adresom x * x_max + y, u stick_address_cache se nalaze koje x i y trazimo

        // Umanji za jedan iskoristivost podatka
        amount_hash[cache_line]--;

        // Proveri da li moze write da upisuje novu liniju
        if(amount_hash[cache_line] == 0)
        {
            write_en[cache_line] = true;
            // dodati jedan event
        }

        // Upisi lokaciju prvog elementa niza
        stick_data_cache = cache_mem + cache_line * 64;
        done_pb_cache.write(true);

    }
}

/* ----------------------------------------------------------------------------------- */
/* Implementacija PB <-> Cache interfejsa */
void cache::write_pb_cache(const uint64 &stick_address) // ostaviti offset
{
    stick_address_cache.write(stick_address);
}

void cache::read_pb_cache(type* stick_data, unsigned int &stick_lenght) // ostaviti offset
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
                case START: // Ovo je isto samo za debug
                {
                    unsigned char* data = pl.get_data_ptr();
                    *data = (unsigned char)start;
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
                    start = *pl.get_data_ptr();
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
