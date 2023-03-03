
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
friend  VstInstr;
friend  VstEffect;

public:

            SubWindow*              vstGuiWin;
            JuceVstParamWindow*     vstParamWin;

            AEffect                *aeff;               // Steinberg base struct
            Vst2Host               *vsthost;
            long                    vstindex;           // Index in VSTHost plugin array
            char                   *vstpath;
            char                   *vstdir;
            HANDLE                  vstMutex;
            HMODULE                 hmodule;
            bool                    generator;
            int                     numins;
            int                     numouts;
            bool                    guiopen;
            bool                    needidle;
            bool                    ineditidle;
            bool                    wantsmidi;
            bool                    settingprogram;

private:

            void*                   parendwindow;
            float                 **inbuffs;
            float                 **outbuffs;
            bool                    isreplacing;
            bool                    hasgui;
            bool                    useschunks;

public:

            Vst2Plugin(const char* path, Vst2Host* vst_host, void* parent_window);
            ~Vst2Plugin();
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
            void                    setReplacing(bool fReplace) { isreplacing = fReplace; };
            bool                    hasGui() { return hasgui; };
            bool                    usesChunks() { return useschunks; };
            void                    getDisplayName(char *name, unsigned int length);
            long                    getNumPresets();
            void                    getProgramName(char *name);
            long                    getProgram();
            void                    setProgram(long index);
            bool                    isLoaded();
            void                    idle();
            void                    editIdle();

            void                    setSampleRate(float smpRate);
            void                    setBufferSize(unsigned int buffSize);
            void                    reset();
            void                    setPresetName(char* name);
            void                    updatePresets();
            void                    extractParams();
            void                    updateVString(Param* param);
            void                    handleParamUpdate(Param* param);
            bool                    onSetParameterAutomated(long index,float value);
            void                    updParamsFromPlugin();
            bool                    setPresetByName(std::string pname);
            bool                    setPresetByIndex(long index);
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
            void                    setIndex(int nNewIndex) { vstindex = nNewIndex; }
            long                    getIndex() { return vstindex; }
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
      Vst2Plugin*                   loadModuleFromFile(char* path);
            void                    removeAllModules();
            void                    removeModule(Vst2Plugin *plug);
            bool                    checkModule(char *path, bool *is_generator, char* name);
            void                    setSampleRate(float fSampleRate);
            void                    setBufferSize(int size);
            void                    setBPM(float bpm);
            long                    onAudioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
    virtual bool                    onCanDo(const char *ptr);
    virtual bool                    onGetVendorString(char *text);
    virtual long                    onGetHostVendorVersion();
    virtual bool                    onGetProductString(char *text);
            int                     getBuffSize() { return buffSize; };

protected:

    static long VSTCALLBACK audioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);

            std::vector<Vst2Plugin*> plugins;

            void*                   ParentHWND;
            HANDLE                  vstMutex;
            VstTimeInfo             vstTimeInfo;
            float                   fSampleRate;
            int                     buffSize;

            void                    calcTimeInfo(long mask = -1);
};


