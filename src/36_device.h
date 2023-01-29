

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

    virtual void                addParam(Parameter* param);
    virtual void                addParamWithControl(Parameter* param, std::string ctrl_name = "", Control* ctrl = NULL);
    virtual SubWindow*          createWindow() { return NULL;  }
            void                dequeueParamEnvelope(Trigger* tgenv);
            void                enqueueParamEnvelope(Trigger* tgenv);
    virtual void                forceStop() {};
            int                 getIndex() { return devIdx; }
    virtual Parameter*          getParamByName(char *param_name);
    virtual Parameter*          getParamByIndex(int devIdx);
    virtual bool                getParamLock() { return paramLocked; };
            std::list<Parameter*>   getParams() { return params; }
            std::list<BrwEntry*>   getPresets() { return presets; }
            BrwEntry*           getCurrPreset() { return currPreset; }
    virtual void                handleParamUpdate(Parameter* param = NULL) {};
            void                handleWindowClosed();
            bool                isWindowVisible();
            bool                isPreviewOnly() { return previewOnly; }
            bool                isInternal()    { return internal; }
    virtual void                reset() { }
    virtual void                removeParam(Parameter* param);
    virtual void                setParamLock(bool lock) { paramLocked = lock; };
    virtual void                scanForPresets();
    virtual void                setBPM(float bpm) {};
    virtual void                setBufferSize(unsigned bufferSize) {};
    virtual void                setSampleRate(float sampleRate) {};
    virtual void                showWindow(bool show);


protected:

            BrwEntry*           currPreset;
            std::string         currPresetName;
            int                 devIdx;
            DevClass            devClass;
            Trigger*            envelopes;
            std::string         filePath;
            SubWindow*          guiWindow;
            bool                internal;
            bool                isLoading;
            int                 lastParamIdx;
            std::string         presetPath;
            Button36*           previewButton;
            bool                previewOnly;
            std::list<BrwEntry*>        presets;
            std::list<Parameter*>       params;
            long                uniqueId;

            void                deletePresets();
            BrwEntry*           getPreset(char* objName);
            BrwEntry*           getPreset(long devIdx);
            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
            long                getPresetIndex(char* objName);
            void                removeElements();
            void                restoreStateData(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomStateData(XmlElement & xmlStateNode) {};
            bool                setPresetByName(BrwEntry* preset);
            void                savePreset();
            bool                setPresetByIndex(long index) { return true; };
            void                savePresetAs(char *preset_name);
            void                saveStateData(XmlElement & xmlParentNode, char* preset_name = NULL, bool global = false);
            void                saveCustomStateData(XmlElement & xmlParentNode) {};
            bool                setPresetByName(char* objName);
            void                setIndex(int idx)  { devIdx = idx; }

private:

            bool                paramLocked;
};

