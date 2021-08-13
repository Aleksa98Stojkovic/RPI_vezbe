#ifndef INTERFACES_HPP_INCLUDED
#define INTERFACES_HPP_INCLUDED

#include <systemc>
#include "common.hpp"

// Interface between pb and cache : CACHE IS TARGET
class pb_cache_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_pb_cache(const bool &last, sc_core::sc_time &offset_pb) = 0;
};

// Interface between pb and cache : CACHE IS INITIATOR
class cache_pb_if: virtual public sc_core::sc_interface
{
    public:
        virtual void write_cache_pb(std::vector<type> stick_data, sc_core::sc_time &offset_cache) = 0;
};

// Interface between cache and DRAM : CACHE IS INITIATOR
class cache_DRAM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_cache_DRAM(dram_word* data, const unsigned int &address, sc_core::sc_time &offset) = 0;
};

// Interface between pb and weights memory : WMEM IS TARGET
class pb_WMEM_if: virtual public sc_core::sc_interface
{
    public:
        virtual void read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn) = 0;
};


#endif
