
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"




class MakeUpObject : public ParamObject
{
protected:

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);

public:

        MakeUpObject();
};


