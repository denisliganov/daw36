
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_paramobject.h"




class ConfigObject : public WinObject, public ParamObject
{
public:

        ConfigObject();

protected:

        Parameter*          brightness;
        Parameter*          hue;
        ListBoxx*           listBox;
        Knob*               knob1;
        Knob*               knob2;
        Knob*               knob3;
        Knob*               knob4;
        Slider36*           slider1;
        Slider36*           slider2;
        Slider36*           slider3;
        Slider36*           slider4;
        Slider36*           slider5;
        Slider36*           slider6;
        Parameter*          saturation;
        
        int                 xstart;
        int                 ystart;
        int                 border;
        int                 xmax;
        int                 ymax;
        int                 groupSpacing;
        int                 smallSpacing;
        int                 wndW;
        int                 wndH;
        bool                horizPut;

        void                placeObject(Gobj* obj, int ow, int oh);
        void                resetPut();
        void                resetGroup();
        void                goHoriz()   { horizPut = true; }
        void                goVert()    { horizPut = false; }
        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
        void                handleParamUpdate(Param * param = NULL);
};


