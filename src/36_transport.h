


#pragma once

#include <list>
#include <forward_list>

#include "36_globals.h"
#include "36_params.h"



extern std::list<Parameter*>           recParams;
extern std::forward_list<Parameter*>   params;





class Transport
{
public:

            Transport(float beatsPerMinute, int ticksPerBeat, int beatsPerBar);

            void                adjustTime(int min, int sec, int ms);
            void                setBeatsPerMinute(float bpm);
            float               getBeatsPerMinute();
            int                 getCurrMin() {return currMin;};
            int                 getCurrSec() {return currSec;};
            int                 getCurrMs()  {return currMs;};
            int                 getTicksPerBeat();
            int                 getBeatsPerBar();
            float               getFramesPerTick();
            float               getInvertedFPT();              // 1 / frames per tick
            int                 getTicksPerBar();
            float               getSecondsPerTick();
            float               getFramesPerPixel(float stepWidth);
            float               getTickFromFrame(long frame);
            long                getFrameFromTick(float tick);
            void                goToHome();
            void                goToEnd();
            bool                isPlaying()     { return playing->getValue(); }
            void                propagateChanges();
            void                reset();
            void                setTicksPerBeat(int tpb);
            void                setBeatsPerBar(int bpb);
            void                stopParamsRecording();
            void                stopPlayback(bool forceresetmix = false);
            void                stopRecording();
            void                togglePlayback();
            void                updTimeFromFrame();

protected:

            ParamToggle*        playing;

            int                 currMin;
            int                 currSec;
            int                 currMs;

            bool                init;
            int                 currBar;
            int                 currBeat;
            float               currTick;
            float               beatsPerMinute;
            int                 ticksPerBeat;
            int                 beatsPerBar;
            float               framesPerPixel;
            float               framesPerTick;
            float               secondsPerTick;
            float               invertedFPT;
};


