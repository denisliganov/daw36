
#pragma once

#include "sndfile.h"

#include "36_device.h"


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



class Sample : public Device36
{
friend  InstrPanel;
friend  SampleNote;
friend  Audio36;

public:

            Sample(float* data, char* pth, SF_INFO sfinfo);
            virtual ~Sample();
            virtual bool        checkBounds(SampleNote* samplent, Trigger* tg, long num_frames);
            void                copyDataToClonedInstrument(Instrument * instr);
            SubWindow*          createWindow();
            float               calcSampleFreqIncrement(int semitones);
            void                dumpData();
            void                load(XmlElement* instrNode);
            void                save(XmlElement* instrNode);
            void                setLoopEnd(long end);
            void                setLoopStart(long start);
            void                setLoopPoints(long start, long end);
            void                toggleNormalize();
            void                updateNormalizeFactor();

            bool                normalized;
            float               timelen;
            long                lp_start;
            long                lp_end;
            LoopType            looptype;
            SF_INFO             sample_info;
            float*              sampleData;

protected:

            long                numFrames;
            int                 numChannels;
            Image*              waveImage;
            float               rateUp;
            float               rateDown;
            float               normFactor;

            void                activateTrigger(Trigger * tg);
            int                 calcPixLength(long num_frames, long sample_rate, float tickwidth);
            inline void         getMonoData(double cursor_pos, float* dataLR);
            inline void         getStereoData(double cursor_pos, float* dataL, float* dataR);
            inline float        gaussInterpolation(float* Yi, float dX);
            long                handleTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
            inline double       sinc(double a);
            inline double       sincWindowedBlackman(double a, double b, unsigned int num);
            inline float        sincInterpolate(float* Yi, double dX, unsigned int num);
            void                updWaveImage();
};



