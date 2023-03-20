//
// Revision History:
//
// Author               Date                        Major Changes
//
// Alex V               07/23/2008                  Initial version
// Denis L              11/15/2020                  Reworked version
//

#include "36.h"
#include "36_vst.h"
#include "36_utils.h"
#include "36_config.h"
#include "36_audio_dev.h"
#include "36_transport.h"
#include "36_params.h"
#include "36_vstwin.h"
#include "36_window.h"
#include "36_events_triggers.h"
#include "36_note.h"
#include "36_devwin.h"

#include <windows.h>
#include <direct.h>



Vst2Host*      VstHost = NULL;

// copied from Juce for working with VST programs

const int defaultMaxSizeMB = 64;
const int fxbVersionNum = 1;


struct fxProgram
{
    long        chunkMagic;         // 'CcnK'
    long        byteSize;           // of this chunk, excl. magic + byteSize
    long        fxMagic;            // 'FxCk'
    long        version;
    long        fxID;               // fx unique id
    long        fxVersion;
    long        numParams;
    char        prgName[28];
    float       params[1];          // variable no. of parameters
};

struct fxSet
{
    long        chunkMagic;         // 'CcnK'
    long        byteSize;           // of this chunk, excl. magic + byteSize
    long        fxMagic;            // 'FxBk'
    long        version;
    long        fxID;               // fx unique id
    long        fxVersion;
    long        numPrograms;
    char        future[128];
    fxProgram   programs[1];        // variable no. of programs
};

struct fxChunkSet
{
    long        chunkMagic;         // 'CcnK'
    long        byteSize;           // of this chunk, excl. magic + byteSize
    long        fxMagic;            // 'FxCh', 'FPCh', or 'FBCh'
    long        version;
    long        fxID;               // fx unique id
    long        fxVersion;
    long        numPrograms;
    char        future[128];
    long        chunkSize;
    char        chunk[8];           // variable
};

struct fxProgramSet
{
    long        chunkMagic;         // 'CcnK'
    long        byteSize;           // of this chunk, excl. magic + byteSize
    long        fxMagic;            // 'FxCh', 'FPCh', or 'FBCh'
    long        version;
    long        fxID;               // fx unique id
    long        fxVersion;
    long        numPrograms;
    char        name[28];
    long        chunkSize;
    char        chunk[8];           // variable
};


typedef AEffect* (*VstInitFunc) (long (*audioMaster)(AEffect *effect, long opcode, long index, long value, void *ptr, float opt));




void Vst2Plugin::checkBounds(Note * gnote, Trigger * tg, long num_frames)
{
    if(tg->framePhase + num_frames >= gnote->getFrameLength() ||  tg->tgState == TS_Release || tg->tgState == TS_Finished ||tg->tgState == TS_SoftFinish)
    {
        // Check: Or better stop?

        tg->stop();
    }
}

long Vst2Plugin::handleTrigger(Trigger* tg, long num_frames, long buffframe)
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

void Vst2Plugin::postProcessTrigger(Trigger* tg, long num_frames, long buff_frame, long mix_buff_frame, long remaining)
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

void Vst2Plugin::deactivateTrigger(Trigger* tg)
{
    Note* gnote = (Note*)tg->el;

    postNoteOFF(tg->noteVal, 127);

    Device36::deactivateTrigger(tg);
}

void Vst2Plugin::vstProcess(float* in_buff, long num_frames, long buff_frame)
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
            events->events[i] = (VstEvent*) &(midiEvents[i]);
        }

        aeffProcessEvents(events);
    }

    processDSP(in_buff, &tempBuff[buff_frame*2], num_frames);

    numEvents = 0;

    // This freeing code was working here, but it causes Absynth 2.04 to crash, while other hosts work
    // well with it. Probably it's responsibility of a plugin to free the memory allocated for VstEvents
    // struct, although need to ensure later in this. Hahaha.
    //if(NULL != events)
    //{
    //    free(events);
    //}

}

void Vst2Plugin::generateData(float* in_buff, float* out_buff, long num_frames, long mix_buff_frame)
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

void Vst2Plugin::addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume)
{
    VstMidiEvent* pEv = &(midiEvents[numEvents]);

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

void Vst2Plugin::postNoteON(int note, float vol)
{
    VstMidiEvent* pEv = &(midiEvents[numEvents]);

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

void Vst2Plugin::postNoteOFF(int note, int velocity)
{
    VstMidiEvent* pEv = &(midiEvents[numEvents]);

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

void Vst2Plugin::stopAllNotes()
{
    VstMidiEvent* pEv = &(midiEvents[numEvents]);

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



SubWindow* Vst2Plugin::createWindow()
{
    if(hasGui())
    {
        return MObject->addWindow(new VstComponent(this));
    }
    else
    {
        return MObject->addWindow((WinObject*)new DevParamObject(this));
    }
}



Vst2Plugin* Vst2Plugin::clone()
{
    Vst2Plugin* clone = new Vst2Plugin(filePath);

    MemoryBlock m;

    getStateInformation(m);

    clone->setStateInformation(m.getData(), m.getSize());

    return clone;
}







Vst2Plugin::Vst2Plugin(std::string path)
{
    aeff = NULL;

    vstPath = NULL;
    hmodule = NULL;
    vstDir = NULL;
    vstGuiWin = NULL;
    inBuffs = NULL;
    outBuffs = NULL;
    guiOpen = false;
    needIdle = false;
    inEditIdle = false;
    wantsMidi = false;
    settingProgram = false;
    vstIndex = -1;
    isReplacing = true;
    _hasGui = false;
    generator = false;

    numIns = numOuts = 0;

    loadAndInit(path.data());

    if (aeff != NULL)
    {
        VstHost->addModule(this);

        numEvents = 0;

        uniqueId = aeff->uniqueID;

        setPath(path);

        objName = ToUpperCase(objName);

        extractParams();

        updatePresets();
    }
}

Vst2Plugin::~Vst2Plugin()
{
    WaitForSingleObject(vstMutex,INFINITE);

    int i;

    if (inBuffs != NULL)
    {
        for (i = 0; i < numIns; ++i)
        {
            if(inBuffs[i] != NULL)
            {
                delete((void*)inBuffs[i]);

                inBuffs[i] = NULL;
            }
        }

        delete((void *) inBuffs);

        inBuffs = NULL;
    }

    for (i = 0; i < numOuts; ++i)
    {
        if (outBuffs[i] != NULL)
        {
            delete((void*)outBuffs[i]);

            outBuffs[i] = NULL;
        }
    }

    for (; i < numOuts; ++i)  outBuffs[i] = NULL;

    delete((void *) outBuffs);

    if(aeff != NULL)
    {
        aeffClose(); 

        // if DLL instance available
        if (hmodule)
        {
            // remove it

            FreeLibrary(hmodule);

            hmodule = NULL; 
        }

        if (vstDir)
        {
            delete[] vstDir;

            vstDir = NULL;
        }

        if (vstPath) 
        {
            delete[] vstPath;

            vstPath = NULL;
        }
    }

    VstHost->removeModule(this);

    ReleaseMutex(vstMutex);
}

void Vst2Plugin::loadAndInit(const char *path) 
{
    ERect *pRC = NULL;

    if (!loadFromDll(path))                      // try to load the thing
    {
        return;                                 // and regretfully return error
    }

    aeffEditGetRect(&pRC);

    if (pRC)
    {
        _hasGui = true;
    }

    _usesChunks = aeffUsesChunks();

    int i, j;  

    if (aeff->numInputs)                // allocate input pointers
    {
        // We need to allocate input and output buffer arrays on plugin initialization

        inBuffs = new float *[aeff->numInputs];

        for (i = 0; i < aeff->numInputs; ++i)
        {
            inBuffs[i] = new float[24000];

            for (j = 0; j < 24000; ++j)
            {
                inBuffs[i][j] = 0;
            }
        }

        long catg = aeffGetPlugCategory();

        if (catg == kPlugCategGenerator || catg == kPlugCategSynth)
        {
            generator = true;
        }
    }
    else
    {
        generator = true;
    }

    numIns = aeff->numInputs;

    if (aeff->numOutputs)                   // allocate output pointers
    {
        int nAlloc;

        if (aeff->numOutputs < 2)
        {
            nAlloc = 2;
        }
        else
        {
            nAlloc = aeff->numOutputs;
        }

        outBuffs = new float *[nAlloc];      // and the buffers pointed to

        for (i = 0; i < aeff->numOutputs; i++)
        {
            // for this sample project, I've assumed a maximum buffer size
            // of 1/4 second at 96KHz - presumably, this is MUCH too much
            // and should be tweaked to more reasonable values, since it
            // requires a machine with lots of main memory this way...
            // user configurability would be nice, of course.

            outBuffs[i] = new float[24000];

            for (j = 0; j < 24000; j++)
            {
                outBuffs[i][j] = 0.f;
            }
        }

        // if less than 2, fill up by replicating buffer 0 pointer

        for (; i < nAlloc; ++i)
        {
            outBuffs[i] = outBuffs[0];
        }

        numOuts = nAlloc;
    }


    // Init sequence

    aeffOpen();                                 // open the effect                   

    aeffSetSampleRate(VstHost->getSampleRate());       // adjust its sample rate            

    // this is a safety measure against some plusgins that only set their buffers
    // ONCE - this should ensure that they allocate a buffer that's large enough

    aeffSetBlockSize(11025);

    // deal with changed behaviour in V2.4 plugins that don't call wantEvents()

    wantsMidi = (aeffCanDo("receiveVstMidiEvent") == 1);

    aeffResume();                            // then force resume
    aeffSuspend();                           // suspend again...
    aeffSetBlockSize(VstHost->getBuffSize());   // and buffer size
    aeffResume();                            // then force resume

    vstMutex = VstHost->vstMutex;

    char tmpname[30] = {};

    getDisplayName(tmpname, 30);

    objName = tmpname;
}

bool Vst2Plugin::loadFromDll(const char *path) 
{
    // pointer to main function 

    VstInitFunc vstMainFunc = NULL;

    hmodule = LoadLibrary(path);                // try to load the DLL 

    if (hmodule)                                // if there, get its main() function 
    {
        vstMainFunc = (VstInitFunc)GetProcAddress(hmodule, "VSTPluginMain");

        if (!vstMainFunc)
        {
            vstMainFunc = (VstInitFunc)GetProcAddress(hmodule, "main");
        }
    }

    if (vstMainFunc)                              // initialize effect  
    {
        //__try
        try
        {
            aeff = vstMainFunc(VstHost->audioMasterCallback);
        }
        //__except (EXCEPTION_EXECUTE_HANDLER)
        catch(...)                              // if any error occured 
        {
            aeff = NULL;
        }

        if (aeff == NULL)
        {
            FreeLibrary(hmodule);

            hmodule = NULL;
        }
    }

    // check for correctness

    if (aeff && (aeff->magic != kEffectMagic))
    {
        aeff = NULL;
    }

    if (aeff)
    {
        unsigned int len = strlen(path);

        vstPath = new char[len + 1];

        if (vstPath)
        {
            strcpy(vstPath, path);
        }

        const char *p = strrchr(path, '\\');

        if (p)
        {
            vstDir = new char[p - path + 1];

            if (vstDir)
            {
                memcpy(vstDir, path, p - path);

                vstDir[p - path] = '\0';
            }
        }
    }

    return !!aeff;
}

long Vst2Plugin::vstDispatch(const int opcode,const int index,const int value,void * const ptr,float opt)
{
    return aeffDispatch(opcode, index, value, ptr, opt);
}

void Vst2Plugin::idle()
{
    aeffIdle();
}

void Vst2Plugin::editIdle()
{
    aeffEditIdle();
}

//
// Wrapper for standart VST ProcessData function.
//      input and outpud flows passed in has the following format:
//      frame[0], frame[1],...,frame[buff_size-1]
//      Where frame is: float(L), float(R) pair.

//      Currently process function supports only mono and stereo data to in and out.
//      All extra channels will be ignored or filled in by duplication of first two channels.

// ARGS:
//   in_buff   - pointer to the data to be processed (in internal program format)
//   out_buff  - pointer to the processed data (in internal program format)
//   buff_size - num of data frames (pair of L & R channel value) in incoming stream buffer

void Vst2Plugin::processDSP(float* in_buff, float* out_buff, int buff_size)
{
    // pVSTCollector->AcquireSema();
    // pPlug->pEffect->EffResume();

    int i,j;

    WaitForSingleObject(vstMutex,INFINITE);

    //If effect has only one input then mix both L and R to one mono channel

    if (in_buff != NULL)
    {
        if (numIns == 1)
        {
            for (j = 0; j < buff_size; ++j)
            {
                inBuffs[0][j] = (in_buff[j*2] + in_buff[j*2+1])/2;
            }
        }
        // if effect has more than one input
        else if (numIns > 1)
        {
            for (j = 0; j < buff_size; ++j)
            {
                //fill in only first two channels

                inBuffs[0][j] = in_buff[j*2];
                inBuffs[1][j] = in_buff[j*2+1];
            }

            //all extra inputs will be duplicated to firts input (we don't support them)

            for (i = 2; i < numIns; ++i)
            {
                inBuffs[i] = inBuffs[0];
            }
        }
    }

    //sanity action: just clear output buffer

    memset(out_buff, 0, (sizeof(float)*2)*buff_size);

    //Actually, plugins can support two data processing methods:
    // First is Process - out += process(in)
    // Second is ProcessReplacing - out = process(in)

    if (aeff->flags & effFlagsCanDoubleReplacing && isReplacing)
    {
        /* pEffect->EffProcessDoubleReplacing(
            (double *)inBufs,
            (double *)outBufs,
            buff_size);
        */
    }
    else if (aeff->flags & effFlagsCanReplacing && isReplacing)
    {
        aeffProcessReplacing(inBuffs, outBuffs, buff_size);
    }
    else
    {
        aeffProcess(inBuffs, outBuffs, buff_size);
    }

    for (i = 0; i < buff_size; ++i)
    {
        //lets duplicate output if effect has only one

        if (numOuts == 1)
        {
            out_buff[i*2] = out_buff[i*2+1] = outBuffs[0][i];
        }
        else if (numOuts > 1)//effect has more than one buffer
        {
            //use only first two channels and ignore all other if any

            out_buff[i*2] = outBuffs[0][i];
            out_buff[i*2+1] = outBuffs[1][i];
        }
    }

    ReleaseMutex(vstMutex);

    // pPlug->pEffect->EffSuspend();
    // pVSTCollector->ReleaseSema();
}

void Vst2Plugin::setParam(long index, float Value)
{
    aeffSetParameter(index, Value);
}

float Vst2Plugin::getParam(long index)
{
    return aeffGetParameter(index);
}

unsigned int Vst2Plugin::getNumParams()
{
    return aeff->numParams;
}

void Vst2Plugin::getDisplayValue(long index, char** ppvalDisp)
{
    if (ppvalDisp != NULL)
    {
        if (*ppvalDisp != NULL)  free(*ppvalDisp);

       *ppvalDisp = (char*)malloc(sizeof(char) * VST_MAX_PARAM_VALUE_LENGTH);

        memset(*ppvalDisp, 0, VST_MAX_PARAM_VALUE_LENGTH);

        aeffGetParamDisplay(index, *ppvalDisp);
    }
}
void Vst2Plugin::getParamLabel(long index, char **pplabel)
{
    if (pplabel != NULL)
    {
        if (*pplabel != NULL)
        {
            free(*pplabel);
        }

       *pplabel = (char*)malloc(sizeof(char) * VST_MAX_PARAM_LABEL_LENGTH);

        memset(*pplabel, 0, VST_MAX_PARAM_LABEL_LENGTH);

        aeffGetParamLabel(index, *pplabel);
    }
}

void Vst2Plugin::getParamName(long index, char** ppname)
{
    if (ppname != NULL)
    {
        if (*ppname != NULL)
        {
            free(*ppname);
        }

       *ppname = (char*) malloc(sizeof(char) * VST_MAX_NAME_LENGTH);

        aeffGetParamName(index, *ppname);
    }
}

// If length is 0 it means we must return whole name without extension
void Vst2Plugin::getDisplayName(char *name, unsigned int length)
{
    char namebuff[MAX_NAME_LENGTH] = {};

    size_t len = 0;

    WaitForSingleObject(vstMutex,INFINITE);

    aeffGetProductString(namebuff);

    if (strlen(namebuff) == 0)
    {
        size_t      i = strlen(vstPath);
        unsigned    j = 0;
        bool        dots = false;

        // Go backward till find a dot in file name

        while (vstPath[i--] != '.');

        // Go backward until reach the first "\\"

        while (vstPath[i--] != '\\')
        {
            ++len; // Here we calc length of file name (without extension)
        }

        i+=2;

        if ((length != 0) && (len > length))
        {
            len = length - 3; // reserve space for three dotes in the end
            dots = true;
        }

        j = 0;

        while ((len--) != 0)
        {
            name[j++] = vstPath[i++];
        }

        if (dots == true)
        {
            strcat(name, "...");
        }
    }
    else
    {
        len = strlen(namebuff);

        if (length != 0)
        {
            len = (length > strlen(namebuff) ? strlen(namebuff) : length);
        }

        strncpy(name, namebuff, len);
    }

    ReleaseMutex(vstMutex);
}

void Vst2Plugin::processEvents(VstEvents* pEvents)
{
    aeffProcessEvents(pEvents);
}

void Vst2Plugin::setBufferSize(unsigned int bufferSize)
{
    aeffStopProcess();
    aeffSuspend();

    aeffSetBlockSize(bufferSize);

    aeffResume();
    aeffStartProcess();
}

void Vst2Plugin::setSampleRate(float sampleRate)
{
    aeffStopProcess();
    aeffSuspend();

    aeffSetSampleRate(sampleRate);

    aeffResume();
    aeffStartProcess();
}

void Vst2Plugin::reset()
{
    stopAllNotes();

    /* // Absynt crashes everything when the below code works because of
    // conflict with StopAllNotes function in VSTGenerator on stopping. Probably unneeded.
    VstEvents myEvents;

    myEvents.numEvents = 1;
    myEvents.reserved = 0;
    myEvents.events[0] = (VstEvent*)malloc(sizeof(VstEvent));
    myEvents.events[1] = NULL;
    VstMidiEvent * pMidi = (VstMidiEvent *)(myEvents.events[0]);
    if (myEvents.events[0] != NULL)
    {
        pMidi->devType = kVstMidiType;
        pMidi->byteSize = sizeof(VstMidiEvent);
        pMidi->deltaFrames = 0;
        pMidi->flags = 0;
        pMidi->noteLength = 0;
        pMidi->noteOffset = 0;
        pMidi->noteOffVelocity = 127;
        pMidi->midiData[0] = char(0xB0); // to avoid some warnings
        pMidi->midiData[1] = 0x7B;
        pMidi->midiData[2] = 0x00;

        if( (this->pPlug != NULL) && (this->pPlug->pEffect != NULL) )
        {
            this->pPlug->pEffect->EffProcessEvents(&myEvents);
        }

        free(myEvents.events[0]);
    }*/

    //That's strange - VST SDK says that Processing should be stopped
    // However, I noticed that it causes FX's and synths to keep their processing state
    // So, when you restart playback for example, you can hear that synthesis continues but not starts from beginning
    //ceff->EffStopProcess();

    aeffSuspend();
    aeffResume();
    //ceff->EffStartProcess();
}

void Vst2Plugin::updatePresets()
{
    long            currentProgram          = getProgram();
    long            num_presets             = getNumPresets();
    char            bzName[MAX_NAME_LENGTH] = {0};

    deletePresets();

    for (int idx = 0; idx < num_presets; ++idx)
    {
        memset(bzName, 0, MAX_NAME_LENGTH * sizeof(char));

        //this->pPlug->setProgram(idx);
        aeffGetProgramNameIndexed(0, idx, bzName);

        if(bzName[0] != '\0')
        {
            std::string nm = bzName;
            presets.push_back(nm);
        }
    }
}

void Vst2Plugin::extractParams()
{
    // Get all parameters and add them to list of parameters for the effect

    int                     index      = 0;
    int                     NumParam   = getNumParams();
    char                   *paramName  = NULL;
    Parameter              *param      = NULL;
    float                   fVal       = 0;
    VstParameterProperties *paramProp  = NULL;

    for (index = 0; index < NumParam; ++index)
    {
        if ( aeffCanBeAutomated(index) == 0 )
        {
            //Don't add parameters which are useless  (todo: check why)
            continue;
        }

        getParamName(index, &paramName);

        if(strlen(paramName) <= MAX_NAME_LENGTH)
        {
            fVal = getParam(index);

            if(fVal > 1)
                fVal = 1;
            else if(fVal < 0)
                fVal = 0;

            if (aeffGetParameterProperties(index, paramProp) == 1)
            {
                if ( (paramProp->flags & kVstParameterIsSwitch) != 0 )
                {
                    param = new Parameter(0, 1, fVal);
                    param->setInterval(1.f);
                }
                else if ( (paramProp->flags & kVstParameterUsesFloatStep) != 0 )
                {
                    param = new Parameter(0, 1, fVal);
                    param->setInterval(paramProp->smallStepFloat);
                }
                else if ( ((paramProp->flags & kVstParameterUsesIntStep) != 0) &&
                          ((paramProp->flags & kVstParameterUsesIntegerMinMax) != 0) )
                {
                    float step = 1.0f/(paramProp->maxInteger - paramProp->minInteger);

                    param = new Parameter(0, 1, fVal);
                    param->setInterval(step);
                }
                else
                {
                    param = new Parameter(0, 1, fVal);
                }
            }
            else
            {
                param = new Parameter(0, 1, fVal);
            }

            //copy only MAX_PARAM_NAME number of chars to prevent corruption
            //strncpy(param->paramName, paramName, MAX_NAME_LENGTH);

            param->setName(paramName);
            param->setIndex(index);      // for VST purpose

            updParamValString(param);

            addParam(param);
        }
    }
}

void Vst2Plugin::updParamValString(Parameter* param)
{
    char      *dispVal    = NULL;
    char      *paramLabel = NULL;

    getDisplayValue(param->getIndex(),&dispVal);
    getParamLabel(param->getIndex(), &paramLabel);

    char label[MAX_NAME_LENGTH] = {};

    strncpy(label, dispVal, min(MAX_NAME_LENGTH - strlen(paramLabel),strlen(dispVal)));
    strcat(label, paramLabel);

    param->setValString(label);

    if (NULL != dispVal)
    {
        free(dispVal);
        dispVal = NULL;
    }

    if (NULL != paramLabel)
    {
        free(paramLabel);
        paramLabel = NULL;
    }
}


void Vst2Plugin::handleParamUpdate(Parameter* param)
{
    if (param != vol && param != pan && param != enabled)
    {
        Parameter* prm = dynamic_cast<Parameter*>(param);

        if (prm != NULL && prm->getType() == Param_Default && getParamLock() == false)
        {
            setParam(prm->getIndex(), prm->getValue());

            // Update units string for the parameter

            updParamValString(prm);
        }
    }
}

bool Vst2Plugin::onSetParameterAutomated(long index,float value)
{
    for(Parameter* param : params)
    {
        if (param->getIndex() == index)
        {
            setParamLock(true);

            param->adjustFromControl(NULL, 0, value);

            setParamLock(false);

            break;
        }
    }

    return false;
}

void Vst2Plugin::syncParamValues()
{
    setParamLock(true);

    for(Parameter* param : params)
    {
        long idx = param->getIndex();

        float fVal = getParam(idx);

        if(fVal < 0)
        {
            fVal = 0;
        }
        else if(fVal > 1)
        {
            fVal = 1;
        }

        param->setValue(fVal);

        updParamValString(param);

        //param->setInitialValue(fVal);
    }

    setParamLock(false);
}

bool Vst2Plugin::setPreset(std::string name)
{
    if (name != "")
    {
        int idx = 0;

        for(std::string pname : presets)
        {
            if(pname == name)
            {
                setProgram(idx);

                syncParamValues();

                return true;
            }

            idx++;
        }
    }

    return false;
}

bool Vst2Plugin::setPreset(long index)
{
    if (index >=0 && index <= (long)presets.size())
    {
        setProgram(index);

        syncParamValues();

        return true;
    }

    return false;
}

long Vst2Plugin::getCurrentPreset()
{
    return aeffGetProgram();
}

void Vst2Plugin::setPresetName(char* new_name)
{
    aeffSetProgramName(new_name);
}

void Vst2Plugin::save(XmlElement * xmlContainer)
{
    XmlElement* vstModule = new XmlElement(T("VstModule"));

    XmlElement* state = new XmlElement("STATE");
    MemoryBlock m;
    getStateInformation(m);
    state->addTextElement(m.toBase64Encoding());
    vstModule->addChildElement(state);

    xmlContainer->addChildElement(vstModule);
}

void Vst2Plugin::load(XmlElement * xmlContainer)
{
    XmlElement* vstModule = xmlContainer->getChildByName (T("VstModule"));

    if(vstModule != NULL)
    {
        const XmlElement* const state = vstModule->getChildByName(T("STATE"));

        if(state != 0)
        {
            MemoryBlock m;

            m.fromBase64Encoding(state->getAllSubText());

            setStateInformation(m.getData(), m.getSize());
        }
    }
}

void Vst2Plugin::getStateInformation (MemoryBlock& destData)
{
    saveToFXBFile (destData, false, defaultMaxSizeMB);
}

void Vst2Plugin::setStateInformation (const void* data, int sizeInBytes)
{
    loadFromFXBFile (data, sizeInBytes);
}

void Vst2Plugin::getChunkData (MemoryBlock& mb, bool isPreset, int maxSizeMB)
{
    if (usesChunks())
    {
        void* data = 0;

        const int bytes = aeffDispatch(effGetChunk, isPreset ? 1 : 0, 0, &data, 0.0f);

        if (data != 0 && bytes <= maxSizeMB * 1024 * 1024)
        {
            mb.setSize (bytes);
            mb.copyFrom (data, 0, bytes);
        }
    }
}

void Vst2Plugin::setChunkData (const char* data, int size, bool isPreset)
{
    if (size > 0 && usesChunks())
    {
        aeffDispatch(effSetChunk, isPreset ? 1 : 0, size, (void*) data, 0.0f);

        //if (!isPreset)
        //    updateStoredProgramNames();
    }
}

const String Vst2Plugin::getCurrPresetName()
{
    char nm[MAX_NAME_LENGTH];

    getPresetName(getCurrentPreset(), nm);

    return String(nm).trim();
}

bool Vst2Plugin::restoreProgramSettings (const fxProgram* const prog)
{
    if (prog->chunkMagic == 'CcnK' && prog->fxMagic == 'FxCk')
    {
        setPresetName((char*)prog->prgName);

        for (int i = 0; i < prog->numParams; ++i)
        {
            setParam(i, prog->params[i]);

            Parameter* p = getParamByIndex(i);
            Parameter* param = dynamic_cast<Parameter*>(p);

            if (param)
            {
                param->setValue(prog->params[i]);
            }
        }

        return true;
    }

    return false;
}

bool Vst2Plugin::loadFromFXBFile (const void* const data, const int dataSize)
{
    if (dataSize < 28)  return false;

    const fxSet* const set = (const fxSet*) data;

    if (((set->chunkMagic) != 'CcnK' && (set->chunkMagic) != 'KncC') || (set->version) > fxbVersionNum)
    {
        return false;
    }

    if ((set->fxMagic) == 'FxBk')
    {
        // bank of programs
        if ((set->numPrograms) >= 0)
        {
            const int oldProg = getCurrentPreset();
            const int numParams = (((const fxProgram*) (set->programs))->numParams);
            const int progLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);

            for (int i = 0; i < (set->numPrograms); ++i)
            {
                if (i != oldProg)
                {
                    const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + i * progLen);

                    if(((const char*) prog) - ((const char*) set) >= dataSize)
                    {
                        return false;
                    }

                    if((set->numPrograms) > 0)
                    {
                        setPreset(i);
                    }

                    if(! restoreProgramSettings (prog))
                    {
                        return false;
                    }
                }
            }

            if(set->numPrograms > 0)  setPreset(oldProg);

            const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + oldProg * progLen);

            if (((const char*) prog) - ((const char*) set) >= dataSize)  return false;

            if (! restoreProgramSettings (prog))  return false;
        }
    }
    else if (set->fxMagic == 'FxCk')
    {
        // single program
        const fxProgram* const prog = (const fxProgram*) data;

        if (prog->chunkMagic != 'CcnK')  return false;

        setPresetName((char*)prog->prgName);

        for (int i = 0; i <  (prog->numParams); ++i)
        {
            setParam(i, prog->params[i]);

            Parameter* extparam =  getParamByIndex(i);

            Parameter* param = dynamic_cast<Parameter*>(extparam);

            if (param != NULL)
            {
                param->setValue(prog->params[i]);
            }
        }
    }
    else if ( (set->fxMagic) == 'FBCh' ||  (set->fxMagic) == 'hCBF')
    {
        // non-preset chunk
        const fxChunkSet* const cset = (const fxChunkSet*) data;

        if ( (cset->chunkSize) + sizeof (fxChunkSet) - 8 > (unsigned int) dataSize)  return false;

        setChunkData (cset->chunk,  (cset->chunkSize), false);
    }
    else if ( (set->fxMagic) == 'FPCh' || (set->fxMagic) == 'hCPF')
    {
        // preset chunk
        const fxProgramSet* const cset = (const fxProgramSet*) data;

        if (cset->chunkSize + sizeof (fxProgramSet) - 8 > (unsigned int) dataSize)  return false;

        setChunkData (cset->chunk, cset->chunkSize, true);

        setPresetName((char*)cset->name);
    }
    else
    {
        return false;
    }

    return true;
}

void Vst2Plugin::createTempParameterStore (MemoryBlock& dest)
{
    dest.setSize (64 + 4 * getNumParams());
    dest.fillWith (0);

    getCurrPresetName().copyToBuffer ((char*) dest.getData(), 63);
    float* const p = (float*) (((char*) dest.getData()) + 64);

    for (unsigned i = 0; i < getNumParams(); ++i)
    {
        p[i] = getParam(i);
    }
}

void Vst2Plugin::restoreFromTempParameterStore (const MemoryBlock& m)
{
    setPresetName((char*)m.getData());

    float* p = (float*) (((char*) m.getData()) + 64);

    for (unsigned i = 0; i < getNumParams(); ++i)
    {
        setParam(i, p[i]);
    }
}

void Vst2Plugin::setParamsInProgramBlock (fxProgram* const prog) throw()
{
    const int numParams = getNumParams();

    prog->chunkMagic = ('CcnK');
    prog->byteSize = 0;
    prog->fxMagic = ('FxCk');
    prog->version = (fxbVersionNum);
    prog->fxID = 0; //vst_swap (getUID());
    prog->fxVersion = 0; //vst_swap (getVersionNumber());
    prog->numParams = (numParams);

    getCurrPresetName().copyToBuffer (prog->prgName, sizeof (prog->prgName) - 1);

    for (int i = 0; i < numParams; ++i)
    {
        prog->params[i] = getParam(i);
    }
}

bool Vst2Plugin::saveToFXBFile(MemoryBlock& dest, bool isFXB, int maxSizeMB)
{
    const int numPrograms = getNumPresets();
    const int numParams = getNumParams();

    if (usesChunks())
    {
        if(isFXB)
        {
            MemoryBlock chunk;
            getChunkData (chunk, false, maxSizeMB);

            const int totalLen = sizeof (fxChunkSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            fxChunkSet* const set = (fxChunkSet*) dest.getData();

            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FBCh');
            set->version = (fxbVersionNum);
            set->fxID = 0; //vst_swap (getUID());
            set->fxVersion = 0;//vst_swap (getVersionNumber());
            set->numPrograms = (numPrograms);
            set->chunkSize = (chunk.getSize());

            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
        else
        {
            MemoryBlock chunk;
            getChunkData (chunk, true, maxSizeMB);

            const int totalLen = sizeof (fxProgramSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            fxProgramSet* const set = (fxProgramSet*) dest.getData();

            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FPCh');
            set->version = (fxbVersionNum);
            set->fxID = 0; //(getUID());
            set->fxVersion = 0; //(getVersionNumber());
            set->numPrograms = (numPrograms);
            set->chunkSize = (chunk.getSize());

            getCurrPresetName().copyToBuffer (set->name, sizeof (set->name) - 1);
            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
    }
    else
    {
        if (isFXB)
        {
            const int progLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);
            const int len = (sizeof (fxSet) - sizeof (fxProgram)) + progLen * jmax (1, numPrograms);

            dest.setSize (len, true);

            fxSet* const set = (fxSet*) dest.getData();

            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FxBk');
            set->version = (fxbVersionNum);
            set->fxID = 0; //(getUID());
            set->fxVersion = 0; //(getVersionNumber());
            set->numPrograms = (numPrograms);

            const int oldProgram = getCurrentPreset();

            MemoryBlock oldSettings;

            createTempParameterStore (oldSettings);
            setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + oldProgram * progLen));

            for (int i = 0; i < numPrograms; ++i)
            {
                if (i != oldProgram)
                {
                    setPreset(i);
                    setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + i * progLen));
                }
            }

            setPreset(oldProgram);

            restoreFromTempParameterStore (oldSettings);
        }
        else
        {
            const int totalLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);

            dest.setSize (totalLen, true);

            setParamsInProgramBlock ((fxProgram*) dest.getData());
        }
    }

    return true;
}

// Loads a .fxb file if it's for this effect

bool Vst2Plugin::loadBank(char *name)
{
/*
    try
    {
        CFxBank fx(name);                     // load the bank 
        if (!fx.isLoaded())                   // if error loading 
        {
            throw (int)1;
        }
    }
    catch(...)                              // if any error occured 
    {
        return false;                         // return NOT!!!
    }
*/
    return true;                            // pass back OKsa
}

// Returns the plug's directory (char* on pc, FSSpec on mac)

void * Vst2Plugin::onGetDirectory()
{
    return vstDir;
}

bool Vst2Plugin::onUpdateDisplay() 
{ 
    return false; 
}

void* Vst2Plugin::onOpenWindow(VstWindow* window) 
{ 
    return 0; 
}

bool Vst2Plugin::onCloseWindow(VstWindow* window) 
{ 
    return false; 
}

bool Vst2Plugin::onIOChanged() 
{ 
    return false; 
}

// VST 2.0, initial version:

void Vst2Plugin::aeffOpen()
{
    aeffDispatch(effOpen); 
}

void Vst2Plugin::aeffClose()
{
    if(aeff != NULL)  aeffDispatch(effClose); 
}

long Vst2Plugin::aeffDispatch(long opCode, long index, long value, void *ptr, float opt)
{
    return aeff->dispatcher(aeff, opCode, index, value, ptr, opt);;
}

void Vst2Plugin::aeffProcess(float **inputs, float **outputs, long sampleframes)
{
    aeff->process(aeff, inputs, outputs, sampleframes);
}

void Vst2Plugin::aeffProcessReplacing(float **inputs, float **outputs, long sampleframes)
{
    if (!(aeff->flags & effFlagsCanReplacing))
    {
        return;
    }

    aeff->processReplacing(aeff, inputs, outputs, sampleframes);
}

void Vst2Plugin::aeffProcessDoubleReplacing(double **inputs, double **outputs, long sampleFrames)
{
    if (!(aeff->flags & effFlagsCanDoubleReplacing))
    {
        return;
    }

#if defined(VST_2_4_EXTENSIONS)

    aeff->processDoubleReplacing(aeff, inputs, outputs, sampleFrames);

#endif
}

void Vst2Plugin::aeffSetParameter(long index, float parameter)
{
    aeff->setParameter(aeff, index, parameter);
}

float Vst2Plugin::aeffGetParameter(long index)
{
    return aeff->getParameter(aeff, index);
}

bool Vst2Plugin::aeffUsesChunks()
{
    return (aeff->flags & effFlagsProgramChunks) != 0;
}

void Vst2Plugin::aeffSetProgram(long lValue) 
{ 
    aeffBeginSetProgram(); 
    aeffDispatch(effSetProgram, 0, lValue); 
    aeffEndSetProgram(); 
}

long Vst2Plugin::getNumPresets()
{
    return aeff->numPrograms;
}

void Vst2Plugin::getProgramName(char *name)
{
    aeffDispatch(effGetProgramName, 0, 0, name);
}

long Vst2Plugin::getProgram()
{
    return aeffDispatch(effGetProgram);
}

void Vst2Plugin::setProgram(long index)
{
    aeffBeginSetProgram(); 
    aeffDispatch(effSetProgram, 0, index); 
    aeffEndSetProgram();

    //void * pChunk = NULL;
    //aeffGetChunk(&pChunk, true);
}

long Vst2Plugin::aeffGetProgram() 
{
    return aeffDispatch(effGetProgram); 
}

void Vst2Plugin::aeffSetProgramName(char *ptr) 
{ 
    aeffDispatch(effSetProgramName, 0, 0, ptr); 
}

void Vst2Plugin::aeffGetProgramName(char *ptr) 
{ 
    aeffDispatch(effGetProgramName, 0, 0, ptr); 
}

void Vst2Plugin::aeffGetParamLabel(long index, char *ptr) 
{ 
    aeffDispatch(effGetParamLabel, index, 0, ptr); 
}

void Vst2Plugin::aeffGetParamDisplay(long index, char *ptr) 
{ 
    aeffDispatch(effGetParamDisplay, index, 0, ptr); 
}

void Vst2Plugin::aeffGetParamName(long index, char *ptr) 
{ 
    aeffDispatch(effGetParamName, index, 0, ptr); 
}

void Vst2Plugin::aeffSetSampleRate(float fSampleRate) 
{ 
    aeffDispatch(effSetSampleRate, 0, 0, 0, fSampleRate); 
}

void Vst2Plugin::aeffSetBlockSize(long value) 
{ 
    aeffDispatch(effSetBlockSize, 0, value); 
}

void Vst2Plugin::aeffMainsChanged(bool bOn) 
{ 
    aeffDispatch(effMainsChanged, 0, bOn); 
}

void Vst2Plugin::aeffSuspend() 
{ 
    aeffDispatch(effMainsChanged, 0, false); 
}

void Vst2Plugin::aeffResume() 
{
    aeffDispatch(effMainsChanged, 0, true); 
}

long Vst2Plugin::aeffEditGetRect(ERect **ptr) 
{ 
    return aeffDispatch(effEditGetRect, 0, 0, ptr); 
}

long Vst2Plugin::aeffEditOpen(void *ptr) 
{ 
    long l = aeffDispatch(effEditOpen, 0, 0, ptr); 

    /* if (l > 0) */ 

    guiOpen = true; 

    return l; 
}

void Vst2Plugin::aeffEditClose() 
{
    if(aeff != NULL)
    {
        aeffDispatch(effEditClose); 
    }

    guiOpen = false; 
}

void Vst2Plugin::aeffEditIdle() 
{ 
    if (!guiOpen || inEditIdle)
    {
        return; 
    }

    inEditIdle = true; 

    aeffDispatch(effEditIdle); 

    inEditIdle = false; 
}

long Vst2Plugin::aeffGetChunk(void** ptr, bool isPreset)
{
    return aeffDispatch(effGetChunk, isPreset, 0, ptr);
}

long Vst2Plugin::aeffSetChunk(void *data, long byteSize, bool isPreset) 
{ 
    aeffBeginSetProgram(); 

    long lResult = aeffDispatch(effSetChunk, isPreset, byteSize, data); 

    aeffEndSetProgram(); 

    return lResult;
}

long Vst2Plugin::aeffProcessEvents(VstEvents* ptr) 
{ 
    return aeffDispatch(effProcessEvents, 0, 0, ptr); 
}

long Vst2Plugin::aeffCanBeAutomated(long index) 
{ 
    return aeffDispatch(effCanBeAutomated, index); 
}

long Vst2Plugin::aeffString2Parameter(long index, char *ptr) 
{ 
    return aeffDispatch(effString2Parameter, index, 0, ptr); 
}

long Vst2Plugin::aeffGetProgramNameIndexed(long category, long index, char* text) 
{ 
    return aeffDispatch(effGetProgramNameIndexed, index, category, text); 
}

long Vst2Plugin::aeffGetPlugCategory() 
{ 
    return aeffDispatch(effGetPlugCategory); 
}

long Vst2Plugin::aeffGetEffectName(char *ptr) 
{ 
    return aeffDispatch(effGetEffectName, 0, 0, ptr); 
}

long Vst2Plugin::aeffGetVendorString(char *ptr) 
{ 
    return aeffDispatch(effGetVendorString, 0, 0, ptr); 
}

long Vst2Plugin::aeffGetProductString(char *ptr) 
{ 
    return aeffDispatch(effGetProductString, 0, 0, ptr); 
}

long Vst2Plugin::aeffGetVendorVersion() 
{
    return aeffDispatch(effGetVendorVersion); 
}

long Vst2Plugin::aeffVendorSpecific(long index, long value, void *ptr, float opt) 
{ 
    return aeffDispatch(effVendorSpecific, index, value, ptr, opt); 
}

long Vst2Plugin::aeffCanDo(const char *ptr) 
{ 
    return aeffDispatch(effCanDo, 0, 0, (void *)ptr); 
}

long Vst2Plugin::aeffIdle() 
{ 
    if (needIdle) 
        return aeffDispatch(effIdle); 
    else 
        return 0; 
}

long Vst2Plugin::aeffGetParameterProperties(long index, VstParameterProperties* ptr) 
{ 
    return aeffDispatch(effGetParameterProperties, index, 0, ptr); 
}

long Vst2Plugin::aeffGetVstVersion() 
{ 
    return aeffDispatch(effGetVstVersion); 
}

// VST 2.1 extensions

long Vst2Plugin::aeffBeginSetProgram() 
{ 
    settingProgram = !!aeffDispatch(effBeginSetProgram); 

    return settingProgram; 
}

long Vst2Plugin::aeffEndSetProgram() 
{ 
    settingProgram = false; 

    return aeffDispatch(effEndSetProgram); 
}

// VST 2.3 Extensions

long Vst2Plugin::aeffStartProcess() 
{ 
    return aeffDispatch(effStartProcess); 
}

long Vst2Plugin::aeffStopProcess() 
{ 
    return aeffDispatch(effStopProcess); 
}

long Vst2Plugin::aeffSetPanLaw(long type, float val) 
{ 
    return aeffDispatch(effSetPanLaw, 0, type, 0, val); 
}

long Vst2Plugin::aeffBeginLoadBank(VstPatchChunkInfo* ptr) 
{ 
    return aeffDispatch(effBeginLoadBank, 0, 0, ptr); 
}

long Vst2Plugin::aeffBeginLoadProgram(VstPatchChunkInfo* ptr) 
{ 
    return aeffDispatch(effBeginLoadProgram, 0, 0, ptr); 
}

// VST 2.4 extensions

long Vst2Plugin::aeffGetNumMidiInputChannels() 
{ 
    return aeffDispatch(effGetNumMidiInputChannels, 0, 0, 0); 
}

long Vst2Plugin::aeffGetNumMidiOutputChannels() 
{ 
    return aeffDispatch(effGetNumMidiOutputChannels, 0, 0, 0); 
}


Vst2Host::Vst2Host(void* MainWindowHandle)
{
    sampleRate = 44100.;

    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = sampleRate;
    vstTimeInfo.nanoSeconds = 0.0;
    vstTimeInfo.ppqPos = 0.0;
    vstTimeInfo.tempo = MTransp->getBeatsPerMinute();
    vstTimeInfo.barStartPos = 0.0;
    vstTimeInfo.cycleStartPos = 0.0;
    vstTimeInfo.cycleEndPos = 0.0;
    vstTimeInfo.timeSigNumerator = 4;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.smpteOffset = 0;
    vstTimeInfo.smpteFrameRate = 1;
    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.flags = kVstTempoValid | kVstTimeSigValid;

    setBufferSize(MAudio->getBufferSize());
    setSampleRate(MAudio->getSampleRate());
    setBPM((float)MTransp->getBeatsPerMinute());

    ParentHWND = MainWindowHandle;

    vstMutex = CreateMutex(NULL, FALSE, NULL);

#ifdef USE_WIN32
#if 0
    WNDCLASS        vst_wc;

    vst_wc.style                = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    vst_wc.lpfnWndProc          = (WNDPROC) (VstGuiWindow::VstWndProc);
    vst_wc.cbClsExtra           = 0;
    vst_wc.cbWndExtra           = 0;
    vst_wc.hInstance            = NULL;
    vst_wc.hIcon                = NULL;
    vst_wc.hCursor              = NULL;
    vst_wc.hbrBackground        = NULL;
    vst_wc.lpszMenuName         = NULL;
    vst_wc.lpszClassName        = "_VSTEditor";

    if(!RegisterClass(&vst_wc))
    {
        MessageBox(0,"Failed To Register The Window Class, Damn It!", "Error", MB_OK|MB_ICONERROR);
    }
#endif
#endif
}

Vst2Host::~Vst2Host()
{
    removeAllModules();                    // remove all loaded effects 
}

void Vst2Host::setBPM(float fBPM)
{
    vstTimeInfo.tempo = fBPM;
}

void Vst2Host::addModule(Vst2Plugin* vst_module)
{
    plugins.push_back(vst_module);

    vst_module->setIndex(plugins.size() - 1);          // tell effect where it is 
}

Vst2Plugin* Vst2Host::loadModuleFromFile(std::string path)
{
    Vst2Plugin* plug = new Vst2Plugin(path);

    if(plug->isLoaded() == false)
    {
        // failed to load

        delete plug;

        plug = NULL;
    }
    else
    {
        plugins.push_back(plug);

        plug->setIndex(plugins.size() - 1);          // tell effect where it is 
    }

    if(plug == NULL)
    {
        MWindow->showAlertBox("Can't load plugin");
    }

    return plug;
}

bool Vst2Host::checkModule(char *path, bool *is_generator, char* name)
{
    bool ret_val = false;

    Vst2Plugin *plug = new Vst2Plugin(path);

    if(plug->isLoaded())
    {
        memset(name, 0, MAX_NAME_LENGTH * sizeof(char));

        plug->aeffGetProductString(name);

        if (name[0] == 0)
        {
            plug->aeffGetEffectName(name);
        }

        if (plug->aeff->numInputs != 0)
        {
            long catg = plug->aeffGetPlugCategory();

            if(catg == kPlugCategSynth || catg == kPlugCategGenerator)
            {
                *is_generator = true;
            }
            else
            {
                *is_generator = false;
            }
        }
        else
        {
            *is_generator = true;
        }

        ret_val = true;
    }

    removeModule(plug);

    return ret_val;
}

void Vst2Host::removeModule(Vst2Plugin *plug)
{
    int i = 0;

    for(Vst2Plugin* p : plugins)
    {
        if (p == plug)
        {
            plugins.erase(plugins.begin() + i);

            break;
        }

        i++;
    }
}

// Remove all of the loaded effects from memory

void Vst2Host::removeAllModules()
{
    for(Vst2Plugin* plug : plugins)
    {
        removeModule(plug);
    }
}

void Vst2Host::setBufferSize(int size)
{
    if (buffSize != size)
    {
        buffSize = size;

        for(Vst2Plugin* plug : plugins)
        {
            // Set new buffer size, then force resume

            plug->aeffSetBlockSize(buffSize);

            plug->aeffMainsChanged(true);
        }
    }
}

// Calculates time info from nanoSeconds 

void Vst2Host::calcTimeInfo(long lMask)
{
    // we don't care for the mask in here

    static double fSmpteDiv[] =
    {
        24.f,
        25.f,
        24.f,
        30.f,
        29.97f,
        30.f
    };

    double dPos = vstTimeInfo.samplePos / vstTimeInfo.sampleRate;

    vstTimeInfo.ppqPos = dPos * vstTimeInfo.tempo / 60.L;

    // offset in fractions of a second 

    double dOffsetInSecond = dPos - floor(dPos);

    vstTimeInfo.smpteOffset = (long)(dOffsetInSecond * fSmpteDiv[vstTimeInfo.smpteFrameRate] * 80.L);
}

void Vst2Host::setSampleRate(float fSampleRate)
{
    if (fSampleRate != this->sampleRate) 
    {
        sampleRate = fSampleRate;    // remember new sample rate

        vstTimeInfo.sampleRate = fSampleRate;

        // inform all loaded plugins 

        for(Vst2Plugin* plug : plugins)
        {
            plug->aeffSetSampleRate(fSampleRate);
        }
    }
}

// Callback to be called by plugins

long VSTCALLBACK Vst2Host::audioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
    return VstHost->onAudioMasterCallback(effect, opcode, index, value, ptr, opt);
}

// Redefine the callback a bit  

long Vst2Host::onAudioMasterCallback(AEffect *aeff, long opcode, long index, long value, void *ptr, float opt)
{
    Vst2Plugin *plug = NULL;

    if(aeff != NULL)
    {
        for(Vst2Plugin* pl : plugins)
        {
            if (pl->aeff == aeff) 
            {
                plug = pl;
                break;
            }
        }
    }

    if(plug == NULL)
    {
        return 2400L;   // Just always return host version, when loading new plugin
    }

    switch (opcode)
    {
        case audioMasterAutomate :
        {
            return plug->onSetParameterAutomated(index, opt);
        }
        case audioMasterVersion :
        {
            return 2400L;
        }
        case audioMasterCurrentId :
        {
            return 123;         // Why need provide unique id to plugin?
        }
        case audioMasterIdle :
        {
            for(Vst2Plugin* plug : plugins) plug->aeffEditIdle();

            return 0;
        }
        case audioMasterPinConnected :
        {
            return false;       // !((value) ? OnOutputConnected(effNum, index) : OnInputConnected(effNum, index)); (both methods were true)
        }
                                        /* VST 2.0 additions...              */
        case audioMasterWantMidi :
        {
            plug->wantsMidi = true;

            return true;
        }
        case audioMasterGetTime :
        {
            return (long)(&vstTimeInfo);
        }
        case audioMasterProcessEvents :
        {
            return false;       //OnProcessEvents(effNum, (VstEvents *)ptr);
        }
        case audioMasterSetTime :
        {
            return false;       //OnSetTime(effNum, value, (VstTimeInfo *)ptr);
        }
        case audioMasterTempoAt :
        {
            return 0;           //OnTempoAt(effNum, value);
        }
        case audioMasterGetNumAutomatableParameters :
        {
            return plug->getNumParams();
        }
        case audioMasterGetParameterQuantization :
        {
            return 1;
        }
        case audioMasterIOChanged :
        {
            plug->onIOChanged();
        }
        case audioMasterNeedIdle :
        {
            plug->needIdle = true;
            return true;
        }
        case audioMasterSizeWindow :
        {
            plug->onSizeEditorWindow(index, value);
            return true;
        }
        case audioMasterGetSampleRate :
        {
            plug->aeffSetSampleRate(sampleRate);
            return (long)sampleRate;
        }
        case audioMasterGetBlockSize :
        {
            plug->aeffSetBlockSize(buffSize);
            return buffSize;
        }
        case audioMasterGetInputLatency :
        {
            return 0;
        }
        case audioMasterGetOutputLatency :
        {
            return 0;
        }
        case audioMasterWillReplaceOrAccumulate :
        {
            return false;
        }
        case audioMasterGetCurrentProcessLevel :
        {
            return 0;
        }
        case audioMasterGetAutomationState :
        {
            return 0;
        }
        case audioMasterOfflineStart :
        {
            return false;       //OnOfflineStart(effNum, (VstAudioFile *)ptr, value, index); 
                                //OnOfflineStart(int nEffect, VstAudioFile* audioFiles, long numAudioFiles, long numNewAudioFiles)
        }
        break;
        case audioMasterOfflineRead :
        {
            return false;       //OnOfflineRead(effNum, (VstOfflineTask *)ptr, (VstOfflineOption)value, !!index); 
                                //virtual bool OnOfflineRead(int nEffect, VstOfflineTask* offline, VstOfflineOption option, bool readSource) { return false; }
        }
        break;
        case audioMasterOfflineWrite :
        {
            return false;       //OnOfflineWrite(effNum, (VstOfflineTask *)ptr, (VstOfflineOption)value);
                                //virtual bool OnOfflineWrite(int nEffect, VstOfflineTask* offline, VstOfflineOption option) { return false; }
        }
        break;
        case audioMasterOfflineGetCurrentPass :
        {
            return 0;
        }
        case audioMasterOfflineGetCurrentMetaPass :
        {
            return 0;
        }
        case audioMasterSetOutputSampleRate :
        {
                                //OnSetOutputSampleRate(effNum, opt); (was empty)
            return 1;
        }
#ifdef VST_2_4_EXTENSIONS
        case audioMasterGetOutputSpeakerArrangement :
#else
        case audioMasterGetSpeakerArrangement :
#endif
        {
            return false;       // OnGetOutputSpeakerArrangement(effNum, (VstSpeakerArrangement *)value, (VstSpeakerArrangement *)ptr);
                                // virtual bool OnGetOutputSpeakerArrangement(int nEffect, VstSpeakerArrangement* pluginInput, VstSpeakerArrangement* pluginOutput) { return false; }
        }
        case audioMasterGetVendorString :
        {
            strcpy((char*)ptr, "DAW36"); 
            return true; 
        }
        case audioMasterGetProductString :
        {
            strcpy((char*)ptr, "DAW36"); 
            return true; 
        }
        case audioMasterGetVendorVersion :
        {
            return 1;
        }
        case audioMasterVendorSpecific :
        {
            return 0;           //OnHostVendorSpecific(effNum, index, value, ptr, opt);
                                //virtual long OnHostVendorSpecific(int nEffect, long lArg1, long lArg2, void* ptrArg, float floatArg) { return 0; }
        }

        case audioMasterSetIcon :

            // undefined in VST 2.0 specification

            break;

        case audioMasterCanDo :
        {
            return onCanDo((const char *)ptr);
        }
        case audioMasterGetLanguage :
        {
            return 0;
        }
        case audioMasterOpenWindow :
        {
            plug->onOpenWindow((VstWindow*)ptr);
            return 0;
        }
        case audioMasterCloseWindow :
        {
            return plug->onCloseWindow((VstWindow*)ptr);
        }
        case audioMasterGetDirectory :
        {
            return (long)plug->onGetDirectory();
        }
        case audioMasterUpdateDisplay :
        {
            return plug->onUpdateDisplay();
        }
                                            /* VST 2.1 additions...              */
#ifdef VST_2_1_EXTENSIONS

        case audioMasterBeginEdit :
        {
            return false;
        }
        case audioMasterEndEdit :
        {
            return false;
        }
        case audioMasterOpenFileSelector :
        {
            return false;       //OnOpenFileSelector(effNum, (VstFileSelect *)ptr);
        }
#endif
                                            /* VST 2.2 additions...              */
#ifdef VST_2_2_EXTENSIONS

        case audioMasterCloseFileSelector :
        {
            return false;       //OnCloseFileSelector(effNum, (VstFileSelect *)ptr);
        }
        case audioMasterEditFile :
        {
            return false; 
        }
        case audioMasterGetChunkFile :
        {
            return false;       //OnGetChunkFile(effNum, ptr);
                                // virtual bool OnGetChunkFile(int nEffect, void * nativePath) { return false; }
        }
#endif

#ifdef VST_2_3_EXTENSIONS
        case audioMasterGetInputSpeakerArrangement :
        {
            return 0;
                                // virtual VstSpeakerArrangement *OnGetInputSpeakerArrangement(int nEffect) { return 0; }
        }
#endif

    }

    return 0L;
}

bool Vst2Host::onCanDo(const char *ptr)
{
    if ((!strcmp(ptr, "supplyIdle")) ||
        (!strcmp(ptr, "sendVstEvents")) ||
        (!strcmp(ptr, "sendVstMidiEvent")) ||
        (!strcmp(ptr, "receiveVstEvents")) ||
        (!strcmp(ptr, "receiveVstMidiEvent")) ||
        (!strcmp(ptr, "sizeWindow")) ||
        (!strcmp(ptr, "sendVstMidiEventFlagIsRealtime")) )
    {
        return true;
    }

    return false;
}

bool Vst2Host::onGetVendorString(char *text) 
{ 
    strcpy(text, "ThirtySix"); return true; 
}

long Vst2Host::onGetHostVendorVersion() 
{ 
    return 1; 
}

bool Vst2Host::onGetProductString(char *text) 
{ 
    strcpy(text, "Studio36"); return true; 
}


// When building with JUCE, we use juce's windows



