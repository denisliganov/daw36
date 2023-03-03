//
// Revision History:
//
// Author               Date                        Major Changes
//
// Alex V               07/23/2008                  Initial version
// Denis L              11/15/2020                  Reworked version
//

#include "36_vst.h"
#include "36_utils.h"
#include "36_config.h"
#include "36_effects.h"
#include "36_audio_dev.h"
#include "36_transport.h"
#include "36_brwentry.h"
#include "36_paramnum.h"
#include "36.h"
#include "36_vstwin.h"
#include "36_window.h"


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


Vst2Plugin::Vst2Plugin(const char* path, Vst2Host* host, void* ParentWindow)
{
    ERect *pRC = NULL;
    aeff = NULL;

    vsthost = host;
    vstpath = NULL;
    hmodule = NULL;
    vstdir = NULL;
    vstGuiWin = NULL;
    inbuffs = NULL;
    outbuffs = NULL;
    guiopen = false;
    needidle = false;
    ineditidle = false;
    wantsmidi = false;
    settingprogram = false;
    vstindex = -1;
    isreplacing = true;
    hasgui = false;
    generator = false;
    parendwindow = ParentWindow;
    vstParamWin = NULL;
    numins = numouts = 0;

    if (!loadFromDll(path))                      // try to load the thing
    {
        return;                                 // and regretfully return error
    }

    aeffEditGetRect(&pRC);

    if (pRC)
    {
        hasgui = true;
    }

    useschunks = aeffUsesChunks();

    int i, j;  

    if (aeff->numInputs)                // allocate input pointers
    {
        // We need to allocate input and output buffer arrays on plugin initialization

        inbuffs = new float *[aeff->numInputs];

        for (i = 0; i < aeff->numInputs; ++i)
        {
            inbuffs[i] = new float[24000];

            for (j = 0; j < 24000; ++j)
            {
                inbuffs[i][j] = 0;
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

    numins = aeff->numInputs;

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

        outbuffs = new float *[nAlloc];      // and the buffers pointed to

        for (i = 0; i < aeff->numOutputs; i++)
        {
            // for this sample project, I've assumed a maximum buffer size
            // of 1/4 second at 96KHz - presumably, this is MUCH too much
            // and should be tweaked to more reasonable values, since it
            // requires a machine with lots of main memory this way...
            // user configurability would be nice, of course.

            outbuffs[i] = new float[24000];

            for (j = 0; j < 24000; j++)
            {
                outbuffs[i][j] = 0.f;
            }
        }

        // if less than 2, fill up by replicating buffer 0 pointer

        for (; i < nAlloc; ++i)
        {
            outbuffs[i] = outbuffs[0];
        }

        numouts = nAlloc;
    }


    // Init sequence

    aeffOpen();                            // open the effect                   
    aeffSetSampleRate(host->fSampleRate);        // adjust its sample rate            

    // this is a safety measure against some plugins that only set their buffers
    // ONCE - this should ensure that they allocate a buffer that's large enough

    aeffSetBlockSize(11025);

    // deal with changed behaviour in V2.4 plugins that don't call wantEvents()

    wantsmidi = (aeffCanDo("receiveVstMidiEvent") == 1);

    aeffResume();                            // then force resume
    aeffSuspend();                           // suspend again...
    aeffSetBlockSize(host->getBuffSize());   // and buffer size
    aeffResume();                            // then force resume

    vstMutex = host->vstMutex;

    char tmpname[30] = {};

    getDisplayName(tmpname, 30);

    objName = tmpname;
}

Vst2Plugin::~Vst2Plugin()
{
    WaitForSingleObject(vstMutex,INFINITE);

    int i;

    if (inbuffs != NULL)
    {
        for (i = 0; i < numins; ++i)
        {
            if(inbuffs[i] != NULL)
            {
                delete((void*)inbuffs[i]);

                inbuffs[i] = NULL;
            }
        }

        delete((void *) inbuffs);

        inbuffs = NULL;
    }

    for (i = 0; i < numouts; ++i)
    {
        if (outbuffs[i] != NULL)
        {
            delete((void*)outbuffs[i]);

            outbuffs[i] = NULL;
        }
    }

    for (; i < numouts; ++i)  outbuffs[i] = NULL;

    delete((void *) outbuffs);

    if(aeff != NULL)
    {
        // aeffClose(); 

        // if DLL instance available
        if (hmodule)
        {
            // remove it

            FreeLibrary(hmodule);

            hmodule = NULL; 
        }

        if (vstdir)
        {
            delete[] vstdir;

            vstdir = NULL;
        }

        if (vstpath) 
        {
            delete[] vstpath;

            vstpath = NULL;
        }
    }

    ReleaseMutex(vstMutex);
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
            aeff = vstMainFunc(vsthost->audioMasterCallback);
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

        vstpath = new char[len + 1];

        if (vstpath)
        {
            strcpy(vstpath, path);
        }

        const char *p = strrchr(path, '\\');

        if (p)
        {
            vstdir = new char[p - path + 1];

            if (vstdir)
            {
                memcpy(vstdir, path, p - path);
                vstdir[p - path] = '\0';
            }
        }
    }

    return !!aeff;
}

long Vst2Plugin::vstDispatch(const int opcode,const int index,const int value,void * const ptr,float opt)
{
    return aeffDispatch(opcode, index, value, ptr, opt);
}

bool Vst2Plugin::isLoaded()
{
    return aeff != NULL;
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
    int i,j;

    WaitForSingleObject(vstMutex,INFINITE);

    //If effect has only one input then mix both L and R to one mono channel

    if (in_buff != NULL)
    {
        if (numins == 1)
        {
            for (j = 0; j < buff_size; ++j)
            {
                inbuffs[0][j] = (in_buff[j*2] + in_buff[j*2+1])/2;
            }
        }
        // if effect has more than one input
        else if (numins > 1)
        {
            for (j = 0; j < buff_size; ++j)
            {
                //fill in only first two channels

                inbuffs[0][j] = in_buff[j*2];
                inbuffs[1][j] = in_buff[j*2+1];
            }

            //all extra inputs will be duplicated to firts input (we don't support them)

            for (i = 2; i < numins; ++i)
            {
                inbuffs[i] = inbuffs[0];
            }
        }
    }

    //sanity action: just clear output buffer

    memset(out_buff, 0, (sizeof(float)*2)*buff_size);

    //Actually, plugins can support two data processing methods:
    // First is Process - out += process(in)
    // Second is ProcessReplacing - out = process(in)

    if (aeff->flags & effFlagsCanDoubleReplacing && isreplacing)
    {
        /* pEffect->EffProcessDoubleReplacing(
            (double *)inBufs,
            (double *)outBufs,
            buff_size);
        */
    }
    else if (aeff->flags & effFlagsCanReplacing && isreplacing)
    {
        aeffProcessReplacing(inbuffs, outbuffs, buff_size);
    }
    else
    {
        aeffProcess(inbuffs, outbuffs, buff_size);
    }

    for (i = 0; i < buff_size; ++i)
    {
        //lets duplicate output if effect has only one

        if (numouts == 1)
        {
            out_buff[i*2] = out_buff[i*2+1] = outbuffs[0][i];
        }
        else if (numouts > 1)//effect has more than one buffer
        {
            //use only first two channels and ignore all other if any

            out_buff[i*2] = outbuffs[0][i];
            out_buff[i*2+1] = outbuffs[1][i];
        }
    }

    ReleaseMutex(vstMutex);
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
        size_t      i = strlen(vstpath);
        unsigned    j = 0;
        bool        dots = false;

        // Go backward till find a dot in file name

        while (vstpath[i--] != '.');

        // Go backward until reach the first "\\"

        while (vstpath[i--] != '\\')
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
            name[j++] = vstpath[i++];
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
    BrwEntry*       preset                 = NULL;
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
            preset = new BrwEntry((Device36*)this);
            preset->prindex = idx;
            preset->setObjName(bzName);

            if(idx == currentProgram)
            {
                currPreset = preset;
            }

            presets.push_back(preset);

            pres.push_back(preset->getObjName());
        }
    }
}

void Vst2Plugin::extractParams()
{
    // Get all parameters and add them to list of parameters for the effect

    int                     index      = 0;
    int                     NumParam   = getNumParams();
    char                   *paramName  = NULL;
    char                   *dispVal    = NULL;
    char                   *paramLabel = NULL;
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
        getDisplayValue(index, &dispVal);
        getParamLabel(index, &paramLabel);

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

            char label[MAX_NAME_LENGTH] = {};
            strncpy(label, dispVal, min(MAX_NAME_LENGTH - strlen(paramLabel), strlen(dispVal)));
            strcat(label, paramLabel);

            param->setValString(label);

            param->setIndex(index);      // for VST purpose

            addParam(param);

            if (NULL != paramName)
            {
                free(paramName);
                paramName = NULL;
            }

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
    }
}

void Vst2Plugin::handleParamUpdate(Param* param)
{
    Parameter* prm = dynamic_cast<Parameter*>(param);

    if (prm != NULL && getParamLock() == false)
    {
        setParam(prm->getIndex(), prm->getValue());

        // Update units string for the parameter
        {
            char      *dispVal    = NULL;
            char      *paramLabel = NULL;

            getDisplayValue(prm->getIndex(),&dispVal);
            getParamLabel(prm->getIndex(), &paramLabel);

            char label[MAX_NAME_LENGTH] = {};
            strncpy(label, dispVal, min(MAX_NAME_LENGTH-strlen(paramLabel),strlen(dispVal)));
            strcat(label, paramLabel);

            prm->setValString(label);

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
    }
}

bool Vst2Plugin::onSetParameterAutomated(long index,float value)
{
    for(Param* p : params)
    {
        Parameter* param = dynamic_cast<Parameter*>(p);

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

void Vst2Plugin::updParamsFromPlugin()
{
    setParamLock(true);

    for(Param* p : params)
    {
        Parameter* param = dynamic_cast<Parameter*>(p);

        float fVal = getParam(param->getIndex());

        if(fVal < 0)
        {
            fVal = 0;
        }
        else if(fVal > 1)
        {
            fVal = 1;
        }

        param->setValue(fVal);
        //param->setInitialValue(fVal);
    }

    setParamLock(false);
}

bool Vst2Plugin::setPresetByName(std::string name)
{
    if (name != "")
    {
        for(BrwEntry* pe : presets)
        {
            if(pe->getObjName() == name)
            {
                setProgram(pe->prindex);
                currPreset = pe;

                updParamsFromPlugin();

                return true;
            }
        }
    }

    return false;
}

bool Vst2Plugin::setPresetByIndex(long index)
{
    if (index >=0 && index <= (long)presets.size())
    {
        setProgram(index);

        for(BrwEntry* pe : presets)
        {
            if(pe->prindex == index)
            {
                currPreset = pe;
                updParamsFromPlugin();

                return true;
            }
        }
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

void Vst2Plugin::save(XmlElement * xmlEff)
{
    XmlElement* state = new XmlElement("STATE");
    MemoryBlock m;
    getStateInformation(m);
    state->addTextElement(m.toBase64Encoding());
    xmlEff->addChildElement(state);
}

void Vst2Plugin::load(XmlElement * xmlEff)
{
    const XmlElement* const state = xmlEff->getChildByName(T("STATE"));

    if(state != 0)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());
        setStateInformation (m.getData(), m.getSize());
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

            Param* p = getParamByIndex(i);
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
                        setPresetByIndex(i);
                    }

                    if(! restoreProgramSettings (prog))
                    {
                        return false;
                    }
                }
            }

            if(set->numPrograms > 0)  setPresetByIndex(oldProg);

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

            Param* extparam =  getParamByIndex(i);

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
                    setPresetByIndex(i);
                    setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + i * progLen));
                }
            }

            setPresetByIndex(oldProgram);

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
    return vstdir;
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

    if (vstGuiWin != NULL)
    {
 //       vstGuiWin->updateTitle();
    }
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

    guiopen = true; 

    return l; 
}

void Vst2Plugin::aeffEditClose() 
{
    if(aeff != NULL)  aeffDispatch(effEditClose); guiopen = false; 
}

void Vst2Plugin::aeffEditIdle() 
{ 
    if (!guiopen || ineditidle) return; 

    ineditidle = true; 

    aeffDispatch(effEditIdle); 

    ineditidle = false; 
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
    if (needidle) 
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
    settingprogram = !!aeffDispatch(effBeginSetProgram); 

    return settingprogram; 
}

long Vst2Plugin::aeffEndSetProgram() 
{ 
    settingprogram = false; 

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
    fSampleRate = 44100.;

    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = fSampleRate;
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

Vst2Plugin* Vst2Host::loadModuleFromFile(char* path)
{
    char szFile[MAX_PATH_LENGTH];       // buffer for file name

    if (path == NULL)
    {
        String strPlugDir(WorkDirectory);

        strPlugDir += LOCAL_PLUGIN_FOLDER;

        juce::File defDir(strPlugDir);

        //Let's show a dialog and allow user to choose a directory
        FileChooser *fileSelector = new FileChooser(T("Open a file"), defDir, T("*.dll;*.DLL"), true);

        if (fileSelector->browseForFileToOpen() == true)
        {
            juce::File vstFile = fileSelector->getResult();

            vstFile.getFullPathName().copyToBuffer(szFile, MAX_NAME_LENGTH - 1);

            delete fileSelector;
        }
    }
    else
    {
        strcpy(szFile, path);
    }

    Vst2Plugin* plug = new Vst2Plugin(path, this, ParentHWND);

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

    Vst2Plugin *plug = new Vst2Plugin(path, this, ParentHWND);

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
    if (plug->vstGuiWin != NULL)
    {
    //    plug->closeGui();

        // todo: possibly unstable, test with different vsts

    //    plug->aeffClose();
    }

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
            // set new buffer size, then force resume

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
    if (fSampleRate != this->fSampleRate) 
    {
        this->fSampleRate = fSampleRate;    // remember new sample rate
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

    if(plug == NULL) return 2400L;   // Just always return host version, when loading new plugin

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
            plug->wantsmidi = true;

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
            plug->needidle = true;
            return true;
        }
        case audioMasterSizeWindow :
        {
            plug->onSizeEditorWindow(index, value);
            return true;
        }
        case audioMasterGetSampleRate :
        {
            plug->aeffSetSampleRate(fSampleRate);
            return (long)fSampleRate;
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



