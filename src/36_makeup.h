
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_device.h"




class MakeUpObject : public WinObject, public Device36
{
protected:

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);

public:

        MakeUpObject();
};


