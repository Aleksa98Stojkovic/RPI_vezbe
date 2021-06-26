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

    // Upis start_address_wmem
    cout << "Starting to write start_address_wmem" << endl;
    address = START_ADDRESS_WMEM + WMEM_BASE;
    data = 0;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Upis mem2write
    cout << "Starting to write mem2write" << endl;
    address = MEM2WRITE + WMEM_BASE;
    data = 0;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Upis start_address_address
    cout << "Starting to write start_address_address" << endl;
    address = START_ADDRESS_ADDRESS + CACHE_BASE;
    data = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);


    // Upis height
    cout << "Starting to write height" << endl;
    address = HEIGHT + CACHE_BASE;
    data = DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Upis width
    cout << "Starting to write width" << endl;
    address = WIDTH + CACHE_BASE;
    data = DATA_WIDTH;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);

    // Upis relu
    cout << "Starting to write relu" << endl;
    address = RELU + CACHE_BASE;
    data = 1;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    soc->b_transport(pl, offset);


    // Pocni proracun
    cout << "Starting convolution" << endl;
    address = START + CACHE_BASE;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    soc->b_transport(pl, offset);
}
