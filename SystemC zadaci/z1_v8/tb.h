#ifndef TB_H_INCLUDED
#define TB_H_INCLUDED

#include <systemc>

class generator;
class IIR_tlm;
class IIR_ca;

SC_MODULE(tb)
{
    public:
        tb(sc_core::sc_module_name, const std::string& cfg);

    protected:
        generator* g;
        IIR_tlm* tlm;
        IIR_ca* ca;
};

#endif // TB_H_INCLUDED
