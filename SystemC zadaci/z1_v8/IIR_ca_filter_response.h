#ifndef IIR_CA_FILTER_RESPONSE_H_INCLUDED
#define IIR_CA_FILTER_RESPONSE_H_INCLUDED

#define SC_INCLUDE_FX
#include <systemc>
#include <vector>

SC_MODULE(IIR_ca_filter_response)
{
    public:

        SC_HAS_PROCESS(IIR_ca_filter_response);

        IIR_ca_filter_response(sc_core :: sc_module_name);

        sc_core :: sc_in <bool> clk;
        sc_core :: sc_in <bool> rst;

        sc_core :: sc_in <float> in_data;
        sc_core :: sc_in <sc_dt :: sc_logic> in_valid;
        sc_core :: sc_in <sc_dt :: sc_logic> in_last;
        sc_core :: sc_out <sc_dt :: sc_logic> in_ready;

        sc_core :: sc_out <float> out_data;
        sc_core :: sc_out <sc_dt :: sc_logic> out_valid;
        sc_core :: sc_in <sc_dt :: sc_logic> out_ready;

    protected:
        void calculate();
        std :: vector<float> x, y;
};


#endif // IIR_CA_FILTER_RESPONSE_H_INCLUDED
