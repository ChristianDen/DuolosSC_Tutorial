//
// Created by denker on 3/17/21.
//

#ifndef DUOLOSSC_TUTORIAL_TARGET_H
#define DUOLOSSC_TUTORIAL_TARGET_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

struct Memory: sc_module
{
    tlm_utils::simple_target_socket<Memory> socket;

    enum { SIZE = 256 };

    SC_CTOR(Memory)
            : socket("socket")
    {
        socket.register_b_transport(this, &Memory::b_transport);

        for (int i = 0; i < SIZE; i++)
            mem[i] = 0xAA000000 | (rand() % 256);
    }

    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address() / 4;
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned char*   byt = trans.get_byte_enable_ptr();
        unsigned int     wid = trans.get_streaming_width();

        if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > 4 || wid < len)
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

        if ( cmd == tlm::TLM_READ_COMMAND )
            memcpy(ptr, &mem[adr], len);
        else if ( cmd == tlm::TLM_WRITE_COMMAND )
            memcpy(&mem[adr], ptr, len);

        trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }

    int mem[SIZE];
};

#endif //DUOLOSSC_TUTORIAL_TARGET_H
