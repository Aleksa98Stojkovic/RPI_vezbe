#include <iostream>
#include "Cache.hpp"
#include "Generator.hpp"

using namespace std;

int sc_main(int argc ,char* argv[])
{
	Generator gen("Generator");
	sc_start(1000, sc_core::SC_NS);

	return 0;
}
