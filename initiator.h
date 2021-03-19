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

    SC_CTOR(Initiator) : socket("socket")
    {
        SC_THREAD(thread_process);
    }

    void thread_process()
    {
        tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(10, SC_NS);

        for (int i = 32; i < 96; i += 4)
        {
            tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);

            if (cmd == tlm::TLM_WRITE_COMMAND) data = 0xFF000000 | i;
            trans->set_command( cmd );
            trans->set_address( i );
            trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
            trans->set_data_length( 4 );
            trans->set_streaming_width( 4 );
            trans->set_byte_enable_ptr( 0 );
            trans->set_dmi_allowed( false );
            trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
            socket->b_transport( *trans, delay );

            if ( trans->is_response_error() )
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
            cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
                 << " } , data = " << hex << data << " at time " << sc_time_stamp()
                 << " delay = " << delay << endl;
            wait(delay);
        }
    }
    int data;
};

#endif //DUOLOSSC_TUTORIAL_INITIATOR_H
