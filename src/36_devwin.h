
#pragma once

#include "36_globals.h"
#include "36_paramswin.h"


class DevParamObject : public ParamObject
{
public:

            DevParamObject(Device36* eff);

protected:

            Device36*           device;

            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj * obj,InputEvent & ev);
            void                handleClose();
    virtual void                initAll();
};



