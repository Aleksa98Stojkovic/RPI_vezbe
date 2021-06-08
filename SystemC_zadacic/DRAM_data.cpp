#include "DRAM_data.hpp"

using namespace std;
using namespace sc_core;

DRAM_data::DRAM_data(sc_module_name name) : sc_channel(name)
{
    cout << "DRAM::Konstruisan je DRAM_data!" << endl;

    int j_len;
    int len;
    unsigned int cnt;
    unsigned char non_zeros;
    sc_dt::uint64 data;

    cnt = 0;
    len = DATA_DEPTH;


    // cout << "DRAM::Sadrzaj DRAM-a je:" << endl;
    for(int i = 0; i < (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT; i++)
    {

        dram[i] = 0;
        non_zeros = 0;

        if(len >= 5)
        {
            j_len = 5;
            len -= 5;
        }
        else
            j_len = len;

        for(int j = 0; j < j_len; j++)
        {
            data = cnt;
            dram[i] |= (MASK_DATA & data) << (BIT_WIDTH * j); // 00 00 0e 71 | 0 000 000 000 000 000

            if(cnt != 0)
                non_zeros++;
            cnt++;
        }

        data = non_zeros;
        dram[i] |= (MASK_NON_ZEROS & data) << 60;

        // cout << dram[i] << endl;

    }

    cnt = 0;
    data = 0;

    for(int i = (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT; i < (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT + DATA_HEIGHT; i++)
    {
        dram[i] = cnt * (DATA_DEPTH / 5 + 1) * DATA_WIDTH;
        cnt++;
    }

    cout << "DRAM::Upisani su podaci u DRAM" << endl;
}

void DRAM_data::read_cache_DRAM(dram_word* data, const unsigned int &address, sc_time &offset)
{
    /*
        Adrese:
            0 - ((DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - 1) = DRAM_data
            (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - ((DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT + DATA_HEIGHT) = DRAM_table
    */

    cout << "DRAM::Citanje DRAM-a na adresi: " << address << endl;

    switch(address)
    {
        case 0 ... (DATA_DEPTH / 5 + 1) * DATA_WIDTH * DATA_HEIGHT - 1:

            offset += sc_time((DATA_DEPTH / 5 + 1) * CLK_PERIOD + DRAM_ACCESS_TIME * CLK_PERIOD, SC_NS);

            for(int i = 0; i < (DATA_DEPTH / 5 + 1); i++)
            {
                data[i] = dram[address + i];
                // cout << "#DRAM::Procitani podaci iz memorije su: " << dram[address + i] << endl;
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
            cout << "DRAM::Invalidna adresa!" << endl;
            break;

    }
}
