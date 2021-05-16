#ifndef CACHE_HPP_INCLUDED
#define CACHE_HPP_INCLUDED

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "common.hpp"
#include "interfaces.hpp"

#define START_ADDRESS_ADDRESS 0x1
#define START 0x2
#define ACK 0x3
#define CLK_PERIOD 10
#define INVALID_ADDRESS 9
#define CACHE_SIZE 9

// Dodati jos jedan header sa zajednickim stvarima

class cache :
    public sc_core::sc_channel,
    public pb_cache_if
{
    public:

        SC_HAS_PROCESS(cache);
        cache(sc_core::sc_module_name);

        // tlm_utils::simple_initiator_socket<cache> DRAM_soc;
        tlm_utils::simple_target_socket<cache> PROCESS_soc;

    protected:

        /* PB <-> Cache interface */
        sc_core::sc_signal<sc_dt::uint64> stick_address_cache; // Sadrzi konkatenirane x i y indekse za trazeni stapic podataka
        type* stick_data_cache; // Adresa prvog elementa u stapicu
        unsigned int stick_lenght_cache; // Duzina tog stapica podataka
        sc_core::sc_signal<bool> done_pb_cache;

        // Deklaracija write i read funkcija za hijararhijski kanal
        void write_pb_cache(const sc_dt::uint64 &stick_address);
        void read_pb_cache(type* stick_data, unsigned int &stick_lenght);

        // Procesi
        void read();
        void write();
        sc_core::sc_event cache_line_ready;

        /* Interconnedct <-> Cache interface */
        unsigned int start_address_address; // Pocetna adresa za tabelu pocetnih adresa
        unsigned int start_address_length;
        bool ack; // ACK bit za potvrdu ispravnosti primljenog podatka
        bool start; // Bit koji oznacava da li je sve spremno za pocetak rada
        unsigned int max_x; // Sadrzi vrednost broja redoav u slici

        // Unutrasnji resursi
        type cache_mem[CACHE_SIZE * 64];
        unsigned int address_hash[CACHE_SIZE]; // x, y -> x * max(x) + y = x * 3 + y = 2 * x + x + y = (x << 1) + x + y
        unsigned char amount_hash[CACHE_SIZE];
        unsigned int start_address[472]; // Cuva pocetne adrese blokova podataka
        bool write_en[CACHE_SIZE];
        unsigned int x, y; // one ce da sacuvaju x i y dobijene preko b_transport_pb

        // TLM
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        void b_transport_pb(pl_t&, sc_core::sc_time&);
        void b_transport_proc(pl_t&, sc_core::sc_time&);
};


#endif // CACHE_HPP_INCLUDED
