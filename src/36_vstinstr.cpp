
#include "36_vstinstr.h"
#include "36_vst.h"
#include "36_events_triggers.h"
#include "36_note.h"
#include "36_params.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_instrpanel.h"
#include "36_project.h"
#include "36_juce_components.h"
#include "36.h"
#include "36_browser.h"
#include "36_vstwin.h"
#include "36_paramswin.h"
#include "36_devwin.h"



//namespace M {



VstInstr::VstInstr(char* fullpath, VstInstr* vst)
{
    type = Instr_VstPlugin;

    internal = false;
    numevents = 0;
    muteCount = 0;

    if(fullpath != NULL)
    {
        vst2 = VstHost->loadModuleFromFile(fullpath);
    }
    else
    {
        vst2 = VstHost->loadModuleFromFile((char*)vst->filePath.data());
    }

    if (vst2 != NULL)
    {
        uniqueId = vst2->aeff->uniqueID;

        if (fullpath)
        {
            filePath = fullpath;
        }
        else
        {
            filePath = vst2->vstpath;
        }

        objName = ToUpperCase(vst2->objName);

        vst2->extractParams();

        presetPath = "Generators\\VST\\";

        vst2->updatePresets();

        presets = vst2->presets;         // sync lists 
    }

    createSelfPattern();
}

VstInstr::~VstInstr()
{
    if (vst2 != NULL)
    {
        VstHost->removeModule(vst2);
    }
}

void VstInstr::checkBounds(Note * gnote, Trigger * tg, long num_frames)
{
    if(tg->framePhase + num_frames >= gnote->framelen || 
                            tg->tgState == TS_Release || tg->tgState == TS_Finished ||tg->tgState == TS_SoftFinish)
    {
        // Check: Or better stop?

        tg->stop();
    }
}

long VstInstr::processTrigger(Trigger* tg, long num_frames, long buffframe)
{
    long loc_num_frames = num_frames;

    Note* gnote = (Note*)tg->el;

    float vol = 1; // tg->tgPatt->vol->val

    vol *= gnote->getVol()->getValue();

    if(!tg->previewTrigger)
    {
        if(gnote->framelen - tg->framePhase < loc_num_frames)
        {
            loc_num_frames = gnote->framelen - tg->framePhase;
        }

        addNoteEvent(tg->noteVal, loc_num_frames, tg->framePhase, gnote->framelen, vol);

        checkBounds(gnote, tg, loc_num_frames);
    }
    else
    {
        addNoteEvent(tg->noteVal, loc_num_frames, tg->framePhase, 1000000, vol);

        if(tg->tgState == TS_Release || 
           tg->tgState == TS_Finished ||
           tg->tgState == TS_SoftFinish)
        {
            // Check: Or better stop?

            tg->stop();
        }
    }

    tg->framePhase += num_frames;

    return num_frames;
}

void VstInstr::postProcessTrigger(Trigger* tg, long num_frames, long buffframe, long mixbuffframe, long remaining)
{
    float       panVal;
    float       volL, volR;
    long        tc, tc0;
    int         ai;

    panVal = pan->getOutVal();

    // PanConstantRule(pan, &volL, &volR);
    // Calculate constant rule directly here to improve performance

    ai = int((PI_F*(panVal + 1)/4)/wt_angletoindex);
    volL = wt_cosine[ai];
    volR = wt_sine[ai];

    Envelope* penv1 = NULL;

    if(pan->envelopes != NULL)
    {
        penv1 = (Envelope*)pan->env;
    }

    tc = mixbuffframe*2;
    tc0 = buffframe*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        if(penv1 != NULL)
        {
            panVal = penv1->buffoutval[mixbuffframe + cc];

            // update pan
            // PanConstantRule(pan, &volL, &volR);

            ai = int((PI_F*(panVal + 1)/4)/wt_angletoindex);

            volL = wt_cosine[ai];
            volR = wt_sine[ai];
        }
    
        outBuff[tc0] = dataBuff[tc0++]*volL;
        outBuff[tc0] = dataBuff[tc0++]*volR;
    }
}

void VstInstr::deactivateTrigger(Trigger* tg)
{
    Note* gnote = (Note*)tg->el;

    postNoteOFF(tg->noteVal, 127);

    Instrument::deactivateTrigger(tg);
}

void VstInstr::vstProcess(long num_frames, long buffframe)
{
    /*
    struct VstEvents			// a block of events for the current audio block
    {
        long numEvents;
        long reserved;			// zero
        VstEvent* events[2];	// variable
    };
    */

    VstEvents* events = NULL;

    if(numevents > 0)
    {
        unsigned short      NumEvents = (unsigned short)numevents;
        long                EvSize = sizeof(long)+ sizeof(long) + (sizeof(VstEvent*)*NumEvents);

        events = (VstEvents*)malloc(EvSize);
        memset(events, 0, EvSize);
        events->numEvents = NumEvents;
        events->reserved = 0;

        for (int i = 0; i < NumEvents; ++i)
        {
            events->events[i] = (VstEvent*) &(MidiEvents[i]);
        }

        vst2->aeffProcessEvents(events);
    }

    vst2->processData(NULL, &dataBuff[buffframe*2], num_frames);

    numevents = 0;

    // This freeing code was working here, but it causes Absynth 2.04 to crash, while other hosts work
    // well with it. Probably it's responsibility of a plugin to free the memory allocated for VstEvents
    // struct, although need to ensure later in this. Hahaha.
    //if(NULL != events)
    //{
    //    free(events);
    //}

}

void VstInstr::generateData(long num_frames, long mixbuffframe)
{
    bool off = false;

    if((muteparam != NULL && muteparam->getOutVal()) || (SoloInstr != NULL && SoloInstr != this))
    {
        off = true;
    }

    fill = true;

    for(auto itr = activeTriggers.begin(); itr != activeTriggers.end(); )
    {
        Trigger* tg = *itr;
        itr++;
        processTrigger(tg, num_frames);
    }

    memset(outBuff, 0, num_frames*sizeof(float)*2);
    memset(dataBuff, 0, num_frames*sizeof(float)*2);

    if(envelopes == NULL)
    {
        vstProcess(num_frames, 0);
    }
    else
    {
        Envelope* env;
        Parameter* param;
        Trigger* tgenv;
        long frames_to_process;
        long frames_remaining = num_frames;
        long buffframe = 0;

        while(frames_remaining > 0)
        {
            if(frames_remaining > BUFF_CHUNK_SIZE)
            {
                frames_to_process = BUFF_CHUNK_SIZE;
            }
            else
            {
                frames_to_process = frames_remaining;
            }

            tgenv = envelopes;

            while(tgenv != NULL)
            {
                env = (Envelope*)tgenv->el;

                param = ((Envelope*)tgenv->el)->param;
                param->setValueFromEnvelope(env->buff[mixbuffframe + buffframe], env);

                tgenv = tgenv->group_prev;
            }

            vstProcess(frames_to_process, buffframe);

            frames_remaining -= frames_to_process;

            buffframe += frames_to_process;
        }
    }

    if(off == false)
    {
        if(muteCount > 0)
        {
            long tc = 0;
            float aa;

            while(tc < num_frames)
            {
                if(muteCount > 0)
                {
                    aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                    muteCount--;
                }

                dataBuff[tc*2] *= aa;
                dataBuff[tc*2 + 1] *= aa;

                tc++;
            }
        }
    }
    else
    {
        if(muteCount < DECLICK_COUNT)
        {
            long tc = 0;
            float aa;

            while(tc < num_frames)
            {
                if(muteCount < DECLICK_COUNT)
                {
                    aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                    muteCount++;
                }

                dataBuff[tc*2] *= aa;
                dataBuff[tc*2 + 1] *= aa;

                tc++;
            }
        }
        else
        {
            fill = false;
        }
    }

    if(fill)
    {
        postProcessTrigger(NULL, num_frames, 0, mixbuffframe);

        fillMixChannel(num_frames, 0, mixbuffframe);
    }
}

void VstInstr::addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume)
{
    VstMidiEvent* pEv = &(MidiEvents[numevents]);

    unsigned char velocity = (unsigned char)((volume <= 1) ? volume * 100 : 100 + (volume - 1)/(DAW_VOL_RANGE - 1)*0x1B); //volume is the value from 0 to 1, if it's greater then it's gaining
/*
struct VstMidiEvent		// to be casted from a VstEvent
{
    long type;			// kVstMidiType
    long byteSize;		// 24
    long deltaFrames;	// sample frames related to the current block start sample position
    long flags;			// none defined yet

    long noteLength;	// (in sample frames) of entire note, if available, else 0
    long noteOffset;	// offset into note from note start if available, else 0

    char midiData[4];	// 1 thru 3 midi bytes; midiData[3] is reserved (zero)
    char detune;		// -64 to +63 cents; for scales other than 'well-tempered' ('microtuning')
    char noteOffVelocity;
    char reserved1;		// zero
    char reserved2;		// zero
};
*/
    memset(pEv, 0, sizeof(VstMidiEvent));

    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);

    //pEv->deltaFrames = (num_frames < total_frames - frame_phase)? (num_frames - 1): (total_frames - frame_phase);
    //pEv->noteLength = total_frames - frame_phase;
    //pEv->noteOffset = frame_phase;

    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;

    pEv->flags = 0;

    if (frame_phase == 0)
    {
        pEv->midiData[0] = (char)0x90;
        pEv->midiData[1] = note;
        pEv->midiData[2] = velocity;

        numevents++;
    }
    //else if (frame_phase < total_frames)
    //{
    //    pEv->midiData[0] = (char)0xa0;
    //    pEv->midiData[1] = note;
    //    pEv->midiData[2] = velocity;
    //    numevents++;
    //}
    else if(frame_phase > total_frames)
    {
        pEv->midiData[0] = (char)0x80;
        pEv->midiData[1] = note;
        pEv->midiData[2] = velocity;

        numevents++;
    }
}

void VstInstr::postNoteON(int note, float vol)
{
    VstMidiEvent* pEv = &(MidiEvents[numevents]);

    //volume is the value from 0 to 1, if it's greater then its gaining

    unsigned char l_velocity = (unsigned char)((vol <= 1) ? vol * 127 : 100 + (vol - 1)/(DAW_VOL_RANGE - 1)*0x1B);

    memset(pEv, 0, sizeof(VstMidiEvent));

    pEv->type = kVstMidiType;

    pEv->byteSize = sizeof(VstMidiEvent);
    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0x90;
    pEv->midiData[1] = note;
    pEv->midiData[2] = l_velocity;

    numevents++;
}

void VstInstr::postNoteOFF(int note, int velocity)
{
    VstMidiEvent* pEv = &(MidiEvents[numevents]);

    memset(pEv, 0, sizeof(VstMidiEvent));

    pEv->type = kVstMidiType;

    pEv->byteSize = sizeof(VstMidiEvent);

    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0x80;
    pEv->midiData[1] = note;
    pEv->midiData[2] = velocity;

    numevents++;
}

void VstInstr::stopAllNotes()
{
    VstMidiEvent* pEv = &(MidiEvents[numevents]);

    memset(pEv, 0, sizeof(VstMidiEvent));

    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0xB0;
    pEv->midiData[1] = 0x7B;
    pEv->midiData[2] = 0x00;

    numevents++;
}

void VstInstr::save(XmlElement * instrNode)
{
    Instrument::save(instrNode);

    //XmlElement* effModule = new XmlElement(T("EffModule"));
    //eff->saveStateData(*instrNode, "Current");

    XmlElement* vstModule = new XmlElement(T("VstModule"));

    vst2->save(vstModule);

    instrNode->addChildElement(vstModule);

/*
    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    eff->GetStateInformation(m);
    state->addTextElement (m.toBase64Encoding());
    instrNode->addChildElement (state);
    */
}

void VstInstr::load(XmlElement * instrNode)
{
    Instrument::load(instrNode);

    XmlElement* vstModule = instrNode->getChildByName (T("VstModule"));

    if(vstModule != NULL)
    {
       vst2->load(vstModule);
    }

    //XmlElement* stateNode = instrNode->getChildByName(T("Module"));
    //if(stateNode != NULL)
    //{
    //    eff->restoreStateData(*stateNode);
    //}

/*
    const XmlElement* const state = instrNode->getChildByName (T("STATE"));

    if(state != 0)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        eff->SetStateInformation (m.getData(), m.getSize());
    }
*/
}

/*
void VSTGenerator::CopyDataToClonedInstrument(Instrument * instr)
{
    Instrument::CopyDataToClonedInstrument(instr);

    VSTGenerator* nvst = (VSTGenerator*)instr;

    MemoryBlock m;
    pEff->GetStateInformation(m);
    nvst->pEff->SetStateInformation(m.getData(), m.getSize());
}
*/
 

void VstInstr::reset()
{
    stopAllNotes();
}

void VstInstr::setBufferSize(unsigned int bufferSize)
{
    Instrument::setBufferSize(bufferSize);

    vst2->setBufferSize(bufferSize);
}

void VstInstr::setSampleRate(float sampleRate)
{
    Instrument::setSampleRate(sampleRate);

    vst2->setSampleRate(sampleRate);
}

bool VstInstr::onUpdateDisplay()
{
    if (isLoading == true || !MProject.isLoading())
    {
        //This effect or current project is loading

        return false;
    }

    vst2->updatePresets();

    presets = vst2->presets;         // sync lists 

    // Update pointer to current active preset

    BrwEntry* updpreset = getPreset(vst2->getProgram());

    if (currPreset != updpreset)
    {
        currPreset = updpreset;
    }

    window->redraw();

    return true;
}

SubWindow* VstInstr::createWindow()
{
    if(vst2->hasGui())
    {
        return window->addWindow(new VstComponent(vst2, this));
    }
    else
    {
        return window->addWindow(new DevParamObject(this));
    }
}

//}


