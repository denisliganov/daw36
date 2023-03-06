
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"



class TemplateWinObj : public ParamObject, WinObject
{
public:

            TemplateWinObj();

protected:

            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj* obj, InputEvent& ev);
};



