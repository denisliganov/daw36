
#pragma once

#include "36_globals.h"
#include "36_window.h"




class SampleObject : public WinObject
{
protected:

        Slider36*   slider1;
        Slider36*   slider2;
        Slider36*   slider3;

        void    drawSelf(Graphics& g);
        void    handleChildEvent(Gobj * obj,InputEvent & ev);

public:

        SampleObject();
};



