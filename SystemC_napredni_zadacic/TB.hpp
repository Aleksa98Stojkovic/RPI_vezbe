#ifndef TB_HPP_INCLUDED
#define TB_HPP_INCLUDED

#include <systemc>
#include "common.hpp"
#include <tlm_utils/simple_initiator_socket.h>

class TB : public sc_core::sc_module
{
    public:

        SC_HAS_PROCESS(TB);
        TB(sc_core::sc_module_name);

        tlm_utils::simple_initiator_socket<TB> soc;

    protected:

        void Test();
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;


};

#endif // TB_HPP_INCLUDED
