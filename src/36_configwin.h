
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_paramobject.h"
#include "36_paramswin.h"



class ConfigObject : public ParamObject
{
public:

        ConfigObject();

protected:

        Parameter*          brightness;
        Parameter*          hue;
        Parameter*          saturation;
        Parameter*          bufferSize;

        ParamRadio*         pr;

        ListBoxx*           interpolation;
        ListBoxx*           midiDevices;
        ListBoxx*           outputDevices;

        ParamBox*           buffSizeBox;
        ParamBox*           pbox2;
        ParamBox*           pbox3;
        ParamBox*           pbox4;
        ParamBox*           pbox5;
        ParamBox*           pbox6;

        Knob*               knob1;
        Knob*               knob2;
        Knob*               knob3;
        Knob*               knob4;

        RadioBox*           chooserBox;
        RadioBox*           rb2;

        Button36*           showASIOPanel;

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

        void                putRight(Gobj* obj, int ow, int oh);
        void                putDown(Gobj* obj, int ow, int oh);
        void                returnLeft();
        void                returnTop();
        void                putSpace();

        void                goHoriz()   { horizPut = true; }
        void                goVert()    { horizPut = false; }

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
        void                handleParamUpdate(Param * param = NULL);
};


