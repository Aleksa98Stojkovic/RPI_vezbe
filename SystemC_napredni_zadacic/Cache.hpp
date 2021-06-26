#ifndef CACHE_HPP_INCLUDED
#define CACHE_HPP_INCLUDED

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "common.hpp"
#include "interfaces.hpp"
#include "DRAM_data.hpp"

class cache :
    public sc_core::sc_channel,
    public pb_cache_if
{
    public:

        SC_HAS_PROCESS(cache);
        cache(sc_core::sc_module_name);

        /* DRAM <-> Cache interface */
        sc_core::sc_port<cache_DRAM_if> cache_DRAM_port;

        /* PB <-> Cache interface */
        bool last_cache;
        sc_core::sc_out<bool> done_pb_cache;
        sc_core::sc_port<cache_pb_if> cache_pb_port;

        /* Processor <-> Cache interface */
        tlm_utils::simple_target_socket<cache> PROCESS_soc;

    protected:

        void write_cache(dram_word* data, dram_word* cache_line);

        // Declaring functions for hierarchical channel
        void read_pb_cache(const bool &last, sc_core::sc_time &offset_pb);

        // Processes
        void read();
        void write();

        // Events required for communication between processes
        sc_core::sc_event write_enable;
        sc_core::sc_event start_event;
        sc_core::sc_event start_read;

        // Object used for time measuring
        sc_core::sc_time offset;

        /* Interconnect <-> Cache interface */
        unsigned int start_address_address; // address of table which holds starting addresses
        unsigned int height;                // number of rows in image
        unsigned int width;                 // number of columns in image
        bool relu;                          // indicates whether ReLu is used or not

        // Internal resources
        dram_word cache_mem[CACHE_SIZE * (DATA_DEPTH / 5 + 1)];
        unsigned char amount_hash[CACHE_SIZE];      // holds remaining number of reads for every cache line
        unsigned int start_address[DATA_HEIGHT];    // holds starting addresses of data blocks
        std::queue<unsigned char> write_en;         // indicates when and which cache line is free for write function
        std::vector<unsigned char> line;
        unsigned int write_cache_line;
        type cache_mem_read_line[DATA_DEPTH];

        // TLM
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_proc(pl_t&, sc_core::sc_time&);

        // DRAM memory
        DRAM_data* dram;
};


#endif // CACHE_HPP_INCLUDED
