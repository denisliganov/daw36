
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_paramobject.h"



class RenderWinObject : public ParamObject
{
public:

        RenderWinObject();

protected:

        Parameter*          format;
        Parameter*          quality;
        Parameter*          interpolation;

        SelectorBox*        formatBox;
        SelectorBox*        qualityBox;
        SelectorBox*        interpolationBox;

        Button36*           startRender;
        Button36*           openFolder;

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
        void                handleParamUpdate(Parameter * param = NULL);
};


