#include "Pixel_Shuffle.h"

using namespace std;

void Pixel_Shuffle(float3D &IFM, int block_size)
{

    float3D OFM(block_size * IFM.size(), vector < vector <t>> (block_size * IFM[0].size(), vector <t> (IFM[0][0].size() / (block_size * block_size))));

    vector <t> numbers;
    int val = (int)IFM[0][0].size() / (block_size * block_size);

    for(int new_c = 0; new_c < val; new_c++) // za svaki novi kanal
    {
        for(int x = 0; x < (int)IFM.size(); x++) // za svaku vrstu
        {
            for(int y = 0; y < (int)IFM[0].size(); y++) // za svaku kolonu
            {
                for(int block_c = 0; block_c < block_size * block_size; block_c++) // setam se po kanalima do dubine jednog bloka
                {
                    numbers.push_back(IFM[x][y][block_c * val + new_c]);
                }

                // upis u OFM na pravo mesto
                for(int out_x = 0; out_x < block_size; out_x++)
                {
                    for(int out_y = 0; out_y < block_size; out_y++)
                    {
                        OFM[x * block_size + out_x][y * block_size + out_y][new_c] = numbers[0];
                        numbers.erase(numbers.begin());
                    }
                }
            }
        }
    }

    IFM.clear();
    IFM = OFM;
    OFM.clear();
}
