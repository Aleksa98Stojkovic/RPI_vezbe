#ifndef GENERATOR_H_INCLUDED
#define GENERATOR_H_INCLUDED

#include <systemc>
#include "IIR_ifs.h"

SC_MODULE(generator)
{
    public:
        SC_HAS_PROCESS(generator);

        generator(sc_core::sc_module_name);

        sc_core::sc_port< IIR_write_if > wr_port;
        sc_core::sc_port< IIR_read_if > rd_port;
    protected:
        void test();
        std::vector<int> data;
};

#endif // GENERATOR_H_INCLUDED
