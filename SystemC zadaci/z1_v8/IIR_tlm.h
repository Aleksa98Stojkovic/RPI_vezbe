#ifndef IIR_TLM_H_INCLUDED
#define IIR_TLM_H_INCLUDED

#include <systemc>
#include "IIR_ifs.h"

class IIR_tlm : public sc_core :: sc_channel,  public IIR_write_if, public IIR_read_if
{
    public:
        IIR_tlm(sc_core :: sc_module_name);
        void write(const std :: vector<float> &data);
        void read(float &data);
        void filter_response();

    protected:
        std :: vector <float> x, y;
        float response;
};


#endif // IIR_TLM_H_INCLUDED
