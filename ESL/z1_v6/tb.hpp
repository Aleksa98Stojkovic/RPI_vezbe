#ifndef TB_HPP_INCLUDED
#define TB_HPP_INCLUDED

#include <systemc>
#include "system.hpp"

class tb : public  sc_core::sc_module
{
	public:
		SC_HAS_PROCESS(tb);
		tb(sc_core::sc_module_name);
	protected:
		sys uut;
		sc_core::sc_signal<bool> sig_in, sig_out;


		void driver();
		void monitor();

};

#endif // TB_HPP_INCLUDED
