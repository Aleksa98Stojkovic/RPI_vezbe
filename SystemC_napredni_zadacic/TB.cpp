#include "TB.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

TB::TB(sc_module_name name) : sc_module(name)
{
    cout << "TB::Test bench created" << endl;
    SC_METHOD(Test);
}

void TB::Test()
{
    pl_t pl;
    uint64 address;
    sc_time offset(0, SC_NS);
    tlm_command cmd;
    unsigned int data;
    unsigned int* data_ptr;

    // Writing starting address of weights memory
    cout << "Writing start_address_wmem..." << endl;
    address = START_ADDRESS_WMEM + WMEM_BASE;
    data = 0;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Writing which memory is targeted
    cout << "Writing mem2write..." << endl;
    address = MEM2WRITE + WMEM_BASE;
    data = 0;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Writing address of table which holds starting addresses
    cout << "Writing start_address_address..." << endl;
    address = START_ADDRESS_ADDRESS + CACHE_BASE;
    data = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Writing image height
    cout << "Writing height..." << endl;
    address = HEIGHT + CACHE_BASE;
    data = DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Writing image width
    cout << "Writing width..." << endl;
    address = WIDTH + CACHE_BASE;
    data = DATA_WIDTH;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Writing whether ReLu is used or not
    cout << "Writing relu..." << endl;
    address = RELU + CACHE_BASE;
    data = 1;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Begin computation
    cout << "Starting convolution..." << endl;
    address = START + CACHE_BASE;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    soc->b_transport(pl, offset);
}
