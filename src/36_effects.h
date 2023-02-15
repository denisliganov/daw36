
#pragma once

#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_device.h"


class Eff : public Device36
{
public:
            Eff();
            virtual ~Eff();

            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
            SubWindow*          createWindow();
            virtual Eff*        clone();
            void                drawSelf(Graphics& g);
            MixChannel*         getMixChannel() { return mixChannel; }
            int                 getMuteCount()  { return muteCount; }
            bool                getBypass()     { return bypass; }
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

            int                 muteCount;

private:

            bool                bypass;
            bool                folded;
            MixChannel*         mixChannel;
};

class Filter1 : public Eff
{
public:
            Filter1();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            ParamRadio*         filterType;
            ParamToggle*        x2toggle;
            Parameter*          cutoff;
            Parameter*          resonance;
            Parameter*          bandwidth;
            bool                f_master;
            Filter1*            f_next;

            rosic::LadderFilter dspCoreCFilter3;
};

class CChorus : public Eff
{
public:
            CChorus();
            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          drywet;
            rosic::Chorus*      dspCoreChorus;
            Parameter*          delay;
            Parameter*          freq;
};

class CFlanger : public Eff
{
public:
            CFlanger();
            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
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

class CPhaser : public Eff
{
public:
            CPhaser();
            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
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

class EQ1 : public Eff
{
public:
            EQ1();
            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter           *bandwidth;
            rosic::TwoPoleFilter dspCoreEq1;
            Parameter           *frequency;
            Parameter           *gain;
};

class GraphicEQ : public Eff
{
public:
            GraphicEQ();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            rosic::Equalizer    dspCoreEqualizer;
            int                 f1, f2, f3, f4, f5, f6, f7, f8, f9;
            Parameter           *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;
};

class EQ3 : public Eff
{
public:
            EQ3();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:

            rosic::Equalizer    dspCoreEqualizer;
            Parameter           *freq1, *freq2, *freq3;
            int                 f1, f2, f3;
            Parameter           *gain1, *gain2, *gain3, *bandwidth;
};

class XDelay : public Eff
{
public:
            XDelay();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:

            Parameter*          delay;
            Parameter*          amount;
            Parameter*          feedback;
            Parameter*          pan;
            Parameter*          lowcut;
            Parameter*          drywet;
            ParamToggle*        delayMode;

            ParamSelector*      selectTypes;

            rosic::PingPongEcho dspCorePingPongDelay;
};

class CTremolo : public Eff
{
public:
            CTremolo();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          depth;
            Parameter*          speed;

            rosic::Tremolo      dspCoreCTremolo;
};

class Compressor : public Eff
{
public:
            Compressor();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
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

class CWahWah : public Eff
{
public:
            CWahWah();

            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();
            void                handleParamUpdate(Param* param = NULL);

protected:
            Parameter*          modfreq;
            Parameter*          depth;

            rosic::WahWah       dspCoreWah;
};

class CDistort : public Eff
{
public:
            CDistort();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          drive;
            Parameter*          postgain;
            Parameter*          slope;

            rosic::WaveShaper   dspCoreDist;
};

class CBitCrusher : public Eff
{
public:
            CBitCrusher();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          decimation;
            Parameter*          quantization;

            rosic::BitCrusher   dspCoreBC;
};

class CStereo : public Eff
{
public:
            CStereo();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter*          offset;

            rosic::CombStereoizer   dspCoreStereo;
};


class CReverb : public Eff
{
public:
            CReverb();

            void                handleParamUpdate(Param* param = NULL);
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                reset();

protected:
            Parameter           *highCut, *lowCut;
            Parameter           *roomsize, *preDelay, *drywet, *decay;
            Parameter           *lowscale, *highscale;

            rosic::Reverb       dspCoreReverb;
};


