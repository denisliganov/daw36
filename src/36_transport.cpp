

#include "36_transport.h"
#include "36.h"
#include "36_audio_dev.h"
#include "36_events_triggers.h"
#include "36_button.h"
#include "36_playhead.h"
#include "36_grid.h"
#include "36_pattern.h"
#include "36_ctrlpanel.h"
#include "36_instrpanel.h"
#include "36_instr.h"
#include "36_edit.h"
#include "36_numbox.h"
#include "36_vst.h"
#include "36_params.h"




std::list<Parameter*>           recParams;
std::forward_list<Parameter*>   params;


// Resets parameters being recorded to envelopes
//
void StopParamsRecording()
{
    if(GRecOn == true)
    {
        while(recParams.size() > 0)
        {
            recParams.front()->finishRecording();
        }
    }
}

// Completely stops recording
//
void StopRecording()
{
    StopParamsRecording();

    GRecOn = false;

    GetButton(MCtrllPanel, "bt.rec")->release();
}

void TogglePlayback()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    if(GPlaying)
    {
        GPlaying = false;

        //GridPlayer->setFrame((long)GridPlayer->getFrameTSync());

        MPattern->deactivate();

        if(GRecOn == true)
        {
            StopParamsRecording();
        }

        MAudio->resetProcessing(false);
    }
    else
    {
        GPlaying = true; // start playing

        MPattern->activate();
    }

    ReleaseMutex(AudioMutex);
}

void StopPlayback(bool force_mixreset)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    GPlaying = false;

    MAudio->resetProcessing(force_mixreset);

    MEdit->playHead->reset();

    MPattern->deactivate();

    MPattern->setFrame(0);

    MGrid->sethoffs(0);

    MTransp->updTimeFromFrame();

    if(GRecOn)
    {
        StopRecording();
    }

    ReleaseMutex(AudioMutex);
}

void GoToHome()
{
    MPattern->setFrame(0);

    MGrid->sethoffs(0);

    MEdit->playHead->updatePosFromFrame();

    MTransp->updTimeFromFrame();
}

void GoToEnd()
{
    long lastframe = MPattern->getLastElementFrame();

    MPattern->setFrame(lastframe);

    //float tick = MainClock->getTickFromFrame(lastframe);
    //GridPlayer->setTick(tick);

    if(GPlaying)
    {
        TogglePlayback(); // Toggle play to stop

        GetButton(MCtrllPanel, "bt.play")->release();
    }

    MEdit->playHead->updatePosFromFrame();

    MTransp->updTimeFromFrame();

    MCtrllPanel->getScroller().goToEnd();

    MGrid->redraw(false, false);
}

void UpdateTime()
{
    MEdit->playHead->updatePosFromFrame();

    MTransp->updTimeFromFrame();
}

void Players_UpdateQueuedEvs()
{
    WaitForSingleObject(AudioMutex, INFINITE);
    
    if(GPlaying)
    {
        MPattern->queueEvent();
    }

    ReleaseMutex(AudioMutex);
}


Transport::Transport(float bpm,int tpb,int bpb)
{
    beatsPerMinute = bpm;
    ticksPerBeat = tpb;
    beatsPerBar = bpb;

    init = true;
    propagateChanges();
    init = false;
}

void Transport::reset()
{
    beatsPerMinute = 120;
    ticksPerBeat = 4;
    beatsPerBar = 4;

    propagateChanges();
}

float Transport::getInvertedFPT()
{
    return invertedFPT;
}

int Transport::getTicksPerBar()
{
    return ticksPerBeat*beatsPerBar;
}

void Transport::setBeatsPerMinute(float bpm)
{
    beatsPerMinute = bpm;

    propagateChanges();
}

void Transport::setTicksPerBeat(int tpb)
{
    ticksPerBeat = tpb;

    propagateChanges();
}

void Transport::setBeatsPerBar(int bpb)
{
    beatsPerBar = bpb;

    propagateChanges();
}

float Transport::getBeatsPerMinute()
{
    return beatsPerMinute;
}

int Transport::getTicksPerBeat()
{
    return ticksPerBeat;
}

int Transport::getBeatsPerBar()
{
    return beatsPerBar;
}

float Transport::getSecondsPerTick()
{
    return secondsPerTick;
}

float Transport::getFramesPerTick()
{
    return framesPerTick;
}

void Transport::propagateChanges()
{
    float sr = MAudio != NULL ? MAudio->getSampleRate() : 44100;

    secondsPerTick = 1.0f / (beatsPerMinute / 60.f * ticksPerBeat);

    framesPerTick = secondsPerTick * sr;

    invertedFPT = 1 / (secondsPerTick * sr);

    if(MGrid)
    {
        MGrid->handleTransportUpdate();
    }

    if(MEdit)
    {
        MEdit->playHead->updateFrameFromPos();
    }

    updTimeFromFrame();

    if (VstHost != NULL)
    {
        VstHost->setBPM(beatsPerMinute);
    }
}

void Transport::adjustTime(int min,int sec,int ms)
{
    Pattern* mainPlayer = MGrid->getPattern();
    double  currTick = mainPlayer->getPlayTick();
    float   timeInSeconds = (currTick/(ticksPerBeat*beatsPerMinute))*60;

    timeInSeconds += float(min)*60 + sec + float(ms)/1000;

    if (timeInSeconds < 0)
        timeInSeconds = 0;

    double newTick = (timeInSeconds/60)*(ticksPerBeat*beatsPerMinute);

    mainPlayer->setPlayTick(newTick);

    updTimeFromFrame();

    MCtrllPanel->getTimeScreen().redraw();

    MEdit->playHead->updatePosFromFrame();
}

void Transport::updTimeFromFrame()
{
    float sr = MAudio != NULL ? MAudio->getSampleRate() : 44100;

    if (MGrid)
    {
        Pattern* mainPlayer = MGrid->getPattern();

        float   fsec = (float)mainPlayer->getFrame() / sr;

        currMin = int(fsec / 60);
        currMs = int(fsec * 1000) % 1000;
        currSec = int(fsec) % 60;

        double  currTick = mainPlayer->getPlayTick();

        currBar = int(currTick / (ticksPerBeat * beatsPerBar));
        currBeat = int(currTick / ticksPerBeat) % beatsPerBar;

        currTick = float(currTick - int(currTick) / ticksPerBeat * ticksPerBeat);

        if (!init)
        {
            MCtrllPanel->getTimeScreen().redraw();
        }
    }
}

long Transport::getFrameFromTick(float tick)
{
    return long(tick*framesPerTick);
}

float Transport::getTickFromFrame(long frame)
{
    return float(frame*invertedFPT);
}



