#include <iostream>
#include "VP.hpp"
#include "TB.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;

int sc_main(int argc ,char* argv[])
{
	VP vp("Virtual_platform");
	TB tb("Test_bench");

	tb.soc.bind(vp.CPU_soc);

	sc_start();

	return 0;
}
