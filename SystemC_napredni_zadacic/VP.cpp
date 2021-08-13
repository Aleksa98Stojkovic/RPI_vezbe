#include "VP.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

VP::VP(sc_module_name name) :
    sc_module(name),
    pb("Processing_Block"),
    cache_mem("Cache"),
    memory("Memory"),
    bus("Interconnect")

{
    CPU_soc.register_b_transport(this, &VP::b_transport_cpu);

    pb.pb_cache_port.bind(cache_mem);
    cache_mem.cache_pb_port.bind(pb);
    pb.pb_WMEM_port.bind(memory);
    pb.done_pb_cache.bind(signal_channel);
    cache_mem.done_pb_cache.bind(signal_channel);

    bus.WMEM_soc.bind(memory.PROCESS_soc);
    bus.CACHE_soc.bind(cache_mem.PROCESS_soc);
    Interconnect_soc.bind(bus.CPU_soc);

    pb.conv_finished.bind(pb_interrupt);
    memory.wmem_loaded.bind(wmem_interrupt);

    pb_interrupt.write(false);
    wmem_interrupt.write(true);
    signal_channel.write(true);
    cout << "VP::Virtual Platform constructed!" << endl;
}

void VP::b_transport_cpu(pl_t &pl, sc_core::sc_time &offset)
{
    Interconnect_soc->b_transport(pl, offset);
}
