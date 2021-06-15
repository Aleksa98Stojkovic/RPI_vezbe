#include "Generator.hpp"

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace sc_dt;

Generator::Generator(sc_module_name name) : sc_module(name)
{
    SC_METHOD(Test);
    pb = new PB("PB");              // Pravim novi PB
    c = new cache("Cache");         // Pravim novi Cache
    memory = new WMEM("Memory");    // Pravim novi WMEM
    pb->pb_cache_port.bind(*c);
    c->cache_pb_port.bind(*pb);
    pb->pb_WMEM_port.bind(*memory);
    pb->done_pb_cache.bind(signal_channel);
    c->done_pb_cache.bind(signal_channel);
    gen_soc.bind(c->PROCESS_soc);
    mem_soc.bind(memory->PROCESS_soc);
    signal_channel.write(true);     // Inicijalizacija za done
    cout << "Kreiran je Generator!" << endl;
}


void Generator::Test()
{
    pl_t pl;
    uint64 address;
    sc_time offset(0, SC_NS);
    tlm_command cmd;
    unsigned int data;
    unsigned int* data_ptr;

    // Upis start_address_wmem
    cout << "Zapocinjem upis u start_address_wmem" << endl;
    address = START_ADDRESS_WMEM;
    data = 0;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Upis start_address_address
    cout << "Zapocinjem upis u start_address_address" << endl;
    address = START_ADDRESS_ADDRESS;
    data = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Upis height
    cout << "Zapocinjem upis u height" << endl;
    address = HEIGHT;
    data = DATA_HEIGHT;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Upis width
    cout << "Zapocinjem upis u width" << endl;
    address = WIDTH;
    data = DATA_WIDTH;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Upis depth
    cout << "Zapocinjem upis u depth" << endl;
    address = DEPTH;
    data = DATA_DEPTH;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Upis relu
    cout << "Zapocinjem upis u relu" << endl;
    address = RELU;
    data = 1;
    data_ptr = &data;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    pl.set_data_ptr(reinterpret_cast<unsigned char*> (data_ptr));
    gen_soc->b_transport(pl, offset);


    // Pocni proracun
    cout << "Zapocinjem konvoluciju" << endl;
    address = START;
    cmd = TLM_WRITE_COMMAND;
    pl.set_command(cmd);
    pl.set_address(address);
    gen_soc->b_transport(pl, offset);

}




