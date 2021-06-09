#ifndef WEIGHTS_MEM_HPP_INCLUDED
#define WEIGHTS_MEM_HPP_INCLUDED

#include <systemc>
#include "common.hpp"
#include "interfaces.hpp"

class WMEM :
    public sc_core::sc_channel,
    public pb_WMEM_if
{
    public:

        WMEM(sc_core::sc_module_name);


    protected:

        void read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn, const unsigned int &kh, const unsigned int &kw);

        type W[W_kn][W_kw][W_kh][W_kd]; // 2 paketa od 9 stapica sa po 2 podatka

};


#endif // WEIGHTS_MEM_HPP_INCLUDED
