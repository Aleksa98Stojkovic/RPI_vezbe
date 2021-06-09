#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <vector>
#include <systemc>

#define DRAM_ACCESS_TIME 50
#define CACHE_SIZE 16
#define DATA_HEIGHT 12
#define DATA_WIDTH 14
#define DATA_DEPTH 4
#define Y_LIMIT 13
#define W_kn 2
#define W_kh 3
#define W_kw 3
#define W_kd 4
#define CLK_PERIOD 10
#define BIT_WIDTH 12
#define MASK_DATA (unsigned long long)0x0000000000000fff
#define MASK_NON_ZEROS (unsigned long long)0x000000000000000f
#define MASK_ADDRESS (unsigned long long)0x00000000ffffffff
#define MASK_WRITE_EN (unsigned int)0xff

typedef unsigned int type;
typedef sc_dt::uint64 dram_word;


#endif // COMMON_HPP_INCLUDED
