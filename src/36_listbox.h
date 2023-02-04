
#pragma once

#include "36_globals.h"
#include "36_scroller.h"



class ListBoxx : public Scrolled
{
public:
            ListBoxx(std::string name);

            void                addEntry(std::string entry)                   { entries.push_back(entry); }
            void                setList(std::list<std::string>&  entries_list) { entries = entries_list; }

protected:

            std::list<std::string>  entries;

            void                drawSelf(Graphics& g);
    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseWheel(InputEvent& ev);
};


