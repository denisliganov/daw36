

#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"
#include "36_paramobject.h"
#include "36_config.h"






typedef enum InstrType
{
    Instr_Sample,
    Instr_Generator,
    Instr_Synth,
    Instr_SoundFont,
    Instr_VstPlugin
}InstrType;



class Device36 : public ParamObject
{
friend InstrPanel;
friend Instrument;

public:
            Device36();
    virtual ~Device36();

    virtual SubWindow*          createWindow();
    virtual void                createSelfPattern();
    virtual void                forceStop();
            int                 getIndex() { return devIdx; }
        std::list<BrwEntry*>    getPresets() { return presets; }
            BrwEntry*           getCurrPreset() { return currPreset; }
            BrwEntry*           getPresetByName(std::string pr_name);
            BrwEntry*           getPresetByIndex(long devIdx);
            float               getLastNoteLength() { return lastNoteLength; }
            int                 getMuteCount()  { return muteCount; }
            bool                getBypass()     { return bypass; }
            Gobj*               getContainer() { return container; }

            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
            long                getPresetIndex(char* objName);
    std::vector<std::string>&   getPresetList()  { return pres; }
            void                removeElements();
            void                handleWindowClosed();
            bool                isWindowVisible();
            bool                isPreviewOnly() { return previewOnly; }
            bool                isInternal()    { return internal; }

    virtual void                reset() { }
            void                savePreset();
            void                savePresetAs(char* preset_name);
            void                saveStateData(XmlElement& xmlParentNode, char* preset_name = NULL, bool global = false);
            void                saveCustomStateData(XmlElement& xmlParentNode) {};
            void                setIndex(int idx) { devIdx = idx; }
    virtual void                scanForPresets();
    virtual void                setBPM(float bpm) {};
    virtual void                setBufferSize(unsigned bufferSize) {};
    virtual void                setSampleRate(float sampleRate) {};
    virtual void                showWindow(bool show);
    virtual bool                setPresetByName(BrwEntry* preset);
    virtual bool                setPresetByIndex(long index) { return true; };
    virtual bool                setPresetByName(std::string pname);
            void                setContainer(Gobj* cnt) { container = cnt; }

    virtual void                activateTrigger(Trigger* tg);
    virtual void                deactivateTrigger(Trigger* tg);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto);
    virtual void                deClick(Trigger* tg, long num_frames, long buff_frame = 0);
    virtual void                generateData(float* in_buff, float* out_buff, long num_frames, long mix_buff_frame = 0);
            void                fillOutputBuffer(float* out_buff, long num_frames, long buff_frame, long mix_buff_frame);
    virtual void                preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buff_frame = 0);
    virtual long                handleTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0) {return 0;};
    virtual void                postProcessTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
    virtual long                processTrigger(Trigger* tg, long num_frames, long remaining, long buff_frame);
    virtual void                processDSP(float* in_buff, float* out_buff, int num_frames);

            void                addNote(Note* note);
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);

            std::list<Trigger*> activeTriggers;
            std::list<Note*>    notes;
            float               cfsV;
            long                endFrame;  // last frame to fill
            Envelope*           envVol;

            float               volBase;
            float               pan0, pan1, pan2, pan3;
            int                 rampCount;
            float               rampCounterV;

            float               tempBuff[MAX_BUFF_SIZE*2];     // Data for single trigger
            float               outBuff[MAX_BUFF_SIZE*2];      // Date for whole session

            ParamVol*           vol;
            ParamPan*           pan;
            ParamToggle*        enabled;

            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;

            int                 muteCount;

            bool                muteparam;
            bool                soloparam;
            int                 devIdx;
            SubWindow*          guiWindow;

            Pattern*            selfPattern;
            Note*               selfNote;

protected:

            InstrType           type;
            Gobj*               container;

            bool                bypass;

            BrwEntry*           currPreset;
            std::string         currPresetName;
            std::string         filePath;

            bool                internal;
            bool                isLoading;
            int                 lastParamIdx;

            std::string         presetPath;
    std::list<BrwEntry*>        presets;
    std::vector<std::string>    pres;
            bool                previewOnly;
            long                uniqueId;

            void                deletePresets();
            void                restoreStateData(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomStateData(XmlElement & xmlStateNode) {};
};

