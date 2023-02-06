
#include "36_listbox.h"



ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    fontId = FontSmall;

    border = 1;
    entryHeight = gGetTextHeight(fontId) + 2;
}

void ListBoxx::drawSelf(Graphics& g)
{
    fill(g, 0.1f);
    rect(g, 0.2f);

    int yoffs = -vscr->getOffset();

    int scrWidth = 10;

    int w = vscr->isActive() ? width : width - 10;

    for (auto e : entries)
    {
        if (yoffs + entryHeight >= 0)
        {
            // Within the visible part
/*
            setc(g, .6f);
            rectx(g, 0, yoffs, width, entryHeight);
            setc(g, .8f);
            txtfit(g, fontId, e, 2, yoffs + entryHeight - 1, width - 2); 
*/

            setc(g, 0.3f);
            fillx(g, 0, yoffs, width, entryHeight);
            setc(g, 0.2f);
            lineH(g, yoffs, 0, width);

            setc(g, 0.9f);
            txtfit(g, fontId, e, 2, yoffs + entryHeight - 3, width - 2);
        }
        else if (yoffs > (vscr->getOffset() + vscr->getVisiblePart()))
        {
            // Went off the visible part

            break;
        }

        yoffs += entryHeight;
    }
}

void ListBoxx::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    if (obj == vscr)
    {
        remapAndRedraw();
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
    vscr->setOffset(vscr->getOffset() - ev.wheelDelta);

    remapAndRedraw();
}

void ListBoxx::handleMouseUp(InputEvent& ev)
{
    
}

void ListBoxx::remap()
{
    //if (vscr->isActive())
    if (entries.size() * entryHeight > height)
    {
        vscr->setCoords1(width - 10, 0, 10, height);
        
        vscr->updBounds(entries.size() * entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVisible(false);
    }
}


