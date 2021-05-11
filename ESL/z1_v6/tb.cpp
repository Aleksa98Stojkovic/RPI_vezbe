#include "tb.hpp"

using namespace std;
using namespace sc_core;

tb::tb(sc_module_name name) : sc_module(name), uut("System")
{
	uut.gen_i(sig_in);
	uut.start_o(sig_out);
	SC_THREAD(driver);
	SC_THREAD(monitor);
	sensitive << uut.sort_out;

	cout << "Poceo je TB" << endl;
}

void tb::driver()
{
	while(true)
	{
		sig_in = false;
		wait(5, SC_NS);
		sig_in = true;
		wait(5, SC_NS);
	}
}

void tb::monitor()
{
    while(uut.sort_out->num_available() != 0)
	{
        int data;
		cout << "Vrednost je: ";
        uut.sort_out->nb_read(data);
        cout << data << endl;
	}
}
