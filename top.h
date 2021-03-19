//
// Created by denker on 3/17/21.
//

#ifndef DUOLOSSC_TUTORIAL_TOP_H
#define DUOLOSSC_TUTORIAL_TOP_H

#include "initiator.h"
#include "target.h"

SC_MODULE(Top)
{
    Initiator *initiator;
    Memory    *memory;

    SC_CTOR(Top)
    {
        initiator = new Initiator("initiator");
        memory    = new Memory   ("memory");

        initiator->socket.bind( memory->socket );
    }
};

#endif //DUOLOSSC_TUTORIAL_TOP_H
