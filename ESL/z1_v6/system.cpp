#include "system.hpp"

using namespace std;
using namespace sc_core;

sys::sys(sc_module_name name) : sc_module(name), rg0("gen0"), rg1("gen1"), rg2("gen2"), c0("coll")
{
	rg0.gen(gen_i);
	rg0.start(s0);
	rg0.num(num0_s);

	rg1.gen(s0);
	rg1.start(s1);
	rg1.num(num1_s);

	rg2.gen(s1);
	rg2.start(start_o);
	rg2.num(num2_s);

	c0.num0(num0_s);
	c0.num1(num1_s);
	c0.num2(num2_s);

	sort_out(c0.sort_o);

	cout << "Konstruisan je System" << endl;
}
