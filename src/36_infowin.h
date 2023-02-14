
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"



class InfoWin : public ParamObject
{
public:

            InfoWin();

protected:

            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj* obj, InputEvent& ev);
};



