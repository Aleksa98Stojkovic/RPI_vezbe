#include "Weights_mem.hpp"
#include <string>

using namespace std;
using namespace sc_core;


WMEM::WMEM(sc_module_name name) : sc_channel(name)
{
    cout << "WMEM::Konstruisan je WMEM!" << endl;

    int cnt = 0;
    for(int kn = 0; kn < W_kn; kn++)
    {
        for(int kh = 0; kh < W_kh; kh++)
        {
            for(int kw = 0; kw < W_kw; kw++)
            {
                for(int kd = 0; kd < W_kd; kd++)
                {
                    W[kn][kh][kw][kd] = cnt;
                    cnt += 2;
                }
            }
        }
    }
}

void WMEM::read_pb_WMEM(std::vector<type> &weights, const unsigned int &kn, const unsigned int &kh, const unsigned int &kw)
{
    weights.clear();

    for(int i = 0; i < W_kd; i++)
    {
        weights.push_back(W[kn][kh][kw][i]);

    }
    cout << "WMEM::Zavrseno citanje tezina!" << endl;

}



