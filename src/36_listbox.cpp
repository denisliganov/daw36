
#include "36_listbox.h"




ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    fontId = FontDix;
}

void ListBoxx::drawSelf(Graphics& g)
{
    int offset = entryHeight;

    for (auto e : entries)
    {
        if (offset - vscr->getOffset() > 0)
        {
            // Within the visible part, draw it
            setc(g, .4f);
            fillx(g, 0, offset, width, offset + entryHeight - 1);
            setc(g, .6f);
            rectx(g, 0, offset, width, offset + entryHeight - 1);

            txtfit(g, fontId, e, 2, offset - 1, width - 2); 
        }
        else if (offset > (vscr->getOffset() + vscr->getVisiblePart()))
        {
            // Went off the visible part, exit drawing
            break;
        }

        offset += entryHeight;
    }
}

void ListBoxx::handleMouseDrag(InputEvent& ev)
{
    
}

void ListBoxx::handleMouseDown(InputEvent& ev)
{
    
}

void ListBoxx::handleMouseWheel(InputEvent& ev)
{
    
}

void ListBoxx::handleMouseUp(InputEvent& ev)
{
    
}

void ListBoxx::remap()
{
    vscr->updBounds(entries.size() * entryHeight, height, vscr->getOffset());
}


