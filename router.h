//
// Created by denker on 3/22/21.
//

#ifndef DUOLOSSC_TUTORIAL_ROUTER_H
#define DUOLOSSC_TUTORIAL_ROUTER_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"



template<unsigned int N_TARGETS>
struct Router: sc_module
{
    tlm_utils::simple_target_socket<Router> target_socket;
    tlm_utils::simple_initiator_socket_tagged<Router>* initiator_socket[N_TARGETS];

    SC_CTOR(Router)
    : target_socket("target_socket")
    {
        target_socket.register_b_transport(this, &Router::b_transport);
        target_socket.register_get_direct_mem_ptr(this, &Router::get_direct_mem_ptr);
        target_socket.register_transport_dbg(this, &Router::transport_dbg);

        for (unsigned int i = 0; i < N_TARGETS; i++)
        {
            char txt[20];
            sprintf(txt, "socket_%d", i);
            initiator_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Router>(txt);
            initiator_socket[i]->register_invalidate_direct_mem_ptr(this, &Router::invalidate_direct_mem_ptr, i);
        }
    }

    virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay)
    {
        sc_dt::uint64 address = trans.get_address();
        sc_dt::uint64  masked_address;
        unsigned int target_nr = decode_address(address, masked_address);
        trans.set_address(masked_address);
        (*initiator_socket[target_nr])->b_transport(trans, delay);
        return;
    }

    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi_data)
    {
        sc_dt::uint64 masked_address;
        unsigned int target_nr = decode_address(trans.get_address(), masked_address);
        trans.set_address(masked_address);
        bool status = (*initiator_socket[target_nr])->get_direct_mem_ptr(trans, dmi_data);
        dmi_data.set_start_address(compose_address(target_nr, dmi_data.get_start_address()));
        dmi_data.set_end_address(compose_address(target_nr, dmi_data.get_end_address()));
        return status;
    }

    virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans)
    {
        sc_dt::uint64 masked_address;
        unsigned int target_nr = decode_address(trans.get_address(), masked_address);
        trans.set_address(masked_address);
        return (*initiator_socket[target_nr])->transport_dbg(trans);
    }

    virtual void invalidate_direct_mem_ptr(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
    {
        sc_dt::uint64 bw_start_range = compose_address(id, start_range);
        sc_dt::uint64 bw_end_range = compose_address(id, end_range);
        target_socket->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
        return;
    }

    inline unsigned int decode_address(sc_dt::uint64 address, sc_dt::uint64 &masked_address)
    {
        unsigned int target_nr = static_cast<unsigned int>((address >> 8) & 0x3);
        masked_address = address & 0xFF;
        return target_nr;
    }

    inline sc_dt::uint64 compose_address(unsigned int target_nr, sc_dt::uint64 address)
    {
        return (target_nr << 8) | (address & 0xFF);
    }
};

#endif //DUOLOSSC_TUTORIAL_ROUTER_H
