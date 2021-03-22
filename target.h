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
    int mem[SIZE];
    const sc_time LATENCY;

    SC_CTOR(Memory)
            : socket("socket"), LATENCY(10, SC_NS)
    {
        socket.register_b_transport(this, &Memory::b_transport);
        socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
        socket.register_transport_dbg(this, &Memory::transport_dbg);

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

        if (adr >= sc_dt::uint64(SIZE))
        {
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
        else if (byt != 0)
        {
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            return;
        }
        else if (len >4 || wid < len)
        {
            trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
            return;
        }

        if ( cmd == tlm::TLM_READ_COMMAND )
            memcpy(ptr, &mem[adr], len);
        else if ( cmd == tlm::TLM_WRITE_COMMAND )
            memcpy(&mem[adr], ptr, len);

        trans.set_response_status( tlm::TLM_OK_RESPONSE );
        trans.set_dmi_allowed(true);
    }

    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi_data)
    {
        dmi_data.allow_read_write();
        dmi_data.set_dmi_ptr(reinterpret_cast<unsigned char*>(&mem[0]));
        dmi_data.set_start_address(0);
        dmi_data.set_end_address(SIZE*4-1);
        dmi_data.set_read_latency(LATENCY);
        dmi_data.set_write_latency(LATENCY);

        return true;
    }

    virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address() / 4;
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned int num_bytes = (len < SIZE - adr) ? len : SIZE - adr;

        if (cmd == tlm::TLM_READ_COMMAND)
            memcpy(ptr, &mem[adr], num_bytes);
        else if (cmd == tlm::TLM_WRITE_COMMAND)
            memcpy(&mem[adr], ptr, num_bytes);
        return num_bytes;
    }
};

#endif //DUOLOSSC_TUTORIAL_TARGET_H
