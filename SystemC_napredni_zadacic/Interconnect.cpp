#include "Interconnect.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

Interconnect::Interconnect(sc_module_name name) : sc_module(name)
{
    CPU_soc.register_b_transport(this, &Interconnect::b_transport_cpu);
    cout << "Interconnect::Interconnect constructed!" << endl;
}

void Interconnect::b_transport_cpu(pl_t &pl, sc_core::sc_time &offset)
{
    uint64 addr = pl.get_address();

    if(addr >= WMEM_BASE && addr < WMEM_BASE + WMEM_REG_NUM)
    {
        pl.set_address(addr - WMEM_BASE);
        WMEM_soc->b_transport(pl, offset);

        offset += sc_time((60 / WMEM_REG_NUM) * CLK_PERIOD, SC_NS);
    }
    else if(addr >= CACHE_BASE && addr < CACHE_BASE + CACHE_REG_NUM)
    {
        pl.set_address(addr - CACHE_BASE);
        CACHE_soc->b_transport(pl, offset);

        offset += sc_time((60 / CACHE_REG_NUM) * CLK_PERIOD, SC_NS);
    }
    else
    {
        cout << "Interconnect::Wrong address!" << endl;
        pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
    }
}

