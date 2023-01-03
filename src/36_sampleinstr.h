
#pragma once


#include "36_instr.h"


typedef enum InterpolationType
{
    Interpol_linear = 1,
    Interpol_3dHermit,
    Interpol_6dHermit,
    Interpol_64Sinc,
    Interpol_128Sinc,
    Interpol_256Sinc
}InterpolationType;


typedef enum LoopType
{
    LoopType_NoLoop,
    LoopType_ForwardLoop,
    LoopType_PingPongLoop
}LoopType;



class Sample : public Instrument
{
friend  InstrPanel;
friend  SampleNote;
friend  Audio36;

protected:

        long            numFrames;
        int             numChannels;
        Image*          waveImage;
        float           rateUp;
        float           rateDown;
        float           normFactor;

        void            activateTrigger(Trigger * tg);
        long            processTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);

        inline float    gaussInterpolation(float* Yi, float dX);
        inline double   sinc(double a);
        inline double   sincWindowedBlackman(double a, double b, unsigned int num);
        inline float    sincInterpolate(float* Yi, double dX, unsigned int num);
        inline void     getMonoData(double cursor_pos, float* dataLR);
        inline void     getStereoData(double cursor_pos, float* dataL, float* dataR);
        void            updWaveImage();
        int             calcPixLength(long num_frames, long sample_rate, float tickwidth);

public:

        bool        normalized;
        float       timelen;
        long        lp_start;
        long        lp_end;
        LoopType    looptype;
        SF_INFO     sample_info;
        float*      sampleData;

        Sample(float* data, char* pth, SF_INFO sfinfo);
        virtual ~Sample();
        virtual bool    checkBounds(SampleNote* samplent, Trigger* tg, long num_frames);
        void        updateNormalizeFactor();
        void        toggleNormalize();
        void        setLoopEnd(long end);
        void        setLoopStart(long start);
        void        setLoopPoints(long start, long end);
        void        save(XmlElement* instrNode);
        void        load(XmlElement* instrNode);
        void        copyDataToClonedInstrument(Instrument * instr);
        void        dumpData();
        SubWindow*  createWindow();
        float       calcSampleFreqIncrement(int semitones);
};



