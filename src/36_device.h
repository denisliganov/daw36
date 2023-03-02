

#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"
#include "36_paramobject.h"
#include "36_config.h"




class Device36 : public ParamObject
{
friend InstrPanel;

public:
            Device36();
    virtual ~Device36();

    virtual SubWindow*          createWindow() { return NULL;  }
    virtual void                forceStop();
            int                 getIndex() { return devIdx; }
        std::list<BrwEntry*>    getPresets() { return presets; }
            BrwEntry*           getCurrPreset() { return currPreset; }
            BrwEntry*           getPresetByName(std::string pr_name);
            BrwEntry*           getPresetByIndex(long devIdx);

            int                 getMuteCount()  { return muteCount; }
            bool                getBypass()     { return bypass; }

            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
            long                getPresetIndex(char* objName);
    std::vector<std::string>&   getPresetList()  { return pres; }
            void                removeElements();
            void                handleWindowClosed();
            bool                isWindowVisible();
            bool                isPreviewOnly() { return previewOnly; }
            bool                isInternal()    { return internal; }


    virtual void                processData(float* in_buff, float* out_buff, int num_frames);
    virtual void                process(float* in_buff, float* out_buff, int num_frames);

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


    virtual void                activateTrigger(Trigger* tg);
    virtual void                deactivateTrigger(Trigger* tg);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto);
    virtual void                deClick(Trigger* tg, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
    virtual void                generateData(long num_frames = 0, long buff_frame = 0);
    virtual void                preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buff_frame = 0);
    virtual long                processTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0) {return 0;};
    virtual void                postProcessTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
    virtual long                workTrigger(Trigger* tg, long num_frames, long remaining, long buff_frame, long mix_buff_frame);

            void                fillMixChannel(long num_frames, long buff_frame, long mix_buff_frame);

            void                addNote(Note* note);
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);

            std::list<Trigger*> activeTriggers;
            std::list<Note*>    notes;
            float               cfsV;
            long                endFrame;  // last frame to fill
            Envelope*           envVol;
            MixChannel*         mixChannel;

            float               volbase;
            float               pan0, pan1, pan2, pan3;
            int                 rampCount;
            float               rampCounterV;
            float               dataBuff[MAX_BUFF_SIZE*2];     // Initial data
            float               inBuff[MAX_BUFF_SIZE*2];       // Data after separate DSP
            float               outBuff[MAX_BUFF_SIZE*2];      // Output after postprocessing

            ParamVol*           vol;
            ParamPan*           pan;
            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;

            int                 muteCount;

            bool                muteparam;
            bool                soloparam;

protected:

            bool                bypass;

            BrwEntry*           currPreset;
            std::string         currPresetName;
            int                 devIdx;
            std::string         filePath;
            SubWindow*          guiWindow;
            bool                internal;
            bool                isLoading;
            int                 lastParamIdx;

            std::string         presetPath;
    std::list<BrwEntry*>        presets;
    std::vector<std::string>    pres;
            Button36*           previewButton;
            bool                previewOnly;
            long                uniqueId;

            void                deletePresets();
            void                restoreStateData(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomStateData(XmlElement & xmlStateNode) {};
};

