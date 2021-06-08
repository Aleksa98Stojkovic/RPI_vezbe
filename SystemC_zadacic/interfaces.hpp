#ifndef INTERFACES_HPP_INCLUDED
#define INTERFACES_HPP_INCLUDED

#include <systemc>
#include "common.hpp"

// Interfejs izmedju pb i kesa : CACHE IS TARGET
class pb_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_pb_cache(const sc_dt::uint64 &stick_address) = 0;
};

// Interfejs izmedju kesa i pb : CACHE IS INITIATOR
class cache_pb_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_cache_pb(type** stick_data, unsigned char &stick_lenght) = 0;
};

// Interfejs izmedju kesa i DRAM-a : CACHE IS INITIATOR
class DRAM_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_DRAM_cache(type** stick_data, const unsigned int &address) = 0; // Za podatke
        virtual void read_DRAM_cache(unsigned int** address_data, const unsigned int &address) = 0; // Za tabelu pocetnih adresa
};

// Interfejs izmedju kesa i memorije za tezine : CACHE IS INITIATOR
class WMEM_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_WMEM_cache(const unsigned char* compressed_stick_address, const unsigned char &compressed_stick_address_length) = 0;
};



#endif
