

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
std::forward_list<Parameter*>   globalParams;




Transport::Transport(float bpm,int tpb,int bpb)
{
    beatsPerMinute = bpm;
    ticksPerBeat = tpb;
    beatsPerBar = bpb;

    playing = new Parameter("PLAYBACK", false);

    init = true;
    propagateChanges();
    init = false;
}

void Transport::adjustTime(int min,int sec,int ms)
{
    Pattern* mainPlayer = MGrid->getPattern();
    double  currTick = mainPlayer->getPlayTick();
    float   timeInSeconds = (currTick/(ticksPerBeat*beatsPerMinute))*60;

    timeInSeconds += float(min)*60 + sec + float(ms)/1000;

    if (timeInSeconds < 0)
    {
        timeInSeconds = 0;
    }

    double newTick = (timeInSeconds/60)*(ticksPerBeat*beatsPerMinute);

    mainPlayer->setPlayTick(newTick);

    updTimeFromFrame();

    MCtrllPanel->getTimeScreen().redraw();

    MEdit->playHead->updatePosFromFrame();
}

float Transport::getInvertedFPT()
{
    return invertedFPT;
}

int Transport::getTicksPerBar()
{
    return ticksPerBeat*beatsPerBar;
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

void Transport::goToHome()
{
    MPattern->setFrame(0);

    MGrid->hscr->setOffset(0);

    MEdit->playHead->updatePosFromFrame();

    updTimeFromFrame();
}

void Transport::goToEnd()
{
    long lastframe = MPattern->getLastElementFrame();

    MPattern->setFrame(lastframe);

    //float tick = MainClock->getTickFromFrame(lastframe);
    //GridPlayer->setTick(tick);

    if(GPlaying)
    {
        togglePlayback(); // Toggle play to stop

        GetButton(MCtrllPanel, "bt.play")->release();
    }

    MEdit->playHead->updatePosFromFrame();

    updTimeFromFrame();

    MGrid->vscr->gotoEnd();

    MGrid->redraw(false, false);
}

float Transport::getSecondsPerTick()
{
    return secondsPerTick;
}

float Transport::getFramesPerTick()
{
    return framesPerTick;
}

long Transport::getFrameFromTick(float tick)
{
    return long(tick*framesPerTick);
}

float Transport::getTickFromFrame(long frame)
{
    return float(frame*invertedFPT);
}

bool Transport::isPlaying()
{ 
    return playing->getBoolValue(); 
}

void Transport::propagateChanges()
{
    float sr = MAudio != NULL ? MAudio->getSampleRate() : 44100;

    secondsPerTick = 1.0f / (beatsPerMinute / 60.f * ticksPerBeat);

    framesPerTick = secondsPerTick * sr;

    invertedFPT = 1 / (secondsPerTick * sr);

    if(MGrid)
    {
        MGrid->updTransport();
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

void Transport::reset()
{
    beatsPerMinute = 120;
    ticksPerBeat = 4;
    beatsPerBar = 4;

    propagateChanges();
}

void Transport::stopParamsRecording()
{
    // Resets parameters being recorded to envelopes

    if(GRecOn == true)
    {
        while(recParams.size() > 0)
        {
            recParams.front()->finishRecording();
        }
    }
}

void Transport::togglePlayback()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    if(GPlaying)
    {
        GPlaying = false;

        //GridPlayer->setFrame((long)GridPlayer->getFrameTSync());

        MPattern->deactivate();

        if(GRecOn == true)
        {
            stopParamsRecording();
        }

        MAudio->resetProcessing(false);
    }
    else
    {
        GPlaying = true; // start playing

        //if(paraminit)
        {
            for(Parameter* param : globalParams)
            {
                /*
                param->lastsetframe = 0;
        
                if(param->envtweaked)
                {
                    param->reset();
                }*/
            }
        }

        MPattern->activate();
    }

    ReleaseMutex(AudioMutex);
}

void Transport::stopPlayback(bool force_mixreset)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    GPlaying = false;

    MAudio->resetProcessing(force_mixreset);

    MEdit->playHead->reset();

    MPattern->deactivate();

    MPattern->setFrame(0);

    MGrid->hscr->setOffset(0);

    updTimeFromFrame();

    if(GRecOn)
    {
        stopRecording();
    }

    ReleaseMutex(AudioMutex);
}

void Transport::stopRecording()
{
    // Completely stops recording

    stopParamsRecording();

    GRecOn = false;

    GetButton(MCtrllPanel, "bt.rec")->release();
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


