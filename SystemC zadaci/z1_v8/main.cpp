#include <iostream>
#include "tb.h"

using namespace std;

int sc_main(int argc, char* argv[])
{
    tb t("TB", "ca");
    sc_start(100, sc_core :: SC_US);
    return 0;
}
