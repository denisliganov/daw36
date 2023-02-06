
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_paramobject.h"




class ConfigObject : public WinObject, public ParamObject
{
public:

        ConfigObject();

protected:

        Parameter*          brightness;
        Parameter*          hue;
        ListBoxx*           listBox;
        Slider36*           slider1;
        Slider36*           slider2;
        Slider36*           slider3;
        Parameter*          saturation;

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
        void                handleParamUpdate(Param * param = NULL);
};


