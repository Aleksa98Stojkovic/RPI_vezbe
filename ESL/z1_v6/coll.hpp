#ifndef COLL_HPP_INCLUDED
#define COLL_HPP_INCLUDED

#include <systemc>


class coll : public sc_core::sc_module
{
	public:
		SC_HAS_PROCESS(coll);
		coll(sc_core::sc_module_name);
		sc_core::sc_in <int> num0;
		sc_core::sc_in <int> num1;
		sc_core::sc_in <int> num2;
		sc_core::sc_export<sc_core::sc_fifo<int>> sort_o;
	protected:
		void process();
		sc_core::sc_fifo<int> fifo;
};

#endif // COLL_HPP_INCLUDED
