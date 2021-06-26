#include "PB.hpp"
#include <string>

using namespace std;
using namespace sc_core;

PB::PB(sc_module_name name) : sc_channel(name), offset(0, SC_NS)
{
    SC_THREAD(conv2D);
    sensitive << done_pb_cache;
    dont_initialize();
    relu = true;

    for(int i = 0; i < W_kn; i++)     // initializing bias (for modeling purposes)
        bias[i] = 1;

    cout << "PB::PB constructed!" << endl;
}

void PB::write_cache_pb(std::vector<type> stick_data, sc_time &offset_cache)
{
    offset += offset_cache;
    data = stick_data;
}

void PB::conv2D()
{

    type sum;               // variable used for accumulating result for one output pixel (64)
    vector<type> weights;
    bool last = false;
    conv_finished.write(false);


    for(int x = 0; x < DATA_HEIGHT - 2; x++)
    {

        for(int y = 0; y < DATA_WIDTH - 2; y++)
        {

            if(y == DATA_WIDTH - 3)
                last = true;
            else
                last = false;

            pb_cache_port->read_pb_cache(last, offset);           // requesting data about input from cache
            offset += sc_time(1 * CLK_PERIOD, SC_NS);

            wait(done_pb_cache.default_event());                  // triggers at every edge of done signal


            for(int kn = 0; kn < W_kn; kn++)
            {
                sum = 0;

                pb_WMEM_port->read_pb_WMEM(weights, kn);          // requesting pertinent weights


                for(int i = 0; i < (int)weights.size(); i++)      // computation
                    sum += data[i] * weights[i];

                cout << endl;


                sum += bias[kn];

                if(relu)
                {
                    if(sum < 0)
                        OFM[x][y][kn] = 0;
                    else
                        OFM[x][y][kn] = sum;

                }
                else
                {
                    OFM[x][y][kn] = sum;

                }
            }
        }
    }

    conv_finished.write(true);


    cout << endl << endl << endl << endl;
    cout << "PB::Finished convolution!" << endl << endl;

    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl << endl;

    for(int c = 0; c < W_kn; c++)
    {
        for(int x = 0; x < DATA_HEIGHT - 2; x++)
        {
            for(int y = 0; y < DATA_WIDTH - 2; y++)
            {
                cout << OFM[x][y][c] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }


    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << endl << endl << endl << endl;
}



