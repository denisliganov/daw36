
#pragma once

#include "36_globals.h"
#include "36_effects.h"
#include "36_window.h"


class DevParamObject : public WinObject
{
public:
            DevParamObject(Device36* eff);
protected:
            Device36*   device;
            void        drawSelf(Graphics& g);
            void        handleChildEvent(Gobj * obj,InputEvent & ev);
            void        handleClose();
    virtual void        initAll();
};



