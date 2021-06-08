#ifndef LOAD_STORE_H_INCLUDED
#define LOAD_STORE_H_INCLUDED

#include <fstream>
#include <string>
#include <sstream>
#include "common.h"

void LoadInput(std :: string path, float3D &IFM);
void WriteFile(std :: string path, const float3D &OFM);
void LoadFile(std :: string path, float4D &W);
void LoadBias(std :: string path, std :: vector <t> &b);

#endif // LOAD_STORE_H_INCLUDED
