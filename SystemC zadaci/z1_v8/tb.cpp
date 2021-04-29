#include "tb.h"
#include "generator.h"
#include "IIR_tlm.h"
#include "IIR_ca.h"

using namespace std;
using namespace sc_core;

tb::tb(sc_module_name name, const std::string& cfg) :
  sc_module(name), g(NULL), tlm(NULL), ca(NULL)
{
  g = new generator("generator");
  if(cfg == "ca")
  {
	  ca = new IIR_ca("IIR_ca");
	  g->wr_port(*ca);
	  g->rd_port(*ca);
  }
  else
  {
	  tlm = new IIR_tlm("IIR_tlm");
	  g->wr_port(*tlm);
	  g->rd_port(*tlm);
  }
}
