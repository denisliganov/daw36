
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"



class ConfigWinObject : public ParamObject, WinObject
{
public:

        ConfigWinObject();

protected:

        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj,InputEvent & ev);
        void                handleParamUpdate(Parameter * param = NULL);

private:

        Parameter*          brightness;
        Parameter*          hue;
        Parameter*          saturation;
        Parameter*          bufferSize;
        Parameter*          interpolationSelect;

        ListBoxx*           interpolation;
        ListBoxx*           midiOutDevices;
        ListBoxx*           midiInDevices;
        ListBoxx*           outputDevices;
        ListBoxx*           inputDevices;

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

};


