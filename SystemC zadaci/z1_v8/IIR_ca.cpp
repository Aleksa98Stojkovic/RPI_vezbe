#include "IIR_ca.h"
#include "IIR_ca_filter_response.h"
#include<algorithm>

using namespace std;
using namespace sc_core;
using namespace sc_dt;

IIR_ca :: IIR_ca(sc_module_name name) :
    sc_module(name),
    clk("clk", 100, SC_NS, 0.4, 5, SC_NS, true),
    rst("rst"),
    in_data("in_data"),
    in_valid("in_valid"),
    in_ready("in_ready"),
    in_last("in_last"),
    out_data("out_data"),
    out_valid("out_valid"),
    out_ready("out_ready")
{
    core = new IIR_ca_filter_response("core");

    core -> clk(clk);
    core -> rst(rst);

    core -> in_data(in_data);
    core -> in_last(in_last);
    core -> in_ready(in_ready);
    core -> in_valid(in_valid);

    core -> out_data(out_data);
    core -> out_ready(out_ready);
    core -> out_valid(out_valid);
}

void IIR_ca :: write(const std :: vector<float> &data)
{
    // Podaci se upisuju u jedan niz, pri cemu su prvo vrednosti x-eva, a onda y-na

    rst.write(false);
    out_ready.write(SC_LOGIC_0);
    in_valid.write(SC_LOGIC_0);
    in_last.write(SC_LOGIC_0);

    // Da li ovo simulira kasnjenje prenosa podataka?
    for(int i = 0; i < 10; i++)
        wait(clk.posedge_event());

    for(int i = 0; i < (int)data.size(); i++)
    {
        in_data = data[i];
        in_valid = SC_LOGIC_1;

        if(i == (int)(data.size() - 1))
            in_last = SC_LOGIC_1;
        else
            in_last = SC_LOGIC_0;

        while(true)
        {
            wait(clk.posedge_event());
            if(in_ready == SC_LOGIC_1)
                break;
        }

        cout << "Poslati su podaci" << endl;
        in_valid = SC_LOGIC_0;
        in_last = SC_LOGIC_0;
    }
}

void IIR_ca :: read(float &data)
{
    rst.write(false);
    wait(clk.posedge_event());
    out_ready = SC_LOGIC_1;

    if(out_valid == SC_LOGIC_1)
    {
        // sc_fix_fast q = out_data;
        data = out_data;
    }
    wait(clk.posedge_event());
    out_ready == sc_logic_0;
}
