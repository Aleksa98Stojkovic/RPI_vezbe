#ifndef DRAM_DATA_HPP_INCLUDED
#define DRAM_DATA_HPP_INCLUDED

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "interfaces.hpp"
#include "common.hpp"

class DRAM_data :
    public sc_core::sc_module
{
    public:

        DRAM_data(sc_core::sc_module_name);

        tlm_utils::simple_target_socket<DRAM_data> DRAM_soc;

    protected:

        type dram_data[DATA_DEPTH * DATA_WIDTH * DATA_HEIGHT]; // Sadrzi podatke o ulazu
        unsigned int dram_table[DATA_HEIGHT]; // Sadrzi podatke o pocetnim adresama

        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_cache(pl_t&, sc_core::sc_time&);
        unsigned char cnt;
};


#endif // DRAM_DATA_HPP_INCLUDED
