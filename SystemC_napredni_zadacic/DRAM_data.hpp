#ifndef DRAM_DATA_HPP_INCLUDED
#define DRAM_DATA_HPP_INCLUDED

#include <systemc>
#include "interfaces.hpp"
#include "common.hpp"

class DRAM_data :
    public sc_core::sc_channel,
    public cache_DRAM_if
{
    public:

        DRAM_data(sc_core::sc_module_name);

    protected:

        dram_word dram[(DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT + DATA_HEIGHT];                   // DRAM memory
        void read_cache_DRAM(dram_word* data, const unsigned int &address, sc_core::sc_time &offset);    // function for reading DRAM
};


#endif // DRAM_DATA_HPP_INCLUDED
