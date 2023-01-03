
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_device.h"




class ConfigObject : public WinObject, public Device36
{
protected:

        Slider36*   slider1;
        Slider36*   slider2;
        Slider36*   slider3;

        Parameter*  hue;
        Parameter*  saturation;
        Parameter*  brightness;

        void    drawSelf(Graphics& g);
        void    handleChildEvent(Gobj * obj,InputEvent & ev);
        void    handleParamUpdate(Parameter * param = NULL);

public:

        ConfigObject();
};


