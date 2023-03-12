
#include "36_vstinstr.h"
#include "36_vst.h"
#include "36_events_triggers.h"
#include "36_note.h"
#include "36_params.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_instrpanel.h"
#include "36_project.h"
#include "36.h"
#include "36_browser.h"
#include "36_vstwin.h"
#include "36_devwin.h"







/*
VstEffect::VstEffect(char* path)
{
    internal = false;
    isLoading = true;

    objId = "eff.vst";

    presetPath = ".\\Data\\Presets\\";

    // While loading some VST plugins current working directory somehow gets changed
    // Thus we need to remember it and restore in the end of initialization, hehehe
    char workdir[MAX_PATH_LENGTH] = {0};
    int drive = _getdrive();
    _getdcwd(drive, workdir, MAX_PATH_LENGTH - 1 );

    // Pass NULL if you want to see OpenFile dialog

    vst2 = VstHost->loadModuleFromFile(path);

    if (vst2 != NULL)
    {
        uniqueId = vst2->aeff->uniqueID;

        if (path)
        {
            filePath = path;
        }
        else
        {
            filePath = vst2->vstpath;
        }

        objName = vst2->objName;

        extractParams();
        presetPath = "Effects\\VST\\";

        updatePresets();
    }

    //  pPlug->pEffect->EffSuspend();
    // Restore previously saved working directory
    _chdir(workdir);

    isLoading = false;
}
*/


Vst2Module::Vst2Module(char* fullpath, Vst2Module* vst)
{
    internal = false;
    numEvents = 0;

    isLoading = true;

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

        pres = vst2->pres;
    }

    isLoading = false;
}

Vst2Module::~Vst2Module()
{
    if (guiWindow)
    {
        guiWindow->setOpen(false);
    }

    if (vst2 != NULL)
    {
        VstHost->removeModule(vst2);

        delete vst2;
    }
}

Vst2Module* Vst2Module::clone()
{
    Vst2Module* clone = new Vst2Module(NULL, this);

    MemoryBlock m;

    vst2->getStateInformation(m);

    clone->vst2->setStateInformation(m.getData(), m.getSize());

/*
    // Set the current preset equal to its parent one

    if (this->CurrentPreset != NULL)
    {
        clone->SetPreset(this->CurrentPreset->name, this->CurrentPreset->native);
    }

    // Now go through the list of parameters and tune them up to the parent effect

    Parameter* pParam = this->firstParam;
    Parameter* pCloneParam = clone->firstParam;

    while((pParam != NULL) && (pCloneParam != NULL))
    {
        clone->ParamUpdate(pParam);
        pCloneParam->SetNormalValue(pParam->val);
        pParam = pParam->next;
        pCloneParam = pCloneParam->next;
    }
*/

    return clone;
}

void Vst2Module::checkBounds(Note * gnote, Trigger * tg, long num_frames)
{
    if(tg->framePhase + num_frames >= gnote->getFrameLength() ||
                            tg->tgState == TS_Release || tg->tgState == TS_Finished ||tg->tgState == TS_SoftFinish)
    {
        // Check: Or better stop?

        tg->stop();
    }
}

long Vst2Module::handleTrigger(Trigger* tg, long num_frames, long buffframe)
{
    long loc_num_frames = num_frames;

    Note* gnote = (Note*)tg->el;

    float vol = 1; // tg->tgPatt->vol->val

    vol *= gnote->getVol()->getValue();

    if(!tg->previewTrigger)
    {
        if(gnote->getFrameLength() - tg->framePhase < loc_num_frames)
        {
            loc_num_frames = gnote->getFrameLength() - tg->framePhase;
        }

        addNoteEvent(tg->noteVal, loc_num_frames, tg->framePhase, gnote->getFrameLength(), vol);

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

void Vst2Module::postProcessTrigger(Trigger* tg, long num_frames, long buff_frame, long mix_buff_frame, long remaining)
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

    tc = mix_buff_frame*2;
    tc0 = buff_frame*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        if(penv1 != NULL)
        {
            panVal = penv1->buffoutval[mix_buff_frame + cc];

            // update pan
            // PanConstantRule(pan, &volL, &volR);

            ai = int((PI_F*(panVal + 1)/4)/wt_angletoindex);

            volL = wt_cosine[ai];
            volR = wt_sine[ai];
        }
    
        outBuff[tc0] = tempBuff[tc0++]*volL;
        outBuff[tc0] = tempBuff[tc0++]*volR;
    }
}

void Vst2Module::deactivateTrigger(Trigger* tg)
{
    Note* gnote = (Note*)tg->el;

    postNoteOFF(tg->noteVal, 127);

    Device36::deactivateTrigger(tg);
}

void Vst2Module::vstProcess(float* in_buff, long num_frames, long buff_frame)
{
    /*
    struct VstEvents            // a block of events for the current audio block
    {
        long numEvents;
        long reserved;          // zero
        VstEvent* events[2];    // variable
    };
    */

    VstEvents* events = NULL;

    if(numEvents > 0)
    {
        unsigned short      NumEvents = (unsigned short)numEvents;
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

    vst2->processDSP(in_buff, &tempBuff[buff_frame*2], num_frames);

    numEvents = 0;

    // This freeing code was working here, but it causes Absynth 2.04 to crash, while other hosts work
    // well with it. Probably it's responsibility of a plugin to free the memory allocated for VstEvents
    // struct, although need to ensure later in this. Hahaha.
    //if(NULL != events)
    //{
    //    free(events);
    //}

}


void Vst2Module::processDSP(float * in_buff,float * out_buff,int num_frames)
{
    // pVSTCollector->AcquireSema();
    // pPlug->pEffect->EffResume();

    vst2->processDSP(in_buff, out_buff, num_frames);

    // pPlug->pEffect->EffSuspend();
    // pVSTCollector->ReleaseSema();
}

void Vst2Module::generateData(float* in_buff, float* out_buff, long num_frames, long mix_buff_frame)
{
    bool off = false;

    /*
    if((muteparam != NULL && muteparam->getOutVal()) || (SoloInstr != NULL && SoloInstr != this))
    {
        off = true;
    }
    */

    bool fill = true;

    for(auto itr = activeTriggers.begin(); itr != activeTriggers.end(); )
    {
        Trigger* tg = *itr;
        itr++;

        handleTrigger(tg, num_frames);
    }

    memset(outBuff, 0, num_frames*sizeof(float)*2);
    memset(tempBuff, 0, num_frames*sizeof(float)*2);

    if(envelopes == NULL)
    {
        vstProcess(in_buff, num_frames, 0);
    }
    else
    {
        Envelope* env;
        Parameter* param;
        Trigger* tgenv;
        long frames_to_process;
        long frames_remaining = num_frames;
        long buffFrame = 0;

        while(frames_remaining > 0)
        {
            if(frames_remaining > BUFF_PROCESSING_CHUNK_SIZE)
            {
                frames_to_process = BUFF_PROCESSING_CHUNK_SIZE;
            }
            else
            {
                frames_to_process = frames_remaining;
            }

            /*
            tgenv = envelopes;
            while(tgenv != NULL)
            {
                env = (Envelope*)tgenv->el;
                param = ((Envelope*)tgenv->el)->param;
                param->setValueFromEnvelope(env->buff[mix_buff_frame + buffFrame], env);
                tgenv = tgenv->group_prev;
            }
            */

            vstProcess(in_buff, frames_to_process, buffFrame);

            frames_remaining -= frames_to_process;

            buffFrame += frames_to_process;
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

                tempBuff[tc*2] *= aa;
                tempBuff[tc*2 + 1] *= aa;

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

                tempBuff[tc*2] *= aa;
                tempBuff[tc*2 + 1] *= aa;

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
        //postProcessTrigger(NULL, num_frames, 0, mix_buff_frame);

        //memcpy(outBuff, tempBuff, num_frames*2);

        for(long cc = 0; cc < num_frames; cc++)
        {
            outBuff[cc*2] = tempBuff[cc*2];
            outBuff[cc*2] = tempBuff[cc*2];
        }

        fillOutputBuffer(out_buff, num_frames, 0, mix_buff_frame);
    }
}

void Vst2Module::addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume)
{
    VstMidiEvent* pEv = &(MidiEvents[numEvents]);

    //volume is the value from 0 to 1, if it's greater then it's gaining

    unsigned char velocity = (unsigned char)((volume <= 1) ? volume * 100 : 100 + (volume - 1)/(DAW_VOL_RANGE - 1)*0x1B);

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

        numEvents++;
    }
    //else if (frame_phase < total_frames)
    //{
    //    pEv->midiData[0] = (char)0xa0;
    //    pEv->midiData[1] = note;
    //    pEv->midiData[2] = velocity;
    //    numEvents++;
    //}
    else if(frame_phase > total_frames)
    {
        pEv->midiData[0] = (char)0x80;
        pEv->midiData[1] = note;
        pEv->midiData[2] = velocity;

        numEvents++;
    }
}

void Vst2Module::postNoteON(int note, float vol)
{
    VstMidiEvent* pEv = &(MidiEvents[numEvents]);

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

    numEvents++;
}

void Vst2Module::postNoteOFF(int note, int velocity)
{
    VstMidiEvent* pEv = &(MidiEvents[numEvents]);

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

    numEvents++;
}

void Vst2Module::stopAllNotes()
{
    VstMidiEvent* pEv = &(MidiEvents[numEvents]);

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

    numEvents++;
}

void Vst2Module::save(XmlElement * instrNode)
{
   // Device36::save(instrNode);

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

void Vst2Module::load(XmlElement * instrNode)
{
    //Device36::load(instrNode);

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
 

void Vst2Module::reset()
{
    stopAllNotes();

    vst2->reset();
}

void Vst2Module::setBufferSize(unsigned int bufferSize)
{
    Device36::setBufferSize(bufferSize);

    vst2->setBufferSize(bufferSize);
}

void Vst2Module::setSampleRate(float sampleRate)
{
    Device36::setSampleRate(sampleRate);

    vst2->setSampleRate(sampleRate);
}

bool Vst2Module::setPreset(std::string pname)
{
    return vst2->setPreset(pname.data());
}

bool Vst2Module::setPreset(long index)
{
    return vst2->setPreset(index);
}

bool Vst2Module::onUpdateDisplay()
{
    // here was redundant code for highlighting current preset

    return true;
}

SubWindow* Vst2Module::createWindow()
{
    if(vst2->hasGui())
    {
        return MObject->addWindow(new VstComponent(vst2, this));
    }
    else
    {
        return MObject->addWindow((WinObject*)new DevParamObject(this));
    }
}


void Vst2Module::handleParamUpdate(Parameter* param)
{
    if (param != vol && param != pan && param != enabled)
    {
        vst2->handleParamUpdate(param);
    }
}


void Vst2Module::processEvents(VstEvents *pEvents)
{
    vst2->processEvents(pEvents);
}



