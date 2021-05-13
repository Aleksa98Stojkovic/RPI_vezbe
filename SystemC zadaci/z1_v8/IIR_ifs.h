#ifndef IIR_IFS_H_INCLUDED
#define IIR_IFS_H_INCLUDED

#include <systemc>
#include <vector>

class IIR_write_if : virtual public sc_core :: sc_interface
{
    public:
        virtual void write(const std :: vector<float> &data) = 0;
};

class IIR_read_if : virtual public sc_core :: sc_interface
{
    public:
        virtual void read(float &data) = 0;
};

#endif // IIR_IFS_H_INCLUDED
