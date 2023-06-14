
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
        void                handleClose() { ParamObject::handleClose();  }

private:

        Parameter*          brightness;
        Parameter*          hue;
        Parameter*          saturation;
        Parameter*          bufferSize;
        Parameter*          interpolationSelect;
        Knob*               bufferSizeBox;
        SelectorBox*        interpolationChooserBox;
        ListBoxx*           midiOutDevices;
        ListBoxx*           midiInDevices;
        ListBoxx*           outputDevices;
        ListBoxx*           inputDevices;
        ListBoxx*           renderBox;
        SelectorBox*        renderFormat;
        SelectorBox*        renderQuality;
        Button36*           showASIOPanel;

};


