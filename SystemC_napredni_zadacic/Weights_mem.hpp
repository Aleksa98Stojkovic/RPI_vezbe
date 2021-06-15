#ifndef WEIGHTS_MEM_HPP_INCLUDED
#define WEIGHTS_MEM_HPP_INCLUDED

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "common.hpp"
#include "interfaces.hpp"

#define START_ADDRESS_WMEM 0x01

class WMEM :
    public sc_core::sc_channel,
    public pb_WMEM_if
{
    public:

        WMEM(sc_core::sc_module_name);

        /* Processor <-> WMEM interface */
        tlm_utils::simple_target_socket<WMEM> PROCESS_soc;

    protected:

        void read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn);

        type W[W_kn][W_kw][W_kh][W_kd]; // 2 paketa od 9 stapica sa po 2 podatka
        unsigned int start_address_wmem;

        // TLM
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_proc(pl_t&, sc_core::sc_time&);

};


#endif // WEIGHTS_MEM_HPP_INCLUDED
