
#pragma once

#include "36_globals.h"
#include "36_scroller.h"

#include <vector>



class ListBoxx : public Scrolled
{
public:
            ListBoxx(std::string name);

            void                addEntry(std::string entry)                     { entries.push_back(entry); remap(); }
            void                addEntry1(String entry)                         { entries1.push_back(entry); remap(); }
            void                setList(std::vector<std::string>&  entries_list)  { entries = entries_list;  remap(); }
            void                setCurrent(int curr)                            { currentEntry = curr; }
            int                 getCurrent()                                    { return currentEntry; }
            std::string         getCurrentName()                                { return entries[currentEntry]; }

protected:

            void                drawSelf(Graphics& g);
    virtual void                handleChildEvent(Gobj * obj,InputEvent & ev);
    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseWheel(InputEvent& ev);
    virtual void                remap();

//private:

            int                 currentEntry;
            int                 scrWidth;
            int                 entryHeight;

            std::vector<std::string>    entries;

            std::vector<String>         entries1;
};


