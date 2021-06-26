#include "Weights_mem.hpp"
#include <string>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;


WMEM::WMEM(sc_module_name name) : sc_channel(name)
{

    PROCESS_soc.register_b_transport(this, &WMEM::b_transport_proc);

    cout << "WMEM::WMEM constructed!" << endl;

    int cnt = 0;
    for(int kn = 0; kn < W_kn; kn++)
    {
        for(int kh = 0; kh < W_kh; kh++)
        {
            for(int kw = 0; kw < W_kw; kw++)
            {
                for(int kd = 0; kd < W_kd; kd++)
                {
                    W[kn][kh][kw][kd] = cnt;
                    cnt += 2;
                }
            }
        }
    }
}

void WMEM::read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn)
{
    weights.clear();

    for(int i = 0; i < W_kw; i++)
    {
        for(int j = 0; j < W_kh; j++)
        {
            for(int k = 0; k < W_kd; k++)
            {
                weights.push_back(W[kn][j][i][k]);
            }
        }
    }

    cout << "WMEM::Finished reading weights!" << endl;

}

void WMEM::b_transport_proc(pl_t& pl, sc_core::sc_time& offset)
{
    uint64 address = pl.get_address();
    tlm_command cmd = pl.get_command();

    if(cmd == TLM_WRITE_COMMAND)
    {
        if(address == START_ADDRESS_WMEM)
        {
            start_address_wmem = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
            pl.set_response_status(TLM_OK_RESPONSE);
        }
        else if(address == MEM2WRITE)
        {
            mem2write = *(reinterpret_cast<unsigned int*>(pl.get_data_ptr()));
            pl.set_response_status(TLM_OK_RESPONSE);
        }
        else
        {

            pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            cout << "WMEM::Wrong address!" << endl;
        }

        offset += sc_time(1 * CLK_PERIOD, SC_NS);
    }
    else
    {
        pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
        cout << "WMEM::Wrong command!" << endl;
    }

}




