

#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"
#include "36_paramobject.h"


class Device36 : public ParamObject
{
friend InstrPanel;

public:
            Device36();
    virtual ~Device36();

    virtual SubWindow*          createWindow() { return NULL;  }
    virtual void                forceStop() {};
            int                 getIndex() { return devIdx; }
            std::list<BrwEntry*>   getPresets() { return presets; }
            BrwEntry*           getCurrPreset() { return currPreset; }
            BrwEntry*           getPresetByName(std::string pr_name);
            BrwEntry*           getPresetByIndex(long devIdx);
            long                getNumPresets();
            void                getPresetName(long devIdx, char *name);
            long                getPresetIndex(char* objName);
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

            std::vector<std::string>&   getPresetList()  { return pres; }

protected:

            BrwEntry*           currPreset;
            std::string         currPresetName;
            int                 devIdx;
            std::string         filePath;
            SubWindow*          guiWindow;
            bool                internal;
            bool                isLoading;
            int                 lastParamIdx;
            std::string         presetPath;
            Button36*           previewButton;
            bool                previewOnly;
            long                uniqueId;

            std::list<BrwEntry*>    presets;

            std::vector<std::string>    pres;

            void                deletePresets();
            void                restoreStateData(XmlElement & xmlStateNode, bool global = false);
            void                restoreCustomStateData(XmlElement & xmlStateNode) {};
};

