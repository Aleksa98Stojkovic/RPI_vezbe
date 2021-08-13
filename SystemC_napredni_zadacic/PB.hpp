#ifndef PB_HPP_INCLUDED
#define PB_HPP_INCLUDED

#include <systemc>
#include "common.hpp"
#include "interfaces.hpp"

class PB :
    public sc_core::sc_channel,
    public cache_pb_if
{
    public:

        SC_HAS_PROCESS(PB);
        PB(sc_core::sc_module_name);

        sc_core::sc_port<pb_cache_if> pb_cache_port; // Port for communication with Cache
        sc_core::sc_port<pb_WMEM_if> pb_WMEM_port;   // Port for communication with WMEM
        sc_core::sc_in<bool> done_pb_cache;
        sc_core::sc_out<bool> conv_finished;

    protected:

        sc_core::sc_time offset;
        std::vector<type> data;
        bool relu;
        type OFM[DATA_HEIGHT - 2][DATA_WIDTH - 2][W_kn];    // used only for modeling purposes
        type bias[W_kn];
        void conv2D();                                      // computes convolution
        void write_cache_pb(std::vector<type> stick_data, sc_core::sc_time &offset_cache);
};

#endif // PB_HPP_INCLUDED
