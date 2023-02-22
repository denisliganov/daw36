
#pragma once

#include "36_globals.h"
#include "36_scroller.h"
#include "36_listbox.h"

#include <vector>





typedef enum EntryType
{
    Entry_Unknown           = 0x1,
    Entry_Directory         = 0x2,
    Entry_Wave              = 0x4,
    Entry_VST               = 0x8,
    Entry_Native            = 0x10,
    Entry_Projects          = 0x20,
    Entry_LevelDirectory    = 0x40,
    Entry_DiskSelector      = 0x80,
    Entry_DiskDrive         = 0x100
}EntryType;


class BrwListEntry : public Gobj
{
public:

            BrwListEntry(std::string name, std::string path, EntryType entry_type);

protected:

            std::string         filePath;
            EntryType           type;

};


class BrowserList  : public ListBoxx
{
public:

            BrowserList(std::string name);
            void                addEntry(BrwListEntry* entry)                     { brwEntries.push_back(entry); remap(); }
            void                setList(std::vector<BrwListEntry*>&  entries_list)  { brwEntries = entries_list;  remap(); }
            void                setCurrent(int curr)                            { currentEntry = curr; }
            int                 getCurrent()                                    { return currentEntry; }
            std::string         getCurrentName()                                { return brwEntries[currentEntry]; }

protected:

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);

            std::vector<BrwListEntry*>     brwEntries;
            int                 currentEntry;
};




