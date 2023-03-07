
#pragma once

#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"





class Eff : public Gobj
{
public:
            Eff(Device36* dev);
            virtual ~Eff();

            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
            virtual Eff*        clone();
            void                drawSelf(Graphics& g);
            MixChannel*         getMixChannel() { return mixChannel; }
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            virtual void        load(XmlElement* xmlEff);
            void                remap();
            virtual void        save(XmlElement* xmlEff);
            virtual void        setMixChannel(MixChannel* ncell);

            Device36*           device;

private:

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
            Parameter*         filterType;
            Parameter*        x2toggle;
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
            rosic::Chorus*      dspCoreChorus;
            Parameter*          delay;
            Parameter*          freq;
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
            rosic::Flanger      dspCoreFlanger;
            Parameter*          drywet;
            Parameter*          frequency;
            Parameter*          feedback;
            float*              fmemory;
            bool          invert;
            Parameter*          modfreq;
};

class CPhaser : public Device36
{
public:
            CPhaser();
            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            rosic::Phaser       dspCorePhaser;
            Parameter*          depth;
            Parameter*          drywet;
            Parameter*          frequency;
            Parameter*          feedback;
            float*              fmemory;
            Parameter*          modfreq;
            Parameter*          numstages;
            Parameter*          stereo;
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
            rosic::TwoPoleFilter dspCoreEq1;
            Parameter           *frequency;
            Parameter           *gain;
};

class GraphicEQ : public Device36
{
public:
            GraphicEQ();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            rosic::Equalizer    dspCoreEqualizer;
            int                 f1, f2, f3, f4, f5, f6, f7, f8, f9;
            Parameter           *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;
};

class EQ3 : public Device36
{
public:
            EQ3();

            void                handleParamUpdate(Parameter* param = NULL);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:

            rosic::Equalizer    dspCoreEqualizer;
            Parameter           *freq1, *freq2, *freq3;
            int                 f1, f2, f3;
            Parameter           *gain1, *gain2, *gain3, *bandwidth;
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
            Parameter           *highCut, *lowCut;
            Parameter           *roomsize, *preDelay, *drywet, *decay;
            Parameter           *lowscale, *highscale;

            rosic::Reverb       dspCoreReverb;
};


