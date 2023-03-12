

#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"
#include "36_params.h"
#include "36_paramobject.h"
#include "36_config.h"
#include "36_vu.h"





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
friend Eff;

public:
            Device36();
    virtual ~Device36();

            void                addBasicParamSet();
    virtual SubWindow*          createWindow();
    virtual void                createSelfPattern();
    virtual void                forceStop();
            int                 getIndex() { return devIdx; }
            float               getLastNoteLength() { return lastNoteLength; }
            int                 getMuteCount()  { return muteCount; }
            bool                isEnabled()     { return enabled->getBoolValue(); }
            Gobj*               getContainer() { return container; }
            void                removeElements();
            void                handleWindowClosed();
            bool                isWindowVisible();
            bool                isPreviewOnly() { return previewOnly; }

            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
    std::vector<std::string>&   getPresetList()  { return presets; }

    virtual bool                setPreset(long index) { return true; };
    virtual bool                setPreset(std::string pname);
            void                savePreset();

            void                setIndex(int idx) { devIdx = idx; }
    virtual void                setBPM(float bpm) {};
    virtual void                setBufferSize(unsigned bufferSize) {};
    virtual void                setSampleRate(float sampleRate) {};
    virtual void                showWindow(bool show);
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

    virtual void                reset() { }

            void                addNote(Note* note);
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);
            void                setVU(ChanVU* v) { vu = v; }


            std::list<Trigger*> activeTriggers;
            float               cfsV;
            int                 devIdx;
            long                endFrame;  // last frame to fill
            Envelope*           envVol;
            SubWindow*          guiWindow;
            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;
            int                 muteCount;
            std::list<Note*>    notes;
            float               outBuff[MAX_BUFF_SIZE*2];      // Date for whole session
            float               pan0, pan1, pan2, pan3;
            int                 rampCount;
            float               rampCounterV;
            Pattern*            selfPattern;
            Note*               selfNote;
            float               tempBuff[MAX_BUFF_SIZE*2];     // Data for single trigger
            float               volBase;
            Parameter*          vol;
            Parameter*          pan;
    std::vector<std::string>    presets;
            bool                previewOnly;
            long                uniqueId;

protected:

            Gobj*               container;
            Parameter*          enabled;
            std::string         filePath;
            bool                isLoading;
            int                 lastParamIdx;
            InstrType           type;
            ChanVU*             vu;

            void                deletePresets();
            void                restoreState(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomState(XmlElement & xmlStateNode) {};
            void                saveState(XmlElement& xmlParentNode, char* preset_name = NULL, bool global = false);
    virtual void                saveCustomState(XmlElement& xmlParentNode) {};
};

