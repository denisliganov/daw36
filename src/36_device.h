

#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"



class Device36 : public Gobj
{
friend InstrPanel;

public:
            Device36();
    virtual ~Device36();

    virtual void                forceStop() {};
    virtual void                addParam(Parameter* param);
    virtual void                removeParam(Parameter* param);
    virtual void                handleParamUpdate(Parameter* param = NULL) {};
            void                handleWindowClosed();
    virtual void                addParamWithControl(Parameter* param, std::string ctrl_name = "", Control* ctrl = NULL);
    virtual Parameter*          getParamByName(char *param_name);
    virtual Parameter*          getParamByIndex(int devIdx);
    virtual void                setParamLock(bool lock) { paramLocked = lock; };
    virtual bool                getParamLock() { return paramLocked; };
    virtual void                scanForPresets();
    virtual void                setBPM(float bpm) {};
    virtual void                setBufferSize(unsigned bufferSize) {};
    virtual void                setSampleRate(float sampleRate) {};
    virtual void                reset() { }
            void                enqueueParamEnvelope(Trigger* tgenv);
            void                dequeueParamEnvelope(Trigger* tgenv);

    virtual SubWindow*          createWindow() { return NULL;  }
    virtual void                showWindow(bool show);
            bool                isWindowVisible();

            std::list<BrwEntry*>        presets;
            std::list<Parameter*>       params;

            DevClass            devClass;
            std::string         filePath;
            std::string         presetPath;
            std::string         currPresetName;        // we use this string to identify current present on scanning

            int                 devIdx;
            int                 lastParamIdx;
            long                uniqueId;
            bool                previewOnly;
            bool                internal;
            bool                isLoading;
            Trigger*            envelopes;
            BrwEntry*           currPreset;


protected:

            SubWindow*          guiWindow;
            Button36*           previewButton;

    // preset handling
    virtual void                savePreset();
    virtual BrwEntry*           getPreset(char* objName);
    virtual BrwEntry*           getPreset(long devIdx);
    virtual long                getNumPresets();
    virtual void                getPresetName(long devIdx, char *name);
    virtual long                getPresetIndex(char* objName);
    virtual bool                setPresetByName(BrwEntry* preset);
    virtual bool                setPresetByIndex(long index) { return true; };
    virtual void                savePresetAs(char *preset_name);

    // state handling
    virtual void                saveStateData(XmlElement & xmlParentNode, char* preset_name = NULL, bool global = false);
    virtual void                restoreStateData(XmlElement & xmlStateNode, bool global = false);
    virtual void                saveCustomStateData(XmlElement & xmlParentNode) {};
    virtual void                restoreCustomStateData(XmlElement & xmlStateNode) {};
    virtual void                deletePresets();
    virtual bool                setPresetByName(char* objName);
            void                removeElements();

    virtual void                setIndex(int idx)  { devIdx = idx; }
            int                 getIndex() { return devIdx; }

private:

            bool                paramLocked;
};

