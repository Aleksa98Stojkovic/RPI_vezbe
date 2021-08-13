#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <vector>
#include <queue>
#include <systemc>
#include <iostream>

#define DRAM_ACCESS_TIME 50
#define CACHE_SIZE 16
#define DATA_HEIGHT 12
#define DATA_WIDTH 14
#define DATA_DEPTH 4
#define W_kn 2
#define W_kh 3
#define W_kw 3
#define W_kd 4
#define CLK_PERIOD 10
#define BIT_WIDTH 12
#define MASK_DATA (unsigned long long)0x0000000000000fff
#define START_ADDRESS_ADDRESS 0x0
#define START 0x1
#define HEIGHT 0x2
#define WIDTH 0x3
#define RELU 0x4
#define WMEM_BASE 0x0
#define CACHE_BASE 0xf
#define WMEM_REG_NUM 2
#define CACHE_REG_NUM 5
#define START_ADDRESS_WMEM 0x0
#define MEM2WRITE 0x1

typedef unsigned int type;
typedef sc_dt::uint64 dram_word;


#endif // COMMON_HPP_INCLUDED
