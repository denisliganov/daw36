
#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
//#include <experimental/filesystem>

#include <list>
#include <string>


#include "36_globals.h"
#include "36_objects.h"




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


class Browser : public Gobj
{
protected:

            void    disableAllEntries();

public:

            Browser(const char* dirpath);
           ~Browser();

            std::string     path;
            std::string     samplespath;
            std::string     projectspath;
            std::string     lastdir;

            std::string             currbt;
            std::list<std::string>  directories;
            std::list<BrwEntry*>    entries[Browse_Max];

            BrwMode         browsingMode;
            unsigned int    viewMask;
            Instrument*     ipreview;
            Scroller*       brwScroller;
            BrwEntry*       currEntry;

            float           scrollOffset;
            float           fullSpan;

            int             brwIndex;
            int             currIndex;
            int             visibleSpan;
            bool            plugsscanned;


            Button36*       btSamples;
            Button36*       btDevices;
            Button36*       btPlugins;

            void            setCurrentEntry(int index);
            void            setCurrentIndex(int index);
            void            activateContextMenuItem(std::string item);
            ContextMenu*      createContextMenu();
            bool            isMouseTouching(int mx, int my);
            void            activateEntry(BrwEntry* be);
            void            handleMouseWheel(InputEvent& ev);
            void            handleMouseDrag(InputEvent& ev);
            void            handleMouseDown(InputEvent& ev);
            void            handleMouseUp(InputEvent& ev);
            void            mapObjects();
            void            drawSelf(Graphics& g);
            BrwEntry*       getEntryByIndex(int index);
            void            setMode(BrwMode mode);
            bool            isFileMode();
            bool            isDevMode();
            void            cleanEntries();
            void            update();
            void            updateEntries();
            void            updateParamsData();
            void            updateCurrentHighlight();
            void            setOffset(float new_offs);
            float           getOffset();
            void            setViewMask(unsigned int vmask);
            void            prreviewSample(bool down);
            void            scanDirForFiles(std::string path, std::string ext, bool recurs);
            void            handleChildEvent(Gobj * obj, InputEvent& ev);
            void            addSearchDir(std::string dir);
            void            removeSearchDir(std::string dir);
////
            void            addEntry(BrwEntry* entry);
            BrwEntry*       addEntry(DevClass ec, std::string name, std::string path = "", std::string alias ="");
            void            removeEntry(BrwEntry* pEntry);
            BrwEntry*       getEntryByPath(char* path);
            FILE*           rescanFromVstFile();
            void            clearVstFile();
            void            rescanDevices();
            void            scanDirForDevs(char *path, char mode, FILE* fhandle, ScanThread* thread);
            void            devScanProc(ScanThread* thread);
            void            initInternalDevices();

};

