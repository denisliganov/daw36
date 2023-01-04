


#pragma once

#include <list>
#include <forward_list>

#include "36_globals.h"




extern std::list<Parameter*>           recParams;
extern std::forward_list<Parameter*>   params;



void        StopParamsRecording();
void        StopRecording();
void        TogglePlayback();
void        StopPlayback(bool forceresetmix = false);
void        UpdateTime();
void        GoToHome();
void        GoToEnd();

void        SyncPos(Pattern* playerToSyncTo);
void        Players_UpdateQueuedEvs();


class Transport
{
public:

        Transport(float beatsPerMinute, int ticksPerBeat, int beatsPerBar);

        void    reset();
        void    setBeatsPerMinute(float bpm);
        float   getBeatsPerMinute();
        void    setTicksPerBeat(int tpb);
        int     getTicksPerBeat();
        void    setBeatsPerBar(int bpb);
        int     getBeatsPerBar();
        float   getFramesPerTick();
        float   getInvertedFPT();              // 1 / frames per tick
        int     getTicksPerBar();
        float   getSecondsPerTick();
        float   getFramesPerPixel(float stepWidth);
        float   getTickFromFrame(long frame);
        long    getFrameFromTick(float tick);
        void    adjustTime(int min, int sec, int ms);
        void    propagateChanges();
        int     getCurrMin() {return currMin;};
        int     getCurrSec() {return currSec;};
        int     getCurrMs()  {return currMs;};
        void    updTimeFromFrame();

protected:

        int             currMin;
        int             currSec;
        int             currMs;

        bool            init;
        int             currBar;
        int             currBeat;
        float           currTick;
        float           beatsPerMinute;
        int             ticksPerBeat;
        int             beatsPerBar;
        float           framesPerPixel;
        float           framesPerTick;
        float           secondsPerTick;
        float           invertedFPT;
};


