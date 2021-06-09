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

    for(int i = 0; i < W_kn; i++)     // inicijalizacija biasa za potrebe modelovanja
        bias[i] = 1;

    cout << "Konstruisan je PB" << endl;
}

void PB::write_cache_pb(std::vector<type> stick_data, sc_time &offset_cache)
{
    offset += offset_cache;
    data = stick_data;
}

void PB::conv2D()
{

    sc_dt::uint64 address; // 8 bajtova, 4 je za x, a 4 je za y
    type sum;   // promenljiva za akumulaciju rezultata jednog izlaznog piksela (64)
    vector<type> weights;


    for(int x = 0; x < DATA_HEIGHT - 2; x++)
    {

        for(int y = 0; y < DATA_WIDTH - 2; y++)
        {

            for(int kn = 0; kn < W_kn; kn++)
            {
                sum = 0;

                for(int kw = 0; kw < W_kw; kw++)
                {

                    for(int kh = 0; kh < W_kh; kh++)
                    {

                        cout << "PB::Adrese: (" << x + kh << ", " << y + kw << ")" << endl;


                        cout << "-----------------------------" << endl;

                        address = 0;
                        address |= (sc_dt::uint64)(x + kh) << 32;
                        address |= (sc_dt::uint64)(y + kw);

                        pb_cache_port->read_pb_cache(address, offset);                          // zahtevamo podatke o ulazu iz kesa

                        offset += sc_time(1 * CLK_PERIOD, SC_NS);

                        wait(done_pb_cache.default_event());                                    // desava se na svaku ivicu signala done

                        pb_WMEM_port->read_pb_WMEM(weights, kn, kh, kw);                        // trazimo odgovarajuce tezine

                        cout << "PB::procitane tezine su: " << endl;
                        for(int i = 0; i < (int)weights.size(); i++)
                            cout << weights[i] << endl;

                        cout << endl;

                        for(int kd = 0; kd < (int)weights.size(); kd++)                                    // sam proracun
                            sum += data[kd] * weights[kd];

                        cout << endl;

                    }
                }


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

    cout << endl << endl << endl << endl;
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



