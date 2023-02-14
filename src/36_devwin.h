
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"


class DevParamObject : public ParamObject
{
public:

            DevParamObject(Device36* dev);

protected:

            Device36*           device;
            ListBoxx*           presetBox;

            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj * obj,InputEvent & ev);
            void                handleClose();
    virtual void                initAll();
};



