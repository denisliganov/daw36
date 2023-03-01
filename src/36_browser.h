
#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
//#include <experimental/filesystem>

#include <list>
#include <string>
#include <vector>

#include "36_globals.h"
#include "36_objects.h"
#include "36_paramobject.h"



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


class Browser : public ParamObject
{
public:

            Browser(std::string dirpath);
            void                activateMenuItem(std::string item);
            void                addSearchDir(std::string dir, bool folders=false, bool recursive=false);
            ContextMenu*        createContextMenu();
            void                clearVstFile();
            void                devScanProc(ScanThread* thread);
            void                drawSelf(Graphics& g);
            BrwEntry*           getEntryByIndex(int index);
            BrwEntry*           getEntryByPath(char* path);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                isFileMode();
            bool                isDevMode();
            void                prreviewSample(bool down);
            void                removeSearchDir(std::string dir);
            void                removeEntry(BrwEntry* pEntry);
            void                remap();
            FILE*               rescanFromVstFile();
            void                rescanDevices();
            void                scanDirForFiles(std::string path, std::string ext, bool recurs, std::vector<std::string>& flist);
            void                scanDirForDevs(char *path, char mode, FILE* fhandle, ScanThread* thread);
            void                updateEntries();

            BrwMode             browsingMode;

protected:

            int                 brwIndex;

            BrwEntry*           currEntry;
            int                 currIndex;
            Instrument*         ipreview;
            std::string         lastdir;
            bool                plugsscanned;
            std::string         path;
            std::string         projectsPath;
            std::string         samplesPath;
            unsigned int        viewMask;

            std::list<std::string>      directories;
            std::vector<ListBoxx*>      listBoxes;
            std::vector<BrowserList*>   brwLists;

            std::list<BrwEntry*>    entries[Browse_Max];


            BrowserList*        fileBox;

            BrowserList*        plugList;
            BrowserList*        sampleList1;
            BrowserList*        vstList1;
            BrowserList*        vstList2;
            BrowserList*        internalList1;
            BrowserList*        projectsList1;

private:
            
};

