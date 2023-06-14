
#pragma once

#include "36_globals.h"
#include "36_devwin.h"




class SampleObject : public ParamObject, WinObject
{
public:

        SampleObject();

protected:

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
};



