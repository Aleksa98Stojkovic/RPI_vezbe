#include "common.h"
#include <cmath>

t my_abs(t val)
{
    return (val < 0) ? (-val) : val;
}

void Quantize3D_uniform(float3D &IFM, int w, int f)
{
    num_t quantizer(w, f);

    t val = 0;
    unsigned int cnt = 0;

    for(int x = 0; x < (int)IFM.size(); x++)
    {
        for(int y = 0; y < (int)IFM[0].size(); y++)
        {
            for(int c = 0; c < (int)IFM[0][0].size(); c++)
            {

                cnt++;
                quantizer = IFM[x][y][c];
                val += my_abs(quantizer - IFM[x][y][c]);
                // std :: cout << "Greska je: " << abs(quantizer - IFM[x][y][c]) << std :: endl;
                IFM[x][y][c] = quantizer;

            }
        }
    }

    std :: cout << "Prosecna greska za 3D kvantizaciju je: " << val / ((t)cnt) << std :: endl;
}

void Quantize4D_uniform(float4D &W, int w, int f, bool sp2, const std::vector<t> &val1, const std::vector<t> &val2, std::vector<t> &alfa)
{

    t val = 0;
    t temp;
    unsigned int cnt = 0;

    num_t quantizer(w, f);
    for(int x = 0; x < (int)W.size(); x++)
    {
        for(int y = 0; y < (int)W[0].size(); y++)
        {
            for(int z = 0; z < (int)W[0][0].size(); z++)
            {
                for(int u = 0; u < (int)W[0][0][0].size(); u++)
                {

                    cnt++;

                    if(sp2)
                    {
                        if(x % 2 == 0)
                        {
                            temp = sp2Quantize(val1, val2, W[x][y][z][u], alfa[x / 2]);
                            val += my_abs(temp - W[x][y][z][u]);
                            W[x][y][z][u] = temp;
                        }
                        else
                        {
                            quantizer = W[x][y][z][u];
                            val += my_abs(quantizer - W[x][y][z][u]);
                            W[x][y][z][u] = quantizer;
                        }
                    }
                    else
                    {
                        quantizer = W[x][y][z][u];
                        val += my_abs(quantizer - W[x][y][z][u]);
                        W[x][y][z][u] = quantizer;
                    }
                }
            }
        }
    }

    std :: cout << "Prosecna greska za 4D kvantizaciju je: " << val / ((t)cnt) << std :: endl;
}

void Quantized1D_uniform(std :: vector<t> &b, int w, int f)
{
    t val = 0;
    unsigned int cnt = 0;
    num_t quantizer(w, f);
    for(int x = 0; x < (int)b.size(); x++)
    {
        cnt++;
        quantizer = b[x];
        val += my_abs(quantizer - b[x]);
        b[x] = quantizer;
    }

    std :: cout << "Prosecna greska za 1D kvantizaciju je: " << val / ((t)cnt) << std :: endl;
}

double PSNR(const float3D &Original, const float3D &Quantized)
{
    double psnr = 0;

    for(int x = 0; x < (int)Original.size(); x++)
    {
        for(int y = 0; y < (int)Original[0].size(); y++)
        {
            for(int c = 0; c < (int)Original[0][0].size(); c++)
            {
                psnr += pow(Original[x][y][c] - Quantized[x][y][c], 2.0);
            }
        }
    }

    psnr /= Original.size() * Original[0].size() * Original[0][0].size();
    psnr = 20 * log10(255.0) - 10 * log10(psnr);

    return psnr;
}

void findAlfa(const float4D &W, std::vector<t> &alfa)
{

    t max_val;
    alfa.clear();

    for(int kn = 0; kn < (int)W.size(); kn += 2)
    {
        max_val = my_abs(W[kn][0][0][0]);

        for(int kh = 0; kh < (int)W[0].size(); kh++)
        {
            for(int kw = 0; kw < (int)W[0][0].size(); kw++)
            {
                for(int kd = 0; kd < (int)W[0][0][0].size(); kd++)
                {
                    if(max_val < my_abs(W[kn][kh][kw][kd]))
                    {
                        max_val = my_abs(W[kn][kh][kw][kd]);
                    }
                }
            }
        }

        alfa.push_back(max_val);
    }
}

t sp2Quantize(const std::vector<t> &q1, const std::vector<t> &q2, const t &w_data, const t &alfa)
{

    t data;
    t diff = 20;
    t val;

    // Mozda se moze smanjiti broj iteracija zbog simetrije matrice

    t temp = my_abs(w_data / alfa);

    for(int i = 0; i < (int)q1.size(); i++)
    {
        for(int j = 0; j < (int)q2.size(); j++)
        {
            val = q1[i] + q2[j];
            if(diff  > my_abs(temp - val))
            {
                data = val;
                diff = my_abs(temp - val);
            }
        }
    }

    data *= alfa;

    if(w_data < 0)
        data *= -1;

    // std::cout << "SP2 greska je: " << my_abs(data - w_data) << std::endl;

    return data;
}
