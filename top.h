//
// Created by denker on 3/17/21.
//

#ifndef DUOLOSSC_TUTORIAL_TOP_H
#define DUOLOSSC_TUTORIAL_TOP_H

#include "initiator.h"
#include "target.h"
#include "router.h"

SC_MODULE(Top)
{
    Initiator *initiator;
    Router<4> *router;
    Memory    *memory[4];

    SC_CTOR(Top)
    {
        initiator = new Initiator("initiator");
        router = new Router<4>("router");
        for (int i = 0; i < 4; i++)
        {
            char txt[20];
            sprintf(txt, "memory_%d", i);
            memory[i] = new Memory(txt);
        }
        initiator->socket.bind(router->target_socket);
        for (int i = 0; i < 4; i++)
        {
            router->initiator_socket[i]->bind(memory[i]->socket);
        }
    }
};

#endif //DUOLOSSC_TUTORIAL_TOP_H
