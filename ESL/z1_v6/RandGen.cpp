#include "RandGen.hpp"

using namespace std;
using namespace sc_core;

RandGen::RandGen(sc_module_name name) : sc_module(name)
{
	srand(time(NULL));
	flag = false;
	SC_METHOD(generate);
	sensitive << gen;

	cout << "Konstruisan je RandGen" << endl;
}

void RandGen::generate()
{
	bool t_gen;
	int wait_time = 0;

	t_gen = gen->read(); // 000011101101

	// Ovde ce da postavlja na izlaz
	if(flag)
	{
		start -> write(true);
		num -> write(rand() % 100);
	}

	if(t_gen)
	{
		wait_time = rand() % 6 + 5;
		sc_time t(wait_time, SC_NS);
		flag = true;
		next_trigger(t);
	}
	else
	{
		flag = false;
		start -> write(false);
		next_trigger();
	}

}
