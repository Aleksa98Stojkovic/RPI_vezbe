#ifndef INTERFACES_HPP_INCLUDED
#define INTERFACES_HPP_INCLUDED

#include <systemc>
#include "common.hpp"

// Interfejs izmedju kesa i pb
class pb_cache_if: virtual public sc_core::sc_interface
{
    virtual void write_pb_cache(const sc_dt::uint64 &stick_address) = 0;
    virtual void read_pb_cache(type* stick_data, unsigned int &stick_lenght) = 0;
};

#endif
