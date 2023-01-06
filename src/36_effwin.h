
#pragma once

#include "36_globals.h"
#include "36_effects.h"
#include "36_window.h"


class EffParamObject : public WinObject
{
protected:

        Eff*        eff;

        void        drawSelf(Graphics& g);
        void        handleChildEvent(Gobj * obj,InputEvent & ev);

public:

        EffParamObject(Eff* eff);
};



