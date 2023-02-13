
#include "36_globals.h"
#include "36_audio_dev.h"
#include "36_midi.h"
#include "36_vst.h"
#include "36_effects.h"
#include "36_instr.h"
#include "36_sampleinstr.h"
#include "36_vstinstr.h"
#include "36_instrpanel.h"
#include "36_renderer.h"
#include "36_params.h"
#include "36_mixer.h"
#include "36_vu.h"
#include "36_utils.h"
#include "36.h"
#include "36_transport.h"
#include "36_audiomanager.h"
#include "36_pattern.h"
#include "36_edit.h"
#include "36_playhead.h"
#include "36_button.h"
#include "36_element.h"
#include "36_note.h"
#include "36_events_triggers.h"


#include "Binarysrc/metronome_waves.h"

#include <mutex>

//namespace M {

JuceAudioDeviceManager*     JAudManager = NULL;
JuceAudioCallback*          JAudCallBack = NULL;
JuceMidiInputCallback*      JMidiCallBack = NULL;

HANDLE                      AudioMutex;
HANDLE                      MixerMutex;

std::mutex                  MixMutex;



Audio36::Audio36(float smpRate)
{
    sampleRate = smpRate;

    AudioMutex = CreateMutex(NULL, FALSE, NULL);
    MixerMutex = CreateMutex(NULL, FALSE, NULL);

    bufferSize = DEFAULT_BUFFER_SIZE;

    Config_DefaultInterpolation = Interpol_linear;
    Config_RenderInterpolation = Interpol_6dHermit;

    Init_MIDI();
    Init_Renderer();

    // Configure and start audio device

    JAudManager = new JuceAudioDeviceManager;
    JAudManager->initialise(0, 2, xmlAudioSettings, true);

    JAudCallBack = new JuceAudioCallback;

    JAudManager->addAudioCallback(JAudCallBack);
    JAudManager->addMidiInputCallback("", JMidiCallBack);

    if(xmlAudioSettings == NULL)
    {
        JuceAudioDeviceManager::AudioDeviceSetup setup;

        JAudManager->getAudioDeviceSetup(setup);

        setup.bufferSize = bufferSize;

        JAudManager->setAudioDeviceSetup(setup, false);
    }

    MasterVol = new Parameter("Master volume", Param_Vol, 0.f, DAW_VOL_RANGE, 1.f);
    MasterVol->setEnvDirect(false);

    globalMute = mixMute = false;

    prevPattern = new Pattern(NULL, 0.f, -1.f, 0, 119, true);
    prevPattern->setBasePattern(prevPattern);
    prevPattern->calcframes();
    prevPattern->addInstance(prevPattern);

    previewEvent = new Event(0, prevPattern);
    dummyTrigger = new Trigger(NULL, prevPattern, NULL); // used to avoid event deletion, need better solution

    previewEvent->addTrigger(dummyTrigger);

    metroOn = false;

    // initMetronome();
}

Audio36::~Audio36()
{
    JAudManager->removeAudioCallback(JAudCallBack);

    delete JAudManager;
    delete JAudCallBack;

    ReleaseMutex(AudioMutex);
    ReleaseMutex(MixerMutex);
}

void Audio36::setSampleRate(float smpRate)
{
    sampleRate = smpRate;

    VstHost->setSampleRate((float)sampleRate);
    MInstrPanel->setSampleRate(sampleRate);
}

float Audio36::getSampleRate()
{
    return sampleRate;
}

float Audio36::getInvertedSampleRate()
{
    return 1.f/sampleRate;
}

void Audio36::setBufferSize(unsigned bufSize)
{
    bufferSize = (int)bufSize;

    VstHost->setBufferSize(bufSize);
    MInstrPanel->setBufferSize(bufSize);

    //audioDeviceManager->stopDevice();

    JuceAudioDeviceManager::AudioDeviceSetup setup;
    JAudManager->getAudioDeviceSetup(setup);

    setup.bufferSize = int(bufSize);
    JAudManager->setAudioDeviceSetup(setup, true);
}

int Audio36::getBufferSize()
{
    return bufferSize;
}

void Audio36::handleMetronome(long framesPerBuffer, bool pre)
{
    static long baroffs = 0;
    static long beatoffs = 0;

    if(metroOn && GPlaying)
    {
        if(pre)
        {
            long frame = MPattern->getFrame();
            long fpbeat = long(MTransp->getFramesPerTick()*MTransp->getTicksPerBeat());
            long fpbar = fpbeat*MTransp->getBeatsPerBar();

            if(frame == 0 ||(frame/fpbar != (frame + framesPerBuffer)/fpbar))
            {
                baroffs = (frame + framesPerBuffer)/fpbar*fpbar - frame;

                barSample->activateTrigger(barSample->selfNote->getTriggers().front());
            }
            else if(frame/fpbeat != (frame + framesPerBuffer)/fpbeat)
            {
                beatoffs = (frame + framesPerBuffer)/fpbeat*fpbeat - frame;

                beatSample->activateTrigger(beatSample->selfNote->getTriggers().front());
            }
        }
        else
        {
            barSample->generateData(framesPerBuffer - baroffs, baroffs);
            beatSample->generateData(framesPerBuffer - beatoffs, beatoffs);
        }
    }
}

void Audio36::mixMaster(const void* inputBuffer, void* outputBuffer, long totalFrames)
{
    // Cast data passed through stream to our structure.

    float          *out = (float*)outputBuffer;
    const float    *in = (float*)inputBuffer;

    Envelope* mvenv = NULL;
    Trigger* tgenv = MasterVol->envelopes;

    if(tgenv != NULL && MasterVol->envaffect)
    {
        mvenv = ((Envelope*)tgenv->el);
    }

    float vol = MInstrPanel->masterVolume->getOutVal(); //MasterVol->outVal;

    if(MasterVol->lastValue == -1)
    {
        MasterVol->setLastVal(MasterVol->getOutVal());
    }
    else if(MasterVol->lastValue != MasterVol->getOutVal())
    {
        if(MasterVol->declickCount == 0)
        {
            MasterVol->declickCount = DECLICK_COUNT;
            MasterVol->declickCoeff = float(MasterVol->getOutVal() - MasterVol->lastValue) / DECLICK_COUNT;

            vol = MasterVol->lastValue;
        }
        else
        {
            vol = MasterVol->lastValue + (DECLICK_COUNT - MasterVol->declickCount)*MasterVol->declickCoeff;
        }
    }
    else if(MasterVol->declickCount > 0) // (params->vol->lastval == params->vol->outval)
    {
        MasterVol->declickCount = 0;
        MasterVol->declickCoeff = 0;
    }

    float outL, outR, lMax, rMax;
    lMax = rMax = 0;
    long bc = 0;

    if(in == NULL)
    {
        for(long offset = 0; offset < totalFrames; offset++)
        {
            if(mvenv != NULL && offset >= mvenv->last_buffframe && MasterVol->declickCount == 0)
            {
                vol = mvenv->buffoutval[offset];

                if(offset == mvenv->last_buffframe_end - 1)
                {
                    MasterVol->setValueFromEnvelope(mvenv->buff[offset], mvenv);
                    MasterVol->setLastVal(MasterVol->getOutVal());
                }
            }

            if(MasterVol->declickCount > 0)
            {
                vol += MasterVol->declickCoeff;
                MasterVol->declickCount--;

                if(MasterVol->declickCount == 0)  MasterVol->setLastVal(MasterVol->getOutVal());
            }

            outL = MMixer->masterChannel->inbuff[bc++]*vol;
            outR = MMixer->masterChannel->inbuff[bc++]*vol;

            if(outL > lMax)  lMax = outL;
            if(outR > rMax)  rMax = outR;

           *out++ = outL;
           *out++ = outR;
        }

        //_MControlPanel->getTimeScreen().vu->setValues(lMax, rMax);

        //if(_MMixer->masterchan->chanvu != NULL && _MMixer->masterchan->chanvu->isShown())
        //    _MMixer->masterchan->chanvu->setValues(lMax, rMax);
    }
}

//
// This callback does three things:
//
// 1. Tick
// 2. Generate
// 3. Mix
//
void Audio36::generalCallBack(const void* inputBuffer, void* outputBuffer, long totalFrames)
{
    if(!InitComplete)
    {
        return;
    }

    WaitForSingleObject(AudioMutex, INFINITE);

    // Cleanup mixer buffers here

    if(MMixer != NULL)
    {
        MMixer->cleanBuffers(totalFrames);
    }

    // Pre-process metronome

    handleMetronome(totalFrames, true);

    processPreviews();

    if(GPlaying)
    {
        MEdit->playHead->restartTimer();

        MTransp->updTimeFromFrame();
    }

    long offset = 0;
    long frames = totalFrames;
    long nextFrames;


    // Pre-calc initial framecount for processing

    for(Pattern* pl : players)
    {
        pl->getSmallestCountDown(&frames);
    }


    // Now fully process triggers and instruments

    while(offset < totalFrames)
    {
        nextFrames = totalFrames - offset - frames;

        for(Pattern* pl : players)
        {
            pl->tickFrame(offset, frames, totalFrames);

            pl->getSmallestCountDown(&nextFrames);
        }

        MInstrPanel->generateAll(frames, offset);

        offset += frames;

        frames = nextFrames;
    }

    // Post-process metronome

    handleMetronome(totalFrames, false);

    // Cleanup active triggers

    for(auto itr = activeTriggers.begin(); itr != activeTriggers.end(); itr++)
    {
        Trigger* tg = *itr;

        if(tg->checkRemoval())
        {
            if(tg->el->isNote() && (tg->tgState == TS_Sustain || tg->tgState == TS_Release))
            {
                tg->setState(TS_SoftFinish);
            }
            else
            {
                tg->stop();

                // restart iterator

                itr = activeTriggers.begin();
            }
        }
    }

    int fadenum = totalFrames >= 100 ? 100 : totalFrames;

    if(globalMute)
    {
        // Force active triggers cleanup, when need to fade out

        for(auto itr = activeTriggers.begin(); itr != activeTriggers.end(); itr++)
        {
            Trigger* tg = *itr;

            if(mixMute)
            {
                tg->stop();

                itr = activeTriggers.begin();

                if (itr == activeTriggers.end())
                {
                    break;
                }
            }
            else
            {
                if(tg->el->isNote())
                {
                    tg->setState(TS_SoftFinish);
                }
                else
                {
                    tg->stop();

                    // restart iterator

                    itr = activeTriggers.begin();

                    if (itr == activeTriggers.end())
                    {
                        break;
                    }
                }
            }
        }

        // Force deactivating active plays as well, if total muting

        if(mixMute)
        {
            while(players.size() > 0)
            {
                players.front()->deactivate();
            }
        }

        globalMute = false;
    }

    if(MMixer != NULL)
    {
        MMixer->mixAll(totalFrames);
    }

    bool mutemixing = false;

    if(mixMute && MMixer != NULL)
    {
        MMixer->resetAll();

        mutemixing = true;
    }

    mixMaster(inputBuffer, outputBuffer, totalFrames);

    if(mutemixing)
    {
        mixMute = false;
    }

    ReleaseMutex(AudioMutex);
}

void Audio36::resetProcessing(bool resetmix)
{
    globalMute = true;

    if(resetmix)
    {
        mixMute = true;

        MInstrPanel->resetAll();
    }
}

bool Audio36::isNotePlayingOnPreview(int note_val)
{
    for(Trigger* tgp : previewEvent->triggers)
    {
        if(tgp != dummyTrigger && tgp->noteVal == note_val && tgp->tgState != TS_Release && tgp->tgState != TS_SoftFinish && tgp->tgState != TS_Finished)
        {
            return true;
        }
    }

    return false;
}

void Audio36::addPreviewTrigger(Trigger* tg)
{
    previewEvent->addTrigger(tg);
}

void Audio36::releasePreviewByElement(Element* el)
{
    for(Trigger* tgp : previewEvent->triggers)
    {
        if(tgp != dummyTrigger && tgp->el == el)
        {
            if(tgp->tgState != TS_SoftFinish && tgp->tgState != TS_Finished)
            {
                tgp->setState(TS_Release);
            }
        }
    }
}

void Audio36::releasePreviewByNote(int note_val)
{
    for(Trigger* tgp : previewEvent->triggers)
    {
        if(tgp != dummyTrigger && tgp->noteVal == note_val)
        {
            if(tgp->tgState != TS_SoftFinish && tgp->tgState != TS_Finished)
            {
                tgp->setState(TS_Release);
            }
        }
    }
}

void Audio36::releaseAllPreviews()
{
    for(Trigger* tgp : previewEvent->triggers)
    {
        if(tgp != dummyTrigger)
        {
            if(tgp->tgState != TS_SoftFinish && tgp->tgState != TS_Finished)
            {
                tgp->setState(TS_Release);
            }
        }
    }

start_cleanup:

    for(Instrument* i : MInstrPanel->instrs)
    {
        if(i->isPreviewOnly())
        {
            MInstrPanel->deleteInstrument(i);

            goto start_cleanup;
        }
    }
}

void Audio36::processPreviews()
{
    for (auto itr = previewEvent->triggers.cbegin(); itr != previewEvent->triggers.cend(); itr++)
    {
        Trigger* tgp = *itr;

        if(tgp != dummyTrigger)
        {
            if(tgp->tgState == TS_Initial)
            {
                tgp->start(0);
            }
            else if(tgp->tgState == TS_Finished)
            {
                tgp->removeFromEvent();

                tgp->el->removeTrigger(tgp);

                delete tgp;

                itr = previewEvent->triggers.cbegin();
            }
        }
    }
}

void Audio36::initMetronome()
{
    SF_INFO         sf_info;

    memset(&sf_info, 0, sizeof(SF_INFO));

/*
    sf_info.frames = 3682;
    sf_info.samplerate = 44100;
    sf_info.channels = 1;
    sf_info.format = SF_FORMAT_WAV;
*/

    sf_info.frames = 2316;
    sf_info.samplerate = 44100;
    sf_info.channels = 1;
    sf_info.format = SF_FORMAT_WAV;


    barSample = new Sample((float*)MetroWavs::beatwav, NULL, sf_info);

    sf_info.frames = 2316;
    sf_info.samplerate = 44100;
    sf_info.channels = 1;
    sf_info.format = SF_FORMAT_WAV;

    beatSample = new Sample((float*)MetroWavs::beatwav, NULL, sf_info);
}

//}

