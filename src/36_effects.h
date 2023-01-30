
#pragma once

#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_device.h"


class Eff : public Device36
{
public:
            Eff();
            virtual ~Eff();

            bool                bypass;
            Button36*           bypassToggle;
            bool                folded;
            Button36*           foldToggle;
            MixChannel*         mixChannel;
            int                 muteCount;
            Slider36*           sliderAmount;
            Button36*           wndToggle;

            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
            SubWindow*          createWindow();
            virtual Eff*        clone();
            void                drawSelf(Graphics& g);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            virtual void        load(XmlElement* xmlEff);
    virtual Eff*                makeClone(Eff* eff);
    virtual void                processData(float* in_buff, float* out_buff, int num_frames) {};
    virtual void                process(float* in_buff, float* out_buff, int num_frames);
            void                remap();
            virtual void        save(XmlElement* xmlEff);
            virtual void        setMixChannel(MixChannel* ncell);
};

class BasicLP : public Eff
{
public:
            BasicLP();

            Parameter*          cutoff;
            Parameter*          resonance;
            float               filtCoefTab[5];
            float               lx1, lx2, ly1, ly2; // Left sample history
            float               rx1, rx2, ry1, ry2; // Right sample history

            void                handleParamUpdate(Parameter* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();
};

class Filter1 : public Eff
{
public:
            Filter1();

            Parameter*      cutoff;
            Parameter*      resonance;
            Parameter*      bandwidth;
            BoolParam*      x2;
            BoolParam*      f1;
            BoolParam*      f2;
            BoolParam*      f3;
            bool            f_master;
            Filter1*        f_next;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();

            rosic::LadderFilter dspCoreCFilter3;
};

class CChorus : public Eff
{
public:
            CChorus();

            Parameter*      depth;
            Parameter*      drywet;
            rosic::Chorus* dspCoreChorus;
            Parameter*      delay;
            Parameter*      freq;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class CFlanger : public Eff
{
public:
            CFlanger();

            Parameter*      depth;
            rosic::Flanger  dspCoreFlanger;
            Parameter*      drywet;
            BoolParam*      invert;
            Parameter*      frequency;
            Parameter*      feedback;
            float*          fmemory;
            Parameter*      modfreq;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class CPhaser : public Eff
{
public:
            CPhaser();

            rosic::Phaser   dspCorePhaser;
            Parameter*      depth;
            Parameter*      drywet;
            Parameter*      frequency;
            Parameter*      feedback;
            float*          fmemory;
            Parameter*      modfreq;
            Parameter*      numstages;
            Parameter*      stereo;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class EQ1 : public Eff
{
public:
            EQ1();

            Parameter      *bandwidth;
            rosic::TwoPoleFilter dspCoreEq1;
            Parameter      *frequency;
            Parameter      *gain;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class GraphicEQ : public Eff
{
public:
            GraphicEQ();

            rosic::Equalizer dspCoreEqualizer;
            int             f1, f2, f3, f4, f5, f6, f7, f8, f9;
            Parameter      *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class EQ3 : public Eff
{
public:
            EQ3();

            rosic::Equalizer dspCoreEqualizer;
            Parameter      *freq1, *freq2, *freq3;
            int             f1, f2, f3;
            Parameter      *gain1, *gain2, *gain3, *bandwidth;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class XDelay : public Eff
{
public:
            XDelay();

            BoolParam*      ppmode;
            Parameter*      delay;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();

            rosic::PingPongEcho dspCorePingPongDelay;
};

class CTremolo : public Eff
{
public:
            CTremolo();

            Parameter*      depth;
            rosic::Tremolo dspCoreCTremolo;
            Parameter*      speed;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class Compressor : public Eff
{
public:
            Compressor();

            rosic::Compressor dspCoreComp;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class CWahWah : public Eff
{
public:
            CWahWah();

            rosic::WahWah   dspCoreWah;

            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
            void            handleParamUpdate(Parameter* param = NULL);
};

class CDistort : public Eff
{
public:
            CDistort();

            rosic::WaveShaper dspCoreDist;

            void            handleParamUpdate(Parameter* param = NULL);
            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
};

class CBitCrusher : public Eff
{
public:
            CBitCrusher();

            rosic::BitCrusher dspCoreBC;
            Parameter*      decimation;
            Parameter*      quantization;

            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
            void            handleParamUpdate(Parameter* param = NULL);
};

class CStereo : public Eff
{
public:
            CStereo();

            rosic::CombStereoizer dspCoreStereo;
            Parameter*      offset;

            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            reset();
            void            handleParamUpdate(Parameter* param = NULL);
};


class CReverb : public Eff
{
public:
            CReverb();

            // user parameters:
            rosic::Reverb   dspCoreReverb;
            Parameter      *highCut, *lowCut;
            Parameter      *roomsize, *preDelay, *drywet, *decay;
            Parameter      *lowscale, *highscale;

            void            processData(float* in_buff, float* out_buff, int num_frames);
            void            handleParamUpdate(Parameter* param = NULL);
            void            reset();
};


