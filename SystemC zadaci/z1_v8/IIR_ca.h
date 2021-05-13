#ifndef IIR_CA_H_INCLUDED
#define IIR_CA_H_INCLUDED

#include "IIR_ifs.h"
#include <systemc>

class IIR_ca_filter_response;

class IIR_ca : public sc_core :: sc_channel,  public IIR_write_if, public IIR_read_if
{
    public:

        SC_HAS_PROCESS(IIR_ca);

        IIR_ca(sc_core :: sc_module_name);
        void write(const std :: vector<float> &data);
        void read(float &data);

        sc_core :: sc_clock clk;
        sc_core :: sc_signal<bool> rst;

        sc_core :: sc_signal<float> in_data;
        sc_core :: sc_signal<sc_dt :: sc_logic> in_valid;
        sc_core :: sc_signal<sc_dt :: sc_logic> in_last;
        sc_core :: sc_signal<sc_dt :: sc_logic> in_ready;

        sc_core :: sc_signal<float> out_data;
        sc_core :: sc_signal<sc_dt :: sc_logic> out_valid;
        sc_core :: sc_signal<sc_dt :: sc_logic> out_ready;

    protected:
        IIR_ca_filter_response *core; // Ovo je deo koji vrsi proracune
};


#endif // IIR_CA_H_INCLUDED
