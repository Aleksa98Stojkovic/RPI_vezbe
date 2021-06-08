#ifndef INTERFACES_HPP_INCLUDED
#define INTERFACES_HPP_INCLUDED

#include <systemc>
#include "common.hpp"

// Interfejs izmedju pb i kesa : CACHE IS TARGET
class pb_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_pb_cache(const sc_dt::uint64 &stick_address, sc_core::sc_time &offset_pb) = 0;
};

// Interfejs izmedju kesa i pb : CACHE IS INITIATOR
class cache_pb_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_cache_pb(type** stick_data, unsigned char &stick_lenght, sc_core::sc_time &offset_cache) = 0;
};

// Interfejs izmedju kesa i DRAM-a : CACHE IS INITIATOR
class cache_DRAM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_cache_DRAM(dram_word* data, const unsigned int &address, sc_core::sc_time &offset) = 0;
};

// Interfejs izmedju kesa i memorije za tezine : CACHE IS INITIATOR
class WMEM_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_cache_WMEM(const unsigned char* compressed_stick_address, const unsigned char &compressed_stick_address_length,
                                      const unsigned int &address, const unsigned int &cache_line) = 0;
        virtual void valid_cache_line(const unsigned int &cache_line) = 0;
};

// Interfejs izmedju pb i memorije za tezine : WMEM IS TARGET
class pb_WMEM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_pb_WMEM(type** compressed_weights, unsigned char &compressed_index_length,
                                  const unsigned int &kn, const unsigned int &kh, const unsigned int &kw) = 0;
};


#endif
