#include "Conv2D.h"

using namespace std;

void Conv2D(const float4D &W, float3D &IFM, vector <t> &b, bool use_bias)
{
    float3D OFM(IFM.size(), vector <vector <t>> (IFM[0].size(), vector <t> (W.size())));
    float3D IFM_new(IFM.size() + 2 , vector< vector<t>> (IFM[0].size() + 2, vector <t> (IFM[0][0].size())));

    // Inicijalizacija OFM
    for(int ci = 0; ci < (int)OFM[0][0].size(); ci++)
    {
        for(int x = 0; x < (int)OFM.size(); x++)
        {
            for(int y = 0; y < (int)OFM[0].size(); y++)
            {
                OFM[x][y][ci] = 0;
            }
        }
    }

    // dodavanje nula oko IFM[height][width][channel]
    for(int c = 0; c < (int)IFM_new[0][0].size(); c++) // 0 - 63
    {
        for(int x = 0; x < (int)(IFM_new.size()); x++) // 0 - 119
        {
            for(int y = 0; y < (int)(IFM_new[0].size()); y++) // 0 - 125
            {
                // ako je w = 124 i h = 118, onda gledam da li se nalazim na pikselu sa kordinatama
                // x pripada [1, 122] i y pripada [1, 116]
                if((x > 0) && (x < ((int)(IFM_new.size()) - 1)) && (y > 0) && (y < ((int)(IFM_new[0].size()) - 1)))
                    IFM_new[x][y][c] = IFM[x - 1][y - 1][c];
                else
                    IFM_new[x][y][c] = 0;
            }
        }
    }

    for(int kd = 0; kd < (int)W[0][0][0].size(); kd++) // Odaberi ulazni kanal
    {
        for(int x = 1; x < (int)IFM_new.size() - 1; x++) // setam se izmedju 1 i 118(max 0 do 119)
        {
            for(int y = 1; y < (int)IFM_new[0].size() - 1; y++) // setam se izmedju 1 i 124(max 0 do 125)
            {
                for(int kn = 0; kn < (int)W.size(); kn++) // stema se kroz svaki filter stapic 3x3x64
                {
                    for(int kh = 0; kh < (int)W[0].size(); kh++) // prodji kroz svaku vrstu 3x3 kernela
                    {
                        for(int kw = 0; kw < (int)W[0][0].size(); kw++) // prodji kroz svaku kolonu 3x3 kernela
                        {
                            OFM[x - 1][y - 1][kn] += IFM_new[x - 1 + kh][y - 1 + kw][kd] * W[kn][kh][kw][kd];
                        }
                    }
                }
            }
        }
    }

    // Dodavanje bias dela
    if(use_bias)
    {
        for(int c = 0; c < (int)OFM[0][0].size(); c++)
        {
            for(int x = 0; x < (int)OFM.size(); x++)
            {
                for(int y = 0; y < (int)OFM[0].size(); y++)
                {
                    OFM[x][y][c] += b[c];
                }
            }
        }
    }

    IFM.clear();
    IFM = OFM;
    OFM.clear();
    IFM_new.clear();

    /* Moguca optimizacija je da sve ove forove strpam u jedan ili dva ugnjezdena fora*/
}


void ReLu(float3D &IFM)
{
    for(int c = 0; c < (int)IFM[0][0].size(); c++)
    {
        for(int x = 0; x < (int)IFM.size(); x++)
        {
            for(int y = 0; y < (int)IFM[0].size(); y++)
            {
                if(IFM[x][y][c] < 0)
                    IFM[x][y][c] = 0;
            }
        }
    }
}
