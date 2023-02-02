
#pragma once

#include "36_objects.h"
#include "36_pattern.h"


class MainEdit : public Gobj
{
friend  ControlPanel;
friend  MainWinObject;
friend  Lane;
friend  Transport;
friend  InstrPanel;

protected:

        Timeline*           timeline;
        Grid*               grid;
        Button36*           back;
        Button36*           forth;
        Button36*           btup;
        Button36*           btdown;
        AuxKeys*            keys;
        AuxKeys*            keys1;

        void                remap();
        void                drawSelf(Graphics& g);
        void                handleChildEvent(Gobj * obj, InputEvent& ev);
        void                drawIntermittentHighlight(Graphics& g, int x, int y, int w, int h, int numBars);

public:

        Playhead*           playHead;

        MainEdit();
};


class ScrollTimer : public juce::Timer
{
protected:

        int             timerPeriodMs;

        void            timerCallback();

public:

        ScrollTimer();
};

        Pattern*        Create_Pattern_Base(float tk1, float tk2, int tr1, int tr2, char* name, Pattern* ptBase);
        Pattern*        Create_Pattern_Instance(Pattern* ptMom, float tk1, float tk2, int tr1, int tr2, Pattern* ptBase);
        Note*           Create_Note(float tick, int line, Instrument* instr, int note_val, float length, float vol, float pan, Pattern* ptBase);
        Note*           _Create_Note(float tick, int line, Instrument* instr, int note_val, float length, float vol, float pan, Pattern* ptBase);


