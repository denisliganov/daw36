
#pragma once

#include "36_globals.h"
#include "36_scroller.h"

#include <vector>



class ListBoxx : public Scrolled
{
public:
            ListBoxx(std::string name);

            void                addEntry(std::string entry)                     { entries.push_back(entry); }
            void                setList(std::vector<std::string>&  entries_list)  { entries = entries_list; }
            void                setCurrent(int curr)                            { currentEntry = curr; }
            int                 getCurrent()                                    { return currentEntry; }

protected:

            int                 currentEntry;
            int                 entryHeight;
            FontId              fontId;

            std::vector<std::string>  entries;

            void                drawSelf(Graphics& g);
    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseWheel(InputEvent& ev);
    virtual void                remap();
};


