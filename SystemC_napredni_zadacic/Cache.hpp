#ifndef CACHE_HPP_INCLUDED
#define CACHE_HPP_INCLUDED

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "common.hpp"
#include "interfaces.hpp"
#include "DRAM_data.hpp"

#define START_ADDRESS_ADDRESS 0x1
#define START 0x2
#define HEIGHT 0x3
#define WIDTH 0x4
#define DEPTH 0x5
#define RELU 0x6

// Dodati jos jedan header sa zajednickim stvarima

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

        // Deklaracija write i read funkcija za hijararhijski kanal
        void read_pb_cache(const bool &last, sc_core::sc_time &offset_pb);

        // Procesi
        void read();
        void write();

        // Dogadjaji potrebni za komunikaciju izmedju procesa
        sc_core::sc_event write_enable;
        sc_core::sc_event start_event;
        sc_core::sc_event start_read;

        // Objekat koji meri vreme
        sc_core::sc_time offset;

        /* Interconnedct <-> Cache interface */
        unsigned int start_address_address; // Pocetna adresa za tabelu pocetnih adresa
        unsigned int height; // Sadrzi vrednost broja redoav u slici
        unsigned int width; // 124 ili 496
        unsigned int depth;
        bool relu;

        // Unutrasnji resursi
        dram_word cache_mem[CACHE_SIZE * (DATA_DEPTH / 5 + 1)];
        unsigned char amount_hash[CACHE_SIZE];
        unsigned int start_address[DATA_HEIGHT]; // Cuva pocetne adrese blokova podataka
        std::queue<unsigned char> write_en; // Govori kada i koju liniju kesa moze da upisuje write
        std::vector<unsigned char> line;
        unsigned int write_cache_line;
        type cache_mem_read_line[DATA_DEPTH];

        // TLM
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_proc(pl_t&, sc_core::sc_time&);

        // DRAM memorija
        DRAM_data* dram;
};


#endif // CACHE_HPP_INCLUDED
