#include "DRAM_data.hpp"
#include <string>

using namespace std;
using namespace sc_core;
using namespace tlm;

DRAM_data::DRAM_data(sc_module_name name) : sc_module(name)
{
    cout << "DRAM::Konstruisan je DRAM_data!" << endl;
    // Upisati te podatke stvarno u memorije

    DRAM_soc.register_b_transport(this, &DRAM_data::b_transport_cache);

    for(int i = 0; i < DATA_DEPTH * DATA_HEIGHT * DATA_WIDTH; i++)
    {
        dram_data[i] = i;
    }

    cout << "DRAM::Upisani podaci u DRAM_data" << endl;

    for(int i = 0; i < DATA_HEIGHT; i++)
    {
        dram_table[i] = i * DATA_WIDTH * DATA_DEPTH; // x = 0, x = 1, x = 2,
    }

    cnt = 0;

    cout << "DRAM::Upisani podaci u DRAM_table" << endl;
}

void DRAM_data::b_transport_cache(pl_t& pl, sc_core::sc_time& offset)
{

    sc_dt::uint64 address = pl.get_address();
    tlm_command cmd = pl.get_command();

    if(cmd == TLM_READ_COMMAND)
    {

        if((address >= DATA_DEPTH * DATA_HEIGHT * DATA_WIDTH) && (address < DATA_DEPTH * DATA_HEIGHT * DATA_WIDTH + DATA_HEIGHT))
        {

            unsigned int* data = (unsigned int*)(pl.get_data_ptr());
            *data = dram_table[cnt];

            pl.set_response_status(TLM_OK_RESPONSE);

            cnt++;
            if(cnt == DATA_HEIGHT)
            {
                cnt = 0;
                offset += sc_time(50 * CLK_PERIOD, SC_NS);
                cout << "DRAM::Citanje za table" << endl;
            }

        }
        else
        {
            if((address < DATA_DEPTH * DATA_HEIGHT * DATA_WIDTH) && (address >= 0))
            {
                // cout << "DRAM::Stigao sam dovde" << endl;

                type* data = reinterpret_cast<type*>(pl.get_data_ptr());
                data = &dram_data[address];

                pl.set_response_status(TLM_OK_RESPONSE);

                offset += sc_time(50 * CLK_PERIOD, SC_NS);
                cout << "DRAM::Citanje za stapice na adresi: " << to_string(address) << endl;
            }
            else
            {
                pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                cout << "Cache::Error while trying to write data" << endl;
            }
        }
    }
    else
    {
        pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
        cout << "Cache::Wrong command!" << endl;
    }


}
