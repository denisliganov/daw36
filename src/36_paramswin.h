
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_paramobject.h"



class ParamWin : public WinObject
{
public:

            ParamWin();

protected:

            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj* obj, InputEvent& ev);
};



