#include "DRAM_data.hpp"

using namespace std;
using namespace sc_core;

DRAM_data::DRAM_data(sc_module_name name) : sc_channel(name)
{
    cout << "DRAM::DRAM_data constructed!" << endl;

    int j_len;
    int len;
    unsigned long long cnt;

    cnt = 0;
    len = DATA_DEPTH;

    std::vector<unsigned long long> temp;

    for(int x = 0; x < DATA_HEIGHT; x++)
    {
        for(int y = 0; y < DATA_WIDTH; y++)
        {
            temp.clear();

            for(int c = 0; c < DATA_DEPTH; c++)
            {
                if((x == 0) || (x == DATA_HEIGHT - 1) || (y == 0) || (y == DATA_WIDTH - 1))
                    temp.push_back(0);
                else
                    temp.push_back(cnt++);
            }


            int index = 0;
            len = DATA_DEPTH;

            while(len > 0)
            {
                dram[(x * DATA_WIDTH + y) * (DATA_DEPTH / 5 + 1) + index] = 0;

                if(len >= 5)
                    j_len = 5;
                else
                    j_len = len;

                for(int j = 0; j < j_len; j++)
                {
                    dram[(x * DATA_WIDTH + y) * (DATA_DEPTH / 5 + 1) + index] |= (MASK_DATA & temp[index * 5 + j]) << (BIT_WIDTH * j);     // (x * y_max + y) * z_max + z
                }

                index++;
                len -= 5;
            }
        }
    }


    cnt = 0;

    for(int i = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT; i < (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT + DATA_HEIGHT; i++)
    {
        dram[i] = cnt * (DATA_DEPTH / 5 + 1) * DATA_WIDTH;
        cnt++;
    }

    cout << "DRAM::Finished writing data into DRAM." << endl;
}

void DRAM_data::read_cache_DRAM(dram_word* data, const unsigned int &address, sc_time &offset)
{
    /*
        Addresses:
            0 - ((DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - 1) = DRAM_data
            (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - ((DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT + DATA_HEIGHT) = DRAM_table
    */

    cout << "DRAM::Reading DRAM data on address: " << address << endl;

    switch(address)
    {
        case 0 ... (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - 1:

            offset += sc_time((DATA_DEPTH / 5 + 1) * CLK_PERIOD + DRAM_ACCESS_TIME * CLK_PERIOD, SC_NS);

            for(int i = 0; i < (DATA_DEPTH / 5 + 1); i++)
            {
                data[i] = dram[address + i];
            }

            break;

        case (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT:

            offset += sc_time(DATA_HEIGHT * CLK_PERIOD + DRAM_ACCESS_TIME * CLK_PERIOD, SC_NS);

            for(int i = 0; i < DATA_HEIGHT; i++)
            {
                data[i] = dram[address + i];
            }

            break;

        default:
            cout << "DRAM::Invalid address!" << endl;
            break;

    }
}
