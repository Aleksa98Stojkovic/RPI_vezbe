#ifndef GENERATOR_HPP_INCLUDED
#define GENERATOR_HPP_INCLUDED

#include "common.hpp"
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include "PB.hpp"
#include "Cache.hpp"
#include "Weights_mem.hpp"

class Generator : public sc_core::sc_module
{
    public:

        SC_HAS_PROCESS(Generator);
        Generator(sc_core::sc_module_name);

        tlm_utils::simple_initiator_socket<cache> gen_soc;
        tlm_utils::simple_initiator_socket<WMEM> mem_soc;

    protected:

        sc_core::sc_signal<bool> signal_channel;
        PB* pb;
        cache* c;
        WMEM* memory;
        void Test();
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
};

#endif // GENERATOR_HPP_INCLUDED
