
#pragma once

//
// VST 2.X plugin wrapper (Vst2Plugin) & host wrapper (Vst2Host)
// Only the most useful stuff is supported
//
// Author               Date          Major Changes
// ----------------------   ------------      -------------
// Holy Spirit          07/23/2008      Initial version
// DenisL               11/15/2020      Reworked version
//




#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"


#include "audioeffectx.h"     // Steinberg VST header

#include "juce_amalgamated.h"

#include <vector>


#define VST_MAX_NAME_LENGTH         (100)
#define VST_MAX_PARAM_LABEL_LENGTH  (30)
#define VST_MAX_PARAM_VALUE_LENGTH  (60)


struct fxProgram;
struct fxSet;
struct fxSet;
struct fxChunkSet;
struct fxProgramSet;



extern Vst2Host*        VstHost;







class Vst2Plugin : public Device36
{
friend  Vst2Plugin;
friend  Vst2Host;

//  Former "external" part of VST module

public:
            void                    addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
            void                    checkBounds(Note* gnote, Trigger* tg, long num_frames);
    virtual SubWindow*              createWindow();
            Vst2Plugin*             clone();
            void                    deactivateTrigger(Trigger* tg);
            void                    fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
            void                    generateData(float* in_buff, float* out_buff, long num_frames = 0, long mix_buff_frame = 0);
            bool                    isLoaded() { return aeff != NULL; }
            long                    handleTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
            void                    postNoteON(int note, float vol);
            void                    postNoteOFF(int note, int velocity);
            void                    postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
            void                    topAllNotes();
            void                    stopAllNotes();
            void                    vstProcess(float* in_buff, long num_frames, long buff_frame);

//  Former "internal" part

private:
            AEffect                *aeff;               // Steinberg base struct
            SubWindow*              vstGuiWin;
            bool                    needIdle;
            bool                    wantsMidi;

            long                    vstIndex;           // Index in VSTHost plugin array
            char*                   vstPath;
            char*                   vstDir;
            HANDLE                  vstMutex;
            HMODULE                 hmodule;
            bool                    generator;
            int                     numIns;
            int                     numOuts;
            bool                    guiOpen;
            bool                    inEditIdle;
            bool                    settingProgram;

            float                 **inBuffs;
            float                 **outBuffs;
            bool                    isReplacing;
            bool                    _hasGui;
            bool                    _usesChunks;

            VstMidiEvent            midiEvents[800];
            long                    numEvents;

public:

            Vst2Plugin(std::string path);
   virtual ~Vst2Plugin();
            void                    loadAndInit(const char *path);
            bool                    loadFromDll(const char *nm) throw(...);
            long                    vstDispatch(const int opcode, const int index, const int value, void* const ptr, float opt);
            void                    processDSP(float* in_buff, float* out_buff, int num_frames);
            void                    processEvents(VstEvents *events);
            void                    setParam(long index, float Value);
            float                   getParam(long index);
            unsigned                getNumParams();
            void                    getDisplayValue(long index, char** disp_val);
            void                    getParamName(long index, char** ppName);
            void                    getParamLabel(long index, char **ppLabel);
            void                    setReplacing(bool fReplace) { isReplacing = fReplace; };
            bool                    hasGui() { return _hasGui; };
            bool                    usesChunks() { return _usesChunks; };
            void                    getDisplayName(char *name, unsigned int length);
            long                    getNumPresets();
            void                    getProgramName(char *name);
            long                    getProgram();
            void                    setProgram(long index);
            void                    idle();
            void                    editIdle();

            void                    setSampleRate(float smpRate);
            void                    setBufferSize(unsigned int buffSize);
            void                    reset();
            void                    setPresetName(char* name);
            void                    updatePresets();
            void                    extractParams();
            void                    updateVString(Parameter* param);
            void                    handleParamUpdate(Parameter* param);
            bool                    onSetParameterAutomated(long index,float value);
            void                    syncParamValues();
            void                    updParamValString(Parameter* param);

            bool                    setPreset(std::string pname);
            bool                    setPreset(long index);


            long                    getCurrentPreset();
            void                    save(XmlElement* xmlEff);
            void                    load(XmlElement* xmlEff);
            const String            getCurrPresetName();
            void                    setStateInformation (const void* data, int sizeInBytes);
            void                    getStateInformation (MemoryBlock& destData);

//// Stolen from JUCE:
            void                    setChunkData(const char* data, int size, bool isPreset);
            void                    getChunkData(MemoryBlock& mb, bool isPreset, int maxSizeMB);
            bool                    saveToFXBFile(MemoryBlock& dest, bool isFXB, int maxSizeMB);
            bool                    loadFromFXBFile(const void* const data, const int dataSize);
            void                    setParamsInProgramBlock (fxProgram* const prog);
            void                    restoreFromTempParameterStore(const MemoryBlock& m);
            void                    createTempParameterStore(MemoryBlock& dest);
            bool                    restoreProgramSettings(const fxProgram* const prog);
////
            void                    setIndex(int nNewIndex) { vstIndex = nNewIndex; }
            long                    getIndex() { return vstIndex; }
            bool                    loadBank(char *objName);
            void*                   onGetDirectory();
            void                    onSizeEditorWindow(long width, long height) {}
            bool                    onUpdateDisplay();
            void*                   onOpenWindow(VstWindow* window);
            bool                    onCloseWindow(VstWindow* window);
            bool                    onIOChanged();

            void                    aeffOpen();
            void                    aeffClose();
            long                    aeffDispatch(long opCode, long index=0, long value=0, void *ptr=0, float opt=0.);
            void                    aeffProcess(float **inputs, float **outputs, long sampleframes);
            void                    aeffProcessReplacing(float **inputs, float **outputs, long sampleframes);
            void                    aeffProcessDoubleReplacing(double** inputs, double** outputs, long sampleFrames);
            void                    aeffSetParameter(long index, float parameter);
            float                   aeffGetParameter(long index);
            bool                    aeffUsesChunks();
            void                    aeffSetProgram(long lValue);
            long                    aeffGetProgram();
            void                    aeffSetProgramName(char *ptr);
            void                    aeffGetProgramName(char *ptr);
            void                    aeffGetParamLabel(long index, char *ptr);
            void                    aeffGetParamDisplay(long index, char *ptr);
            void                    aeffGetParamName(long index, char *ptr);
            void                    aeffSetSampleRate(float fSampleRate);
            void                    aeffSetBlockSize(long value);
            void                    aeffMainsChanged(bool bOn);
            void                    aeffSuspend();
            void                    aeffResume();
            long                    aeffEditGetRect(ERect **ptr);
            long                    aeffEditOpen(void *ptr);
            void                    aeffEditClose();
            void                    aeffEditIdle();
            long                    aeffGetChunk(void **ptr, bool isPreset = false);
            long                    aeffSetChunk(void *data, long byteSize, bool isPreset = false);
// VST 2.0
            long                    aeffProcessEvents(VstEvents* ptr);
            long                    aeffCanBeAutomated(long index);
            long                    aeffString2Parameter(long index, char *ptr);
            long                    aeffGetProgramNameIndexed(long category, long index, char* text);
            long                    aeffGetPlugCategory();
            long                    aeffGetEffectName(char *ptr);
            long                    aeffGetVendorString(char *ptr);
            long                    aeffGetProductString(char *ptr);
            long                    aeffGetVendorVersion();
            long                    aeffVendorSpecific(long index, long value, void *ptr, float opt);
            long                    aeffCanDo(const char *ptr);
            long                    aeffIdle();
            long                    aeffGetParameterProperties(long index, VstParameterProperties* ptr);
            long                    aeffGetVstVersion();
// VST 2.1 extensions
            long                    aeffBeginSetProgram();
            long                    aeffEndSetProgram();
// VST 2.3 Extensions
            long                    aeffStartProcess();
            long                    aeffStopProcess();
            long                    aeffSetPanLaw(long type, float val);
            long                    aeffBeginLoadBank(VstPatchChunkInfo* ptr);
            long                    aeffBeginLoadProgram(VstPatchChunkInfo* ptr);
// VST 2.4 extensions
            long                    aeffGetNumMidiInputChannels();
            long                    aeffGetNumMidiOutputChannels();
};


class Vst2Host
{
friend class Vst2Plugin;

public:
            Vst2Host(void* main_window);
           ~Vst2Host();
            void                    addModule(Vst2Plugin* vst_module);
      Vst2Plugin*                   loadModuleFromFile(std::string path);
            void                    removeAllModules();
            void                    removeModule(Vst2Plugin *plug);
            bool                    checkModule(char *path, bool *is_generator, char* name);
            void                    setSampleRate(float sample_rate);
            float                   getSampleRate()     { return sampleRate; }
            void                    setBufferSize(int size);
            void                    setBPM(float bpm);
            long                    onAudioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
    virtual bool                    onCanDo(const char *ptr);
    virtual bool                    onGetVendorString(char *text);
    virtual long                    onGetHostVendorVersion();
    virtual bool                    onGetProductString(char *text);
            int                     getBuffSize() { return buffSize; };

protected:

     static long VSTCALLBACK        audioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);

            std::vector<Vst2Plugin*> plugins;

            void*                   ParentHWND;
            HANDLE                  vstMutex;
            VstTimeInfo             vstTimeInfo;
            float                   sampleRate;
            int                     buffSize;

            void                    calcTimeInfo(long mask = -1);
};


