

#pragma once


#include "36_globals.h"
#include "36_env.h"
#include "36_juce_components.h"
#include "36_draw.h"
#include "36_window.h"


class LoadPresetButton;
class SavePresetButton;




class EnvelopeComponent : public Component
{
public:

        Envelope*   env;
        EnvPoint*   activePoint;
        EnvAction   envAction;
        float       envXStart;
        bool        vOnly;
        bool        drawscale;
        bool        drawnumbers;
        int         x;
        int         y;

        EnvelopeComponent();
        EnvelopeComponent(Envelope*       nenv);
        ~EnvelopeComponent();
        void    paint(Graphics& g);
        void    mouseMove(const MouseEvent &);
        void    mouseExit(const MouseEvent &);
        void    mouseDrag(const MouseEvent &);
        void    mouseDown(const MouseEvent &);
        void    setEnvelope(Envelope*    nenv);
        //void mouseDown(const MouseEvent &);
};

class SampleWindow  : public SubWindow
{
public:

        Sample*     sample;

        SampleWindow();
        ~SampleWindow();
        void    SetSample(Sample* smp);
        void    moved();
        void    closeButtonPressed();
};

class Looper : public Component
{
public:

        bool    leftside;
        ComponentDragger dragger;
        SampleWave* wave;

        Looper(bool lside, SampleWave* wave);
        void    setWave(SampleWave * wv);
        void    mouseDrag(const MouseEvent &);
        void    mouseDown(const MouseEvent &);
        void    mouseEnter(const MouseEvent &);
        void    mouseExit(const MouseEvent &);
        void    paint(Graphics& g);
};

class SampleWave : public Component
{
public:

        Sample* sample;

        bool    looped;
        long    loopstart;
        long    loopend;
        Looper* left;
        Looper* right;
        double  wratio;
        double  wratio1;


        SampleWave(Sample* smp);
        void    SetSample(Sample* smp);
        void    SetLoopPoints(long start, long end);
        void    ConstrainLooper(Looper* looper);
        void    Loopers();
        void    paint(Graphics& g);
        void    resized();
};

class ConfigWindow  : public SubWindow
{
        public:

        ConfigWindow();
        ~ConfigWindow();
};

class RenderWindow  : public SubWindow
{
        public:

        RenderWindow();
        ~RenderWindow();

        void ResetToStart();
        RenderComponent* RC;
};

class JuceVstParamWindow    : public SubWindow
{
protected:

public:

    JuceVstParamWindow(Vst2Plugin* vst);
};

class ParamWindow      : public SubWindow
{
protected:

public:

    ParamWindow(Device36* dev);
};



