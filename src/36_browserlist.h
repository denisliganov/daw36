
#pragma once

#include "36_globals.h"
#include "36_scroller.h"
#include "36_listbox.h"

#include <vector>







typedef enum BrwListType
{
    BrwList_Files,
    BrwList_Projects,
    BrwList_WavSamples,
    BrwList_SoundFonts,
    BrwList_InternalModules,
    BrwList_VST2,
    BrwList_VST3,
}BrwListType;

typedef enum EntryType
{
    Entry_Default           = 0x1,
    Entry_Directory         = 0x2,
    Entry_Wave              = 0x4,
    Entry_DLL               = 0x8,
    Entry_Native            = 0x10,
    Entry_Projects          = 0x20,
    Entry_LevelDirectory    = 0x40,
    Entry_DiskSelector      = 0x80,
    Entry_DiskDrive         = 0x100
}EntryType;


class BrwListEntry : public Gobj
{
public:

            BrwListEntry(std::string name, std::string path="", EntryType entry_type=Entry_Default);
            EntryType           getType()   { return type; }
            std::string         getPath()   { return filePath; }

protected:

            std::string         filePath;
            EntryType           type;

};


class BrowserList  : public ListBoxx
{
public:

            BrowserList(std::string name, BrwListType t = BrwList_Files);
            void                addEntry(BrwListEntry* entry)                     { brwEntries.push_back(entry); remap(); }
            int                 getCurrent()                                    { return currentEntry; }
            BrwListEntry*       getCurrentName()                                { return brwEntries[currentEntry]; }
            void                setList(std::vector<BrwListEntry*>&  entries_list)  { brwEntries = entries_list;  remap(); }
            void                setCurrent(int curr)                            { currentEntry = curr; }

protected:

            void                composeList();
            void                drawSelf(Graphics& g);
            void                deleteEntries();
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDrag(InputEvent & ev);
    virtual void                remap();
            void                scanDirForFiles(std::string scan_path, std::string extension, bool recurs);

            std::vector<BrwListEntry*>      brwEntries;
            int                             currentEntry;
            std::string                     currDir;

            BrwListType                     type;
};




