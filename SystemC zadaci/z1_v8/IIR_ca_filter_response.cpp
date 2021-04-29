#include "IIR_ca_filter_response.h"

using namespace std;
using namespace sc_dt;
using namespace sc_core;

IIR_ca_filter_response :: IIR_ca_filter_response(sc_module_name name) : sc_module(name)
{
    SC_CTHREAD(calculate, clk.pos());
    reset_signal_is(rst, true);
}

void IIR_ca_filter_response :: calculate()
{

    const vector <float> a = {0.2, 0.132, 1.4, -0.32}; // koeficijenti uz y-ne veceg reda
    const vector <float> b = {0.1, -2.2, 0.56, 0.198}; // koeficijenti uz x-eve
    unsigned int num;
    float response;

    while(true)
    {
        out_valid.write(SC_LOGIC_0);
        in_ready.write(SC_LOGIC_1);
        wait();
        num = 0;

        while(true)
        {
            wait();
            if(in_valid.read() == SC_LOGIC_1)
            {
                if(num < a.size())
                {
                    x.push_back(in_data.read());
                    num++;
                }
                else
                {
                    y.push_back(in_data.read());
                    num++;
                    if(in_last.read() == SC_LOGIC_1)
                        break;
                }
            }
        }

        wait();
        cout << "Upisano je " << num << " podataka" << endl;
        in_ready.write(SC_LOGIC_0);

        response = 0;

        for(int i = 0; i < (int)x.size(); i++)
        {
            wait();
            response += b[i] * x[i];
        }
        for(int i = 0; i < (int)y.size(); i++)
        {
            wait();
            response -= a[i] * y[i];
        }

        out_valid.write(SC_LOGIC_1);
        wait();
        out_data.write(response);

    }
}
