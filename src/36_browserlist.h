
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


class ListEntry : public Gobj
{
public:

            ListEntry(std::string name, std::string path);

protected:

            bool                isDirectory;

            std::string         type;
};


class BrowserList  : public ListBoxx
{
public:

            BrowserList(std::string name);

protected:

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);

            std::vector<ListEntry*>     brwEntries;
};




