#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "top.h"

int sc_main(int argc, char* argv[])
{
    Top top("top");
    sc_start();
    return 0;
}
