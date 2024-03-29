
#include "36_globals.h"
#include "36_audio_dev.h"
#include "36_midi.h"
#include "36_vst.h"
#include "36_effects.h"
#include "36_instr.h"
#include "36_sampleinstr.h"
#include "36_vst.h"
#include "36_instrpanel.h"
#include "36_renderer.h"
#include "36_params.h"
#include "36_mixchannel.h"
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




JuceAudioDeviceManager*     JAudioManager = NULL;
JuceAudioCallback*          JAudioCallBack = NULL;
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

    JAudioManager = new JuceAudioDeviceManager;
    JAudioManager->initialise(0, 2, xmlAudioSettings, true);

    JAudioCallBack = new JuceAudioCallback;

    JAudioManager->addAudioCallback(JAudioCallBack);
    JAudioManager->addMidiInputCallback("", JMidiCallBack);

    if(xmlAudioSettings == NULL)
    {
        JuceAudioDeviceManager::AudioDeviceSetup setup;

        JAudioManager->getAudioDeviceSetup(setup);

        setup.bufferSize = bufferSize;

        JAudioManager->setAudioDeviceSetup(setup, false);
    }

    globalMute = mixMute = false;

    prevPattern = new Pattern(NULL, 0.f, -1.f, 0, 119, true);
    prevPattern->setBasePattern(prevPattern);
    prevPattern->calcFrames();
    prevPattern->addInstance(prevPattern);

    previewEvent = new Event(0, prevPattern);
    dummyTrigger = new Trigger(NULL, prevPattern, NULL); // used to avoid event deletion, need better solution

    previewEvent->addTrigger(dummyTrigger);

    metroOn = false;

    // initMetronome();
}

Audio36::~Audio36()
{
    JAudioManager->removeAudioCallback(JAudioCallBack);

    delete JAudioManager;
    delete JAudioCallBack;

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
    JAudioManager->getAudioDeviceSetup(setup);

    setup.bufferSize = int(bufSize);
    JAudioManager->setAudioDeviceSetup(setup, true);
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
        /*
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
        */
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

    if(MInstrPanel != NULL)
    {
        MInstrPanel->cleanBuffers(totalFrames);
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


    // Pre-calc minimal processing frame-count

    for(Pattern* pl : players)
    {
        pl->getSmallestCountDown(&frames);
    }


    // Now fully process all triggers and instruments

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

    if(MInstrPanel != NULL)
    {
        MInstrPanel->mixAll(totalFrames);
    }

    bool mutemixing = false;

    if(mixMute && MInstrPanel != NULL)
    {
        MInstrPanel->resetAll();

        mutemixing = true;
    }

    //mixMaster(inputBuffer, outputBuffer, totalFrames);
    //memcpy(outputBuffer, MMixer->getMasterChannel()->outBuff, totalFrames*2);

    float      *out = (float*)outputBuffer;
    float      *in = MInstrPanel->getMasterChannel()->outBuff;

    for(long c = 0; c < totalFrames; c++)
    {
       *out++ = *in++;
       *out++ = *in++;
    }

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

    /*
start_cleanup:

    for(Instrument* i : MInstrPanel->instrs)
    {
        if(i->getDevice() && i->getDevice()->isPreviewOnly())
        {
            MInstrPanel->deleteInstrument(i);

            goto start_cleanup;
        }
    }*/
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

