//
// Created by denker on 3/17/21.
//

#ifndef DUOLOSSC_TUTORIAL_INITIATOR_H
#define DUOLOSSC_TUTORIAL_INITIATOR_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

struct Initiator: sc_module
{
    tlm_utils::simple_initiator_socket<Initiator> socket;
    bool dmi_ptr_valid;

    SC_CTOR(Initiator)
    : socket("socket"),
      dmi_ptr_valid(false)
    {
        socket.register_invalidate_direct_mem_ptr(this, &Initiator::invalidate_direct_mem_ptr);
        SC_THREAD(thread_process);
    }

    void thread_process() {
        tlm::tlm_dmi dmi_data;

        tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(10, SC_NS);

        for (int i = 32; i < 96; i += 4)
        {
            int data;
            tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
            if (cmd == tlm::TLM_WRITE_COMMAND) data = 0xFF000000 | i;

            if (dmi_ptr_valid)
            {
                if (cmd == tlm::TLM_READ_COMMAND)
                {
                    assert(dmi_data.is_read_allowed());
                    memcpy(&data, dmi_data.get_dmi_ptr() + i, 4);
                    wait(dmi_data.get_read_latency());
                }
                else if (cmd == tlm:: TLM_WRITE_COMMAND)
                {
                    assert(dmi_data.is_write_allowed());
                    memcpy(dmi_data.get_dmi_ptr() + i, &data, 4);
                    wait(dmi_data.get_write_latency());
                }
                cout << "DMI = { " << (cmd ? 'W' : 'R') << ", " << hex << i
                     << " } , data = " << hex << data << " at time " << sc_time_stamp()
                     << " delay = " << delay << endl;
            }
            else
            {
                trans->set_command(cmd);
                trans->set_address(i);
                trans->set_data_ptr(reinterpret_cast<unsigned char *>(&data));
                trans->set_data_length(4);
                trans->set_streaming_width(4);
                trans->set_byte_enable_ptr(0);
                trans->set_dmi_allowed(false);
                trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
                socket->b_transport(*trans, delay);

                if (trans->is_response_error()) {
                    char txt[100];
                    sprintf(txt, "Error from b_transport, response status = %s", trans->get_response_string().c_str());
                    SC_REPORT_ERROR("TLM-2", txt);
                }

                if (trans->is_dmi_allowed()) {
                    dmi_data.init();
                    dmi_ptr_valid = socket->get_direct_mem_ptr(*trans, dmi_data);
                }

                cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
                     << " } , data = " << hex << data << " at time " << sc_time_stamp()
                     << " delay = " << delay << endl;
                wait(delay);
            }
        }

        trans->set_address(0);
        trans->set_read();
        trans->set_data_length(128);

        unsigned char *data = new unsigned char[128];
        trans->set_data_ptr(data);
        unsigned int n_bytes = socket->transport_dbg(*trans);

        for (unsigned int i = 0; i < n_bytes; i += 4)
        {
            cout << "mem[" << i << "] = " << *(reinterpret_cast<unsigned int*>(&data[i])) << endl;
        }
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
    {
        dmi_ptr_valid = false;
    }
};

#endif //DUOLOSSC_TUTORIAL_INITIATOR_H
