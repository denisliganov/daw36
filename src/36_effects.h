
#pragma once

#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_device.h"


class Eff : public Device36
{
protected:

            bool            folded;
            Button36*       foldToggle;
            Button36*       bypassToggle;
            Button36*       wndToggle;
            Slider36*       sliderAmount;

            ContextMenu*    createContextMenu();
            void            activateContextMenuItem(std::string item);
    virtual void            processData(float* in_buff, float* out_buff, int num_frames) {};
    virtual Eff*            makeClone(Eff* eff);
            void            drawSelf(Graphics& g);
            void            mapObjects();
            void            handleMouseUp(InputEvent& ev);
            void            handleMouseDown(InputEvent& ev);
            void            handleMouseDrag(InputEvent& ev);
            void            handleChildEvent(Gobj * obj, InputEvent& ev);
            SubWindow*      createWindow();

public:

            bool            bypass;
            MixChannel*     mixChannel;
            int             muteCount;
            virtual Eff*    clone();
            virtual void    save(XmlElement* xmlEff);
            virtual void    load(XmlElement* xmlEff);

            Eff();
            virtual ~Eff();
            virtual void    setMixChannel(MixChannel* ncell);
            virtual void    process(float* in_buff, float* out_buff, int num_frames);
};

class BasicLP : public Eff
{
public:

            // Filter stuff
            Parameter*  cutoff;
            Parameter*  resonance;
            float   filtCoefTab[5];
            float   lx1, lx2, ly1, ly2; // Left sample history
            float   rx1, rx2, ry1, ry2; // Right sample history

            BasicLP();

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);
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

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::LadderFilter dspCoreCFilter3;
};

class CChorus : public Eff
{
public:

            CChorus();

            Parameter*      delay;
            Parameter*      freq;
            Parameter*      depth;
            Parameter*      drywet;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Chorus* dspCoreChorus;
};

class CFlanger : public Eff
{
public:

            CFlanger();

            Parameter*      frequency;
            Parameter*      feedback;
            Parameter*      modfreq;
            Parameter*      depth;
            Parameter*      drywet;

            BoolParam*      invert;

            float*          fmemory;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Flanger dspCoreFlanger;
};

class CPhaser : public Eff
{
public:

            CPhaser();

            Parameter*      frequency;
            Parameter*      feedback;
            Parameter*      modfreq;
            Parameter*      depth;
            Parameter*      drywet;
            Parameter*      numstages;
            Parameter*      stereo;

            float*          fmemory;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Phaser dspCorePhaser;
};

class EQ1 : public Eff
{
public:

            EQ1();

            Parameter *frequency;	
            Parameter *gain, *bandwidth;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::TwoPoleFilter dspCoreEq1;
};

class GraphicEQ : public Eff
{
public:

            GraphicEQ();

            Parameter *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;
            int f1, f2, f3, f4, f5, f6, f7, f8, f9;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Equalizer dspCoreEqualizer;
};

class EQ3 : public Eff
{
public:

            EQ3();

            Parameter *gain1, *gain2, *gain3, *bandwidth;
            Parameter *freq1, *freq2, *freq3;

            int f1, f2, f3;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Equalizer dspCoreEqualizer;
};

class XDelay : public Eff
{
public:

            XDelay();

            BoolParam*      ppmode;
            Parameter*      delay;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::PingPongEcho dspCorePingPongDelay;
};

class CTremolo : public Eff
{
public:

            CTremolo();

            Parameter*  speed;
            Parameter*  depth;

            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Tremolo dspCoreCTremolo;
};

class Compressor : public Eff
{
public:

            Compressor();
            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::Compressor dspCoreComp;
};

class CWahWah : public Eff
{
public:

            CWahWah();
            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::WahWah dspCoreWah;
};

class CDistort : public Eff
{
public:

            CDistort();
            void    processData(float* in_buff, float* out_buff, int num_frames);
            void    reset();
            void    handleParamUpdate(Parameter* param = NULL);

            rosic::WaveShaper dspCoreDist;
};

class CBitCrusher : public Eff
{
public:

            CBitCrusher();

            Parameter*      decimation;
            Parameter*      quantization;

            void processData(float* in_buff, float* out_buff, int num_frames);
            void reset();
            void handleParamUpdate(Parameter* param = NULL);

            rosic::BitCrusher dspCoreBC;
};

class CStereo : public Eff
{
public:

            CStereo();

            Parameter*      offset;

            void        processData(float* in_buff, float* out_buff, int num_frames);
            void        reset();
            void        handleParamUpdate(Parameter* param = NULL);

            rosic::CombStereoizer dspCoreStereo;
};


class CReverb : public Eff
{
protected:

        // user parameters:
        Parameter *highCut, *lowCut;
        Parameter *roomsize, *preDelay, *drywet, *decay;
        Parameter *lowscale, *highscale;
        
        void    processData(float* in_buff, float* out_buff, int num_frames);
        void    handleParamUpdate(Parameter* param = NULL);
        void    reset();

        rosic::Reverb dspCoreReverb;

public:

        CReverb();
};


