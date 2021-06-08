#include "Conv2D.h"
#include <unistd.h>
#include <iomanip>
#include <algorithm>

using namespace std;

void Conv2D(const float4D &W, float3D &IFM, vector <t> &b, bool use_bias, bool relu)
{
    float3D OFM(IFM.size(), vector <vector <t>> (IFM[0].size(), vector <t> (W.size())));
    float3D IFM_new(IFM.size() + 2 , vector< vector<t>> (IFM[0].size() + 2, vector <t> (IFM[0][0].size())));

    t min_val, max_val;
    min_val = IFM[0][0][0];
    max_val = IFM[0][0][0];


    // dodavanje nula oko IFM[height][width][channel]
    for(int c = 0; c < (int)IFM_new[0][0].size(); c++) // 0 - 63
    {
        for(int x = 0; x < (int)(IFM_new.size()); x++) // 0 - 119
        {
            for(int y = 0; y < (int)(IFM_new[0].size()); y++) // 0 - 125
            {
                if((x > 0) && (x < ((int)(IFM_new.size()) - 1)) && (y > 0) && (y < ((int)(IFM_new[0].size()) - 1)))
                {
                    IFM_new[x][y][c] = IFM[x - 1][y - 1][c];
                    min_val = min(min_val, IFM[x - 1][y - 1][c]);
                    max_val = max(max_val, IFM[x - 1][y - 1][c]);
                }

                else
                    IFM_new[x][y][c] = 0;
            }
        }
    }

    t sum;

    for(int kn = 0; kn < (int)W.size(); kn++) // Odaberi ulazni kanal
    {
        for(int x = 1; x < (int)IFM_new.size() - 1; x++) // setam se izmedju 1 i 118(max 0 do 119)
        {
            for(int y = 1; y < (int)IFM_new[0].size() - 1; y++) // setam se izmedju 1 i 124(max 0 do 125)
            {
                sum = 0;
                for(int kh = 0; kh < (int)W[0].size(); kh++) // stema se kroz svaki filter stapic 3x3x64
                {
                    for(int kw = 0; kw < (int)W[0][0].size(); kw++) // prodji kroz svaku vrstu 3x3 kernela
                    {
                        for(int kd = 0; kd < (int)W[0][0][0].size(); kd++) // prodji kroz svaku kolonu 3x3 kernela
                        {
                            sum += IFM_new[x - 1 + kh][y - 1 + kw][kd] * W[kn][kh][kw][kd];
                        }
                    }
                }

                if(use_bias)
                {
                    sum += b[kn];
                }

                if(relu)
                {
                    if(sum < 0)
                        OFM[x - 1][y - 1][kn] = 0;
                    else
                        OFM[x - 1][y - 1][kn] = sum;
                }
                else
                    OFM[x - 1][y - 1][kn] = sum;
            }
        }
    }

    cout << "MAX: " << max_val << "MIN: " << min_val << endl;

    IFM.clear();
    IFM = OFM;
    OFM.clear();
    IFM_new.clear();

    /* Moguca optimizacija je da sve ove forove strpam u jedan ili dva ugnjezdena fora*/
}

void Conv2D_Q(const float4D &W, float3D &IFM, vector <t> &b, bool use_bias, int w1, int f1, int w2, int f2, bool relu)
{
    float3D OFM(IFM.size(), vector <vector <t>> (IFM[0].size(), vector <t> (W.size())));
    float3D IFM_new(IFM.size() + 2 , vector< vector<t>> (IFM[0].size() + 2, vector <t> (IFM[0][0].size())));

    num_t q0(w1, f1); // kvantizacija za sirinu registra
    num_t q1(w2, f2); // kvantizacija krajnjeg izlaza

    t max_ifm = IFM[0][0][0];
    t min_ifm = IFM[0][0][0];

    // dodavanje nula oko IFM[height][width][channel]
    for(int c = 0; c < (int)IFM_new[0][0].size(); c++) // 0 - 63
    {
        for(int x = 0; x < (int)(IFM_new.size()); x++) // 0 - 119
        {
            for(int y = 0; y < (int)(IFM_new[0].size()); y++) // 0 - 125
            {

                if((x > 0) && (x < ((int)(IFM_new.size()) - 1)) && (y > 0) && (y < ((int)(IFM_new[0].size()) - 1)))
                {
                    IFM_new[x][y][c] = IFM[x - 1][y - 1][c];

                    // ----------------
                    max_ifm = max(max_ifm, IFM[x - 1][y - 1][c]);
                    min_ifm = min(min_ifm, IFM[x - 1][y - 1][c]);
                    // ----------------
                }
                else
                    IFM_new[x][y][c] = 0;
            }
        }
    }

    t dbg_val; // promenljiva za debag
    t dbg_sum = 0;

    for(int x = 1; x < (int)IFM_new.size() - 1; x++) // setam se izmedju 1 i 118(max 0 do 119)
    {
        for(int y = 1; y < (int)IFM_new[0].size() - 1; y++) // setam se izmedju 1 i 118(max 0 do 119)
        {
            for(int kn = 0; kn < (int)W.size(); kn++) // setam se izmedju 1 i 124(max 0 do 125)
            {
                q0 = 0;
                dbg_val = 0;

                for(int kh = 0; kh < (int)W[0].size(); kh++) // stema se kroz svaki filter stapic 3x3x64
                {
                    for(int kw = 0; kw < (int)W[0][0].size(); kw++) // prodji kroz svaku vrstu 3x3 kernela
                    {
                        for(int kd = 0; kd < (int)W[0][0][0].size(); kd++) // prodji kroz svaku kolonu 3x3 kernela
                        {
                            dbg_val += IFM_new[x - 1 + kh][y - 1 + kw][kd] * W[kn][kh][kw][kd];
                            q0 += IFM_new[x - 1 + kh][y - 1 + kw][kd] * W[kn][kh][kw][kd];
                        }
                    }
                }

                if(use_bias)
                {
                    q0 += b[kn];
                    dbg_val += b[kn];
                }


                if(relu)
                {

                    if(dbg_val < 0)
                        dbg_val = 0;

                    if(q0 < 0)
                        OFM[x - 1][y - 1][kn] = 0;
                    else
                    {
                        q1 = q0;
                        OFM[x - 1][y - 1][kn] = q1;
                    }
                }
                else
                {
                    q1 = q0;
                    OFM[x - 1][y - 1][kn] = q1;
                }

                dbg_sum += my_abs((OFM[x - 1][y - 1][kn] - dbg_val));
            }
        }
    }

    cout << "Greska pri konvoluciji je: " << dbg_sum / (t)(IFM.size() * IFM[0].size() * IFM[0][0].size()) << endl;
    cout << endl;
    cout << "Max vrednost je: " << max_ifm << " Min vrednost je: " << min_ifm << endl;
    cout << endl;

    IFM.clear();
    IFM = OFM;
    OFM.clear();
    IFM_new.clear();
}
