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
    dont_initialize();

    SC_THREAD(read);
    sensitive << start_read;


    for(int i = 0; i < CACHE_SIZE; i++)
    {
        write_en.push(i);
        line.push_back(i);
    }

    cout << "Cache::Cache constructed!" << endl;
}

void cache::write_cache(dram_word* data, dram_word* cache_line)
{
    for(int i = 0; i < DATA_DEPTH / 5 + 1; i++)
    {
        *(cache_line + i) = data[i];
    }
}


void cache::write()
{

    dram_word* data;
    int counter = 0;

    // Initial
    data = new dram_word[DATA_HEIGHT];
    cache_DRAM_port->read_cache_DRAM(data, start_address_address, offset); // Reading data from DRAM-a

    for(int i = 0; i < DATA_HEIGHT; i++)
    {
        start_address[i] = data[i];
    }
    delete data;


    cout << "WRITE::Vreme koje je proteklo da se procita start_address: " << offset << endl;


    // Citanje ostatka ulaza
    int cache_init = 0;
    unsigned char free_cache_line;

    for(unsigned int x_i = 0; x_i < DATA_HEIGHT - 2; x_i++)
    {
        for(unsigned int y_i = 0; y_i < DATA_WIDTH; y_i++)
        {
            for(unsigned int d = 0; d < 3; d++)
            {
                free_cache_line = write_en.front();

                if(cache_init < CACHE_SIZE)
                {
                    data = new dram_word;
                    cache_DRAM_port->read_cache_DRAM(data, start_address[x_i + d] + y_i * (DATA_DEPTH / 5 + 1), offset);
                    write_cache(data, cache_mem + free_cache_line * (DATA_DEPTH / 5 + 1));
                    cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
                    delete data;
                //    address_hash[free_cache_line] = (x_i + d) * max_y + y_i;
                    if((y_i == 0) || (y_i == DATA_WIDTH - 1))
                    {
                        amount_hash[free_cache_line] = 1;
                    }
                    else
                    {
                        if((y_i == 1) || (y_i == DATA_WIDTH - 2))
                        {
                            amount_hash[free_cache_line] = 2;
                        }
                        else
                        {
                            amount_hash[free_cache_line] = 3;
                        }
                    }

                    cout << "WRITE::amount_hash je: " << to_string(amount_hash[free_cache_line]) << endl;

                    cache_init++;

                }
                else
                {

                    if(!counter)
                    {
                        cout << "WRITE::Write ceka na slobodno mesto!" << endl;
                        wait(write_enable);

                        if(last_cache)
                            counter = W_kh * W_kw;
                        else
                            counter = W_kh;
                    }

                    cout << "WRITE::Write cita podatak: " << "(" << x_i + d << ", " << y_i << ")" << endl;
                    data = new dram_word;
                    cache_DRAM_port->read_cache_DRAM(data, start_address[x_i + d] + y_i * (DATA_DEPTH / 5 + 1), offset);
                    cout << "WRITE::Vreme koje je proteklo da se procita jedan stapic podataka: " << offset << endl;
                    write_cache(data, cache_mem + free_cache_line * (DATA_DEPTH / 5 + 1));
                    delete data;
                //    address_hash[free_cache_line] = (x_i + d) * max_y + y_i;

                    switch(y_i)
                    {
                        case 0:
                        case DATA_WIDTH - 1:
                            {
                                amount_hash[free_cache_line] = 1;
                            }
                            break;

                        case 1:
                        case DATA_WIDTH - 2:
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

                    cout << "WRITE::amount_hash je: " << to_string(amount_hash[free_cache_line]) << endl;

                    counter--;
                }

                write_en.pop();
                write_en.push(free_cache_line);
            }
        }
    }
}


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

        cout << "READ::Stopping read process!" << endl;
        wait(); // Waiting for data
        cout << "READ::Continuing read process!" << endl;

        vector<type> data_stick;
        unsigned char cache_line;

        for(int n = 0; n < W_kh * W_kw; n++)
        {
            cache_line = line[n];

            offset += sc_time(2 * CLK_PERIOD, SC_NS);

            // Decrease remaining data usage of cache line
            amount_hash[cache_line]--;
            cout << "READ::Amount_hash of cache line: " << to_string(cache_line) << " is: " << to_string(amount_hash[cache_line]) << endl;

            offset += sc_time(1 * CLK_PERIOD, SC_NS);

            // Check whether new data can be written or not
            if(amount_hash[cache_line] == 0)
                write_enable.notify();


            // Write first element location
            dram_word* stick_data_cache = cache_mem + cache_line * (DATA_DEPTH / 5 + 1);
            int j_len;

            for(int i = 0; i < DATA_DEPTH / 5 + 1; i++)
            {
                if(i == DATA_DEPTH / 5)
                    j_len = DATA_DEPTH % 5;
                else
                    j_len = 5;

                for(int j = 0; j < j_len; j++)
                {
                    data_stick.push_back((type)((stick_data_cache[i] & (MASK_DATA << (BIT_WIDTH * j))) >> (BIT_WIDTH * j)));
                    // cout << (type)((stick_data_cache[i] & (MASK_DATA << (BIT_WIDTH * j))) >> (BIT_WIDTH * j)) << endl;
                }

            }


            offset += sc_time((DATA_DEPTH / 5 + 1) * CLK_PERIOD, SC_NS);

        }

        cache_pb_port->write_cache_pb(data_stick, offset);
        data_stick.clear();

        int range;
        unsigned char temp;

        if(last_cache)
            range = W_kh * W_kw;
        else
            range = W_kh;

        for(int i = 0; i < range; i++)
        {
            temp = line[0];
            line.erase(line.begin());
            line.push_back(temp);
        }

        bool done = done_pb_cache.read();
        done_pb_cache.write(!done);

    }
}

/* ----------------------------------------------------------------------------------- */
/* Implementation of PB <-> Cache interface */

void cache::read_pb_cache(const bool &last, sc_time &offset_pb)
{
    offset += offset_pb;
    last_cache = last;
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
                case START_ADDRESS_ADDRESS: // only for debug
                {
                    unsigned int* data = reinterpret_cast<unsigned int*>(pl.get_data_ptr());
                    *data = start_address_address;

                    pl.set_response_status(TLM_OK_RESPONSE);

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
                case HEIGHT:
                {
                    height = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    break;
                }
                case WIDTH:
                {
                    width = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    break;
                }
                case RELU:
                {
                    relu = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
                    pl.set_response_status(TLM_OK_RESPONSE);

                    break;
                }
                default:

                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Cache::Error while trying to write data" << endl;

                    break;
            }

            offset += sc_time(6 * CLK_PERIOD, SC_NS); // only address is sent

            break;

        }
        default:

            pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            cout << "Cache::Wrong command!" << endl;

            break;


    }
}
