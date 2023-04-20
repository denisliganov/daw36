
#pragma once

#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"





class EffEnableButton;
class EffFoldButton;
class EffGuiButton;



class Eff : public Gobj
{
public:
            Eff(Device36* dev);
            virtual ~Eff();

            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
    virtual Eff*                clone();
            MixChannel*         getMixChannel()     { return mixChannel; }
            Device36*           getDevice()         { return device; }
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                isEnabled();
    virtual void                load(XmlElement* xmlEff);
            void                remap();
    virtual void                save(XmlElement* xmlEff);
    virtual void                setMixChannel(MixChannel* ncell);
            void                showDevice(bool show);

protected:
            void                drawSelf(Graphics& g);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseWheel(InputEvent & ev);

            EffGuiButton*       guiButt;
            EffEnableButton*    enableButt;

            Device36*           device;
            MixChannel*         mixChannel;
};


class Filter1 : public Device36
{
public:
            Filter1();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          filterType;
            Parameter*          x2toggle;
            Parameter*          cutoff;
            Parameter*          resonance;
            Parameter*          bandwidth;
            bool                f_master;
            Filter1*            f_next;

            rosic::LadderFilter dspCoreCFilter3;
};


class CChorus : public Device36
{
public:
            CChorus();
            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          drywet;
            Parameter*          delay;
            Parameter*          freq;

            rosic::Chorus*      dspCoreChorus;
};

class CFlanger : public Device36
{
public:
            CFlanger();
            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          drywet;
            Parameter*          frequency;
            Parameter*          feedback;
            float*              fmemory;
            bool                invert;
            Parameter*          modfreq;

            rosic::Flanger      dspCoreFlanger;
};

class CPhaser : public Device36
{
public:
            CPhaser();
            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          drywet;
            Parameter*          frequency;
            Parameter*          feedback;
            float*              fmemory;
            Parameter*          modfreq;
            Parameter*          numstages;
            Parameter*          stereo;

            rosic::Phaser       dspCorePhaser;
};

class EQ1 : public Device36
{
public:
            EQ1();
            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter           *bandwidth;
            Parameter           *frequency;
            Parameter           *gain;

            rosic::TwoPoleFilter dspCoreEq1;
};

class GraphicEQ : public Device36
{
public:
            GraphicEQ();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            int                 f1, f2, f3, f4, f5, f6, f7, f8, f9;
            Parameter           *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;

            rosic::Equalizer    dspCoreEqualizer;
};

class EQ3 : public Device36
{
public:
            EQ3();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();
            void                drawOverChildren(Graphics & g);
            void                drawCurve(Graphics& g, int x, int y, int w, int h);

protected:
            Parameter           *freqHigh, *freqCentr, *freqLow;
            int                 f1, f2, f3;
            Parameter           *gainHigh, *gainCentr, *gainLow, *centrBW;

            rosic::Equalizer    dspCoreEqualizer;
};

class XDelay : public Device36
{
public:
            XDelay();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:

            Parameter*          delay;
            Parameter*          amount;
            Parameter*          feedback;
            Parameter*          pan;
            Parameter*          lowcut;
            Parameter*          drywet;
            Parameter*        delayMode;
            Parameter*      selectTypes;

            rosic::PingPongEcho dspCorePingPongDelay;
};

class CTremolo : public Device36
{
public:
            CTremolo();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          speed;

            rosic::Tremolo      dspCoreCTremolo;
};

class Compressor : public Device36
{
public:
            Compressor();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          thresh;
            Parameter*          knee;
            Parameter*          ratio;
            Parameter*          gain;
            Parameter*          attack;
            Parameter*          release;

            rosic::Compressor   dspCoreComp;

};

class CWahWah : public Device36
{
public:
            CWahWah();

            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();
            void                handleParamUpdate(Parameter* param = NULL);

protected:
            Parameter*          modfreq;
            Parameter*          depth;

            rosic::WahWah       dspCoreWah;
};

class CDistort : public Device36
{
public:
            CDistort();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          drive;
            Parameter*          postgain;
            Parameter*          slope;

            rosic::WaveShaper   dspCoreDist;
};

class CBitCrusher : public Device36
{
public:
            CBitCrusher();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          decimation;
            Parameter*          quantization;

            rosic::BitCrusher   dspCoreBC;
};

class CStereo : public Device36
{
public:
            CStereo();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          offset;

            rosic::CombStereoizer   dspCoreStereo;
};


class CReverb : public Device36
{
public:
            CReverb();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter          *highCut, *lowCut;
            Parameter          *roomsize, *preDelay, *drywet, *decay;
            Parameter          *lowscale, *highscale;

            rosic::Reverb       dspCoreReverb;
};


