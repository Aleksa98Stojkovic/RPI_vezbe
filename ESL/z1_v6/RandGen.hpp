#ifndef RANDGEN_HPP_INCLUDED
#define RANDGEN_HPP_INCLUDED

#include <systemc>

class RandGen : public sc_core::sc_module
{
	public:
		SC_HAS_PROCESS(RandGen);
		RandGen(sc_core::sc_module_name);

		sc_core::sc_in <bool> gen;
		sc_core::sc_out <int> num;
		sc_core::sc_out <bool> start;

	protected:
		bool flag;
		void generate();
};

#endif // RANDGEN_HPP_INCLUDED
