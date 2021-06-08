#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define SC_INCLUDE_FX
#include <systemc>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>


typedef double t;
typedef std :: vector < std :: vector < std :: vector <t>>> float3D;
typedef std :: vector < std :: vector < std :: vector < std :: vector <t>>>> float4D;
typedef sc_dt :: sc_fix_fast num_t;
typedef std :: vector < std :: vector < std :: vector <num_t>>> float3D_Q;
typedef std :: vector < std :: vector < std :: vector < std :: vector <num_t>>>> float4D_Q;

void Quantize3D_uniform(float3D &IFM, int w, int f);
void Quantize4D_uniform(float4D &W, int w, int f, bool sp2, const std::vector<t> &val1, const std::vector<t> &val2, std::vector<t> &alfa);
void Quantized1D_uniform(std :: vector<t> &b, int w, int f);
double PSNR(const float3D &Original, const float3D &Quantized);
t my_abs(t val);
void findAlfa(const float4D &W, std::vector<t> &alfa);
t sp2Quantize(const std::vector<t> &q1, const std::vector<t> &q2, const t &w_data, const t &alfa);

#endif // COMMON_H_INCLUDED
