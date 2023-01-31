
#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
//#include <experimental/filesystem>

#include <list>
#include <string>


#include "36_globals.h"
#include "36_objects.h"
#include "36_scroller.h"




typedef enum BrwMode
{
    Browse_Projects,
    Browse_Samples,
    Browse_InternalDevs,
    Browse_ExternalDevs,
    Browse_Presets,
    Browse_Folders,
    Browse_Max
}BrwMode;


class Browser : public Scrolled
{
protected:

            void    disableAllEntries();

public:

            Browser(const char* dirpath);
           ~Browser();

            int                 brwIndex;
            BrwMode             browsingMode;
            BrwEntry*           currEntry;
            int                 currIndex;
            Instrument*         ipreview;
            std::string         lastdir;
            bool                plugsscanned;
            std::string         path;
            std::string         projectspath;
            std::string         samplespath;
            unsigned int        viewMask;

            std::list<std::string>  directories;
            std::list<BrwEntry*>    entries[Browse_Max];

            Button36*           btSamples;
            Button36*           btDevices;
            Button36*           btPlugins;

            void                addSearchDir(std::string dir);
            void                activateMenuItem(std::string item);
            void                addEntry(BrwEntry* entry);
            BrwEntry*           addEntry(DevClass ec, std::string name, std::string path = "", std::string alias ="");
            void                activateEntry(BrwEntry* be);
            ContextMenu*        createContextMenu();
            bool                checkMouseTouching(int mx, int my);
            void                clearVstFile();
            void                cleanEntries();
            void                devScanProc(ScanThread* thread);
            void                drawSelf(Graphics& g);
            BrwEntry*           getEntryByIndex(int index);
            BrwEntry*           getEntryByPath(char* path);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            void                initInternalDevices();
            bool                isFileMode();
            bool                isDevMode();
            void                prreviewSample(bool down);
            void                removeSearchDir(std::string dir);
            void                removeEntry(BrwEntry* pEntry);
            void                remap();
            FILE*               rescanFromVstFile();
            void                rescanDevices();
            void                setViewMask(unsigned int vmask);
            void                scanDirForFiles(std::string path, std::string ext, bool recurs);
            void                setCurrentEntry(int index);
            void                setCurrentIndex(int index);
            void                setMode(BrwMode mode);
            void                scanDirForDevs(char *path, char mode, FILE* fhandle, ScanThread* thread);
            void                update();
            void                updateEntries();
            void                updateParamsData();
            void                updateCurrentHighlight();
};

