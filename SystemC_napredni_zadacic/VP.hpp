#ifndef VP_HPP_INCLUDED
#define VP_HPP_INCLUDED

#include <systemc>
#include "common.hpp"
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "PB.hpp"
#include "Cache.hpp"
#include "Weights_mem.hpp"
#include "Interconnect.hpp"

class VP : public sc_core::sc_module
{
    public:

        VP(sc_core::sc_module_name);

        tlm_utils::simple_target_socket<VP> CPU_soc;

    protected:

        tlm_utils::simple_initiator_socket<VP> Interconnect_soc;
        sc_core::sc_signal<bool> signal_channel;
        sc_core::sc_signal<bool> pb_interrupt;
        sc_core::sc_signal<bool> wmem_interrupt;
        PB pb;
        cache cache_mem;
        WMEM memory;
        Interconnect bus;

        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_cpu(pl_t&, sc_core::sc_time&);

};


#endif // VP_HPP_INCLUDED
