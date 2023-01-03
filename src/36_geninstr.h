
#pragma once


#include "36_instr.h"



class Osc
{
public:
    float   phase_shift;
    float   finetune;
    int     note_shift;

    Osc();
    virtual void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2) {}
};

class Saw : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Sine : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Triangle : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Generator : public Instrument
{
public:

    Generator();
    virtual ~Generator();
    virtual void CheckBounds(Note* gnote, Trigger* tg, long num_frames);
};


/*
class SoundFont : public Generator
{
public:

    sfBankID    bank_id;
    char        sf_file_path[MAX_PATH_STRING];
    char        sf_name[MAX_NAME_STRING];

    SoundFont(sfBankID bankid);
};
*/

/*

class Synth : public Generator
{
    typedef struct sVoice
    {
        double      sec_phase1;
        double      sec_phase2;
        double      sec_phase3;
        float       envVal1;
        float       envVal2;
        float       envVal3;
        float       envVal4;
        float       envVal5;
        EnvPoint*     ep1;
        EnvPoint*     ep2;
        EnvPoint*     ep3;
        EnvPoint*     ep4;
        EnvPoint*     ep5;
        double      env_phase1;
        double      env_phase2;
        double      env_phase3;
        double      env_phase4;
        double      env_phase5;
    };

public:
    int         numUsedVoices;
    int         maxVoices;
    int         freeVoice;

    Parameter*  osc1Level;
    Parameter*  osc1Octave;
    Parameter*  osc1Detune;
    Parameter*  osc1Width;
    BoolParam*  osc1Sine;
    BoolParam*  osc1Saw;
    BoolParam*  osc1Tri;
    BoolParam*  osc1Pulse;
    BoolParam*  osc1Noise;
    float       osc1fmulStatic;
    float       osc1fmulLFO;
    float       osc1fmulFM;

    Parameter*  osc2Level;
    Parameter*  osc2Octave;
    Parameter*  osc2Detune;
    Parameter*  osc2Width;
    BoolParam*  osc2Sine;
    BoolParam*  osc2Saw;
    BoolParam*  osc2Tri;
    BoolParam*  osc2Pulse;
    BoolParam*  osc2Noise;
    float       osc2fmulStatic;
    float       osc2fmulLFO;
    float       osc2fmulFM;

    Parameter*  osc3Level;
    Parameter*  osc3Octave;
    Parameter*  osc3Detune;
    Parameter*  osc3Width;
    BoolParam*  osc3Sine;
    BoolParam*  osc3Saw;
    BoolParam*  osc3Tri;
    BoolParam*  osc3Pulse;
    BoolParam*  osc3Noise;
    float       osc3fmulStatic;
    float       osc3fmulLFO;
    float       osc3fmulFM;

    BoolParam*  osc1Fat;
    BoolParam*  osc2Fat;
    BoolParam*  osc3Fat;

    Parameter*  noiseLevel;

    Parameter*  osc1FM2Level;
    Parameter*  osc2FM3Level;
    Parameter*  osc1FM3Level;

    Parameter*  osc1RM2Level;
    Parameter*  osc2RM3Level;
    Parameter*  osc1RM3Level;

    Parameter*  LFOAmp_Vol1;
    Parameter*  LFOAmp_Vol2;
    Parameter*  LFOAmp_Vol3;
    Parameter*  LFOAmp_Pitch1;
    Parameter*  LFOAmp_Pitch2;
    Parameter*  LFOAmp_Pitch3;
    Parameter*  LFOAmp_Flt1Freq;
    Parameter*  LFOAmp_Flt2Freq;

    Parameter*  LFORate_Vol1;
    Parameter*  LFORate_Vol2;
    Parameter*  LFORate_Vol3;
    Parameter*  LFORate_Pitch1;
    Parameter*  LFORate_Pitch2;
    Parameter*  LFORate_Pitch3;
    Parameter*  LFORate_Flt1Freq;
    Parameter*  LFORate_Flt2Freq;

    CFilter3*   filt1[12];
    BoolParam*  filt1Osc1;
    BoolParam*  filt1Osc2;
    BoolParam*  filt1Osc3;

    CFilter3*   filt2[12];
    BoolParam*  filt2Osc1;
    BoolParam*  filt2Osc2;
    BoolParam*  filt2Osc3;

    CChorus*    chorus;
    XDelay*     delay;
    CReverb*    reverb;

    BoolParam*  chorusON;
    BoolParam*  delayON;
    BoolParam*  reverbON;

    sVoice      voice[12];
    float       data_buff2[MAX_BUFF_SIZE*2];
    float       data_buff3[MAX_BUFF_SIZE*2];

    SynthWindow*    synthWin;
    SynthComponent* SynthComp;

    float       fltbuff1[MAX_BUFF_SIZE*2];
    float       fltbuff2[MAX_BUFF_SIZE*2];
    float       fltbuff1_out[MAX_BUFF_SIZE*2];
    float       fltbuff2_out[MAX_BUFF_SIZE*2];

    float       fltlfo1[MAX_BUFF_SIZE];
    float       fltlfo2[MAX_BUFF_SIZE];

    // Buffers where parsed envelope values are stored
    float       ebuff1[MAX_BUFF_SIZE];
    float       ebuff2[MAX_BUFF_SIZE];
    float       ebuff3[MAX_BUFF_SIZE];
    float       ebuff4[MAX_BUFF_SIZE];
    float       ebuff5[MAX_BUFF_SIZE];

    float       fp;
    float       rate;

    float*      p_osc1envVal;
    float*      p_osc2envVal;
    float*      p_osc3envVal;
    float*      p_flt1envVal;
    float*      p_flt2envVal;

    Envelope*   env1;
    Envelope*   env2;
    Envelope*   env3;
    Envelope*   env4;
    Envelope*   env5;

    float       envV1;
    float       envV2;
    float       envV3;
    float       envV4;
    float       envV5;

    float       lenvV1;
    float       lenvV2;
    float       lenvV3;
    float       lenvV4;
    float       lenvV5;

    Trigger*    tgramp;

    Random*     rnd;

    Synth();
    virtual ~Synth();
    void ActivateTrigger(Trigger * tg);
    void DeactivateTrigger(Trigger* tg);
    long ProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
    void FillMixChannel(long num_frames, long buffframe, long mixbuffframe);
    void ApplyDSP(Trigger* tg, long buffframe, long num_frames);
    void UpdateOsc1Mul();
    void UpdateOsc2Mul();
    void UpdateOsc3Mul();
    void ParamUpdate(Parameter* param);
    void CheckBounds(GenNote* gnote, Trigger* tg, long num_frames);
    void SaveCustomStateData(XmlElement & xmlParentNode);
    void RestoreCustomStateData(XmlElement & xmlStateNode);
    int  MapEnvPointerToNum(float* eV);
    float* MapNumToEnvPointer(int num);
    float* MapNumToEBuffPointer(int num);
    void ResetValues();
    bool IsTriggerAtTheEnd(Trigger* tg);
    void Save(XmlElement* instrNode);
    void Load(XmlElement* instrNode);
    void CreateModuleWindow();
    Envelope* CreateEnvelope();
    void CopyDataToClonedInstrument(Instrument * instr);
};

*/