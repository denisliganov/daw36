

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
friend  InstrPanel;
friend  Instrument;
friend  Eff;
friend  Grid;
friend  MixChannel;
friend  Vst2Plugin;
friend  Sample;

public:
            Device36();
    virtual ~Device36();

            void                addBasicParamSet();
            void                addNote(Note* note);
    virtual void                activateTrigger(Trigger* tg);
    virtual SubWindow*          createWindow();
    virtual void                createSelfPattern();
            ContextMenu*        createContextMenu()     {  return parent->createContextMenu();  }
    virtual void                deactivateTrigger(Trigger* tg);
    virtual void                deClick(Trigger* tg, long num_frames, long buff_frame = 0);
            void                fillOutputBuffer(float* out_buff, long num_frames, long buff_frame, long mix_buff_frame);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto);
    virtual void                forceStop();
            std::string         getPath()                   { return filePath; }
    virtual void                generateData(float* in_buff, float* out_buff, long num_frames, long mix_buff_frame = 0);
            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
    std::vector<std::string>&   getPresetList()         { return presets; }
            int                 getIndex()              { return devIdx; }
            float               getLastNoteLength()     { return lastNoteLength; }
            int                 getMuteCount()          { return muteCount; }
            Gobj*               getContainer()          { return container; }
            Note*               getSelfNote()           { return selfNote; }
    virtual long                handleTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0) {return 0;};
            void                handleWindowClosed();
            void                handleMouseWheel(InputEvent & ev);
            void                handleMouseDrag(InputEvent& ev);
            bool                isEnabled()             { return enabled->getBoolValue(); }
            bool                isWindowVisible();
    virtual void                preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buff_frame = 0);
    virtual void                postProcessTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
    virtual long                processTrigger(Trigger* tg, long num_frames, long remaining, long buff_frame);
    virtual void                processDSP(float* in_buff, float* out_buff, int num_frames);
    virtual void                reset() { }
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                removeElements();
    virtual bool                setPreset(long index)   { return true; };
    virtual bool                setPreset(std::string pname);
            void                savePreset();
            void                setIndex(int idx)       { devIdx = idx; }
    virtual void                setBPM(float bpm) {};
    virtual void                setBufferSize(unsigned bufferSize) {};
    virtual void                setSampleRate(float sampleRate) {};
    virtual void                showWindow(bool show);
            void                setContainer(Gobj* cnt)     { container = cnt; }
            void                setPath(std::string path)       { filePath = path; }
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);
            void                setVU(ChanVU* v) { vu = v; }


private:
            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;
            int                 muteCount;
            std::list<Note*>    notes;

            std::list<Trigger*> activeTriggers;
            float               cfsV;
            int                 devIdx;
            long                endFrame;  // last frame to fill
            Envelope*           envVol;
            SubWindow*          guiWindow;

            float               outBuff[MAX_BUFF_SIZE*2];      // Data for whole session
            Parameter*          pan;
            std::vector<std::string>    presets;
            float               pan0, pan1, pan2, pan3;
            int                 rampCount;
            float               rampCounterV;
            Pattern*            selfPattern;
            Note*               selfNote;

            Gobj*               container;
            Parameter*          enabled;
            std::string         filePath;
            bool                isLoading;
            int                 lastParamIdx;
            float               tempBuff[MAX_BUFF_SIZE*2];     // Data for single trigger
            InstrType           type;
            long                uniqueId;
            ChanVU*             vu;
            Parameter*          vol;
            float               volBase;

            void                deletePresets();
            void                restoreState(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomState(XmlElement & xmlStateNode) {};
            void                saveState(XmlElement& xmlParentNode, char* preset_name = NULL, bool global = false);
    virtual void                saveCustomState(XmlElement& xmlParentNode) {};
};

