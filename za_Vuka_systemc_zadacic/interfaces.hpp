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
        virtual void write_cache_pb(std::vector<type> stick_data, sc_core::sc_time &offset_cache) = 0;
};

// Interfejs izmedju kesa i DRAM-a : CACHE IS INITIATOR
class cache_DRAM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_cache_DRAM(dram_word* data, const unsigned int &address, sc_core::sc_time &offset) = 0;
};

// Interfejs izmedju pb i memorije za tezine : WMEM IS TARGET
class pb_WMEM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn, const unsigned int &kh, const unsigned int &kw) = 0;
};


#endif
