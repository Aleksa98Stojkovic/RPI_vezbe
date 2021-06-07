#ifndef WEIGHTS_MEM_HPP_INCLUDED
#define WEIGHTS_MEM_HPP_INCLUDED

#include <systemc>
#include "common.hpp"
#include "interfaces.hpp"

class WMEM :
    public sc_core::sc_channel,
    public WMEM_cache_if,
    public pb_WMEM_if
{
    public:

        WMEM(sc_core::sc_module_name);


    protected:

        void write_cache_WMEM(const unsigned char* compressed_stick_address, const unsigned char &compressed_stick_address_length,
                              const unsigned int &address, const unsigned int &cache_line);
        void valid_cache_line(const unsigned int &cache_line);
        void read_pb_WMEM(type** compressed_weights, unsigned char &compressed_index_length,
                          const unsigned int &kn, const unsigned int &kh, const unsigned int &kw);

        type W[W_kn][W_kw][W_kh][W_kd]; // 2 paketa od 9 stapica sa po 2 podatka
        unsigned char compressed_index_memory[CACHE_SIZE * DATA_DEPTH];
        unsigned int data_stick_address[CACHE_SIZE];
        unsigned int y_max;
        unsigned int compressed_index_len[CACHE_SIZE];
        unsigned int wmem_cache_line;
        unsigned char local_length;
        unsigned char local_index[DATA_DEPTH];
        type local_weights[DATA_DEPTH];

};


#endif // WEIGHTS_MEM_HPP_INCLUDED
