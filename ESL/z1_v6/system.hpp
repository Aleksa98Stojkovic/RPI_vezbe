#ifndef SYSTEM_HPP_INCLUDED
#define SYSTEM_HPP_INCLUDED

#include <systemc>
#include "coll.hpp"
#include "RandGen.hpp"

class sys : public sc_core::sc_module
{
	public:
        SC_HAS_PROCESS(sys);
		sys(sc_core::sc_module_name);

		sc_core::sc_in<bool> gen_i;
		sc_core::sc_out<bool> start_o;
		sc_core::sc_export<sc_core::sc_fifo<int>> sort_out;

	protected:
		sc_core::sc_signal<int> num0_s;
		sc_core::sc_signal<int> num1_s;
		sc_core::sc_signal<int> num2_s;

		sc_core::sc_signal<bool> s0;
		sc_core::sc_signal<bool> s1;

		RandGen rg0;
		RandGen rg1;
		RandGen rg2;

		coll c0;

};

#endif // SYSTEM_HPP_INCLUDED
