#include "Normalization.h"

using namespace std;

void Normalize(float3D &IFM)
{
    for(int wi = 0; wi < (int)IFM.size(); wi++)
    {
        for(int hi = 0; hi < (int)IFM[0].size(); hi++)
        {
            for(int ci = 0; ci < (int)IFM[0][0].size(); ci++)
            {
                IFM[wi][hi][ci] = (IFM[wi][hi][ci] - mean[ci]) / 127.5;
            }
        }
    }
}

void Denormalize(float3D &IFM)
{
    for(int wi = 0; wi < (int)IFM.size(); wi++)
    {
        for(int hi = 0; hi < (int)IFM[0].size(); hi++)
        {
            for(int ci = 0; ci < (int)IFM[0][0].size(); ci++)
            {
                IFM[wi][hi][ci] = 127.5 * IFM[wi][hi][ci] + mean[ci];
            }
        }
    }
}
