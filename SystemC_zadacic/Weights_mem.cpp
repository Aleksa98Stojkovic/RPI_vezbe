#include "Weights_mem.hpp"
#include <string>

using namespace std;
using namespace sc_core;


WMEM::WMEM(sc_module_name name) : sc_channel(name)
{
    cout << "WMEM::Konstruisan je WMEM!" << endl;

    y_max = DATA_WIDTH;

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

void WMEM::write_cache_WMEM(const unsigned char* compressed_stick_address, const unsigned char &compressed_stick_address_length,
                            const unsigned int &address, const unsigned int &cache_line)
{
    for(int i = 0; i < compressed_stick_address_length; i++)
    {
        compressed_index_memory[cache_line * DATA_DEPTH + i] = compressed_stick_address[i]; // npr. dubina 3, 0-2: prvi stapic, 3-5: drugi stapic...
    }

    cout << "WMEM::adresa koja je primljena od Cache-a je: " << to_string(address) << endl;
    data_stick_address[cache_line] = address;   // address sadrzi x * y_max + y
    compressed_index_len[cache_line] = compressed_stick_address_length;

}

void WMEM::valid_cache_line(const unsigned int &cache_line)
{
    wmem_cache_line = cache_line;

    local_length = compressed_index_len[wmem_cache_line];

    for(int i = 0; i < local_length; i++)
    {
        local_index[i] = compressed_index_memory[wmem_cache_line * DATA_DEPTH + i];
    }

}

void WMEM::read_pb_WMEM(type** compressed_weights, unsigned char &compressed_index_length,
                        const unsigned int &kn, const unsigned int &kh, const unsigned int &kw)
{

    cout << "WMEM::Linija kesa koja je dobije je: " << wmem_cache_line << endl;

    compressed_index_length = local_length;
    for(int i = 0; i < local_length; i++)
    {
        local_weights[i] = W[kn][kh][kw][local_index[i]];
    }

    *compressed_weights = local_weights;


}



