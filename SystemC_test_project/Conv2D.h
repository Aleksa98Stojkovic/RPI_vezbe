#ifndef CONV2D_H_INCLUDED
#define CONV2D_H_INCLUDED

#include "common.h"

void Conv2D(const float4D &W, float3D &IFM, std :: vector <t> &b, bool use_bias, bool relu);
void Conv2D_Q(const float4D &W, float3D &IFM, std :: vector <t> &b, bool use_bias, int w1, int f1, int w2, int f2, bool relu);


#endif // CONV2D_H_INCLUDED
