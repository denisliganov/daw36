
#include "36_listbox.h"



ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    fontId = FontSmall;

    border = 1;
    entryHeight = gGetTextHeight(fontId) + 2;
    scrWidth = 12;
}

void ListBoxx::drawSelf(Graphics& g)
{
    setc(g, .1f);
    fillx(g, 0, entryHeight, width, height - entryHeight);
    setc(g, 0.2f);
    rectx(g, 0, entryHeight, width, height - entryHeight);

    int yoffs = -vscr->getOffset();

    int scrWidth = 10;

    int w = vscr->isActive() ? width : width - scrWidth;
    int y = entryHeight;
    int entryNum = 0;

    g.saveState();
    g.reduceClipRegion(x1, y1 + entryHeight - 1, width, height - entryHeight);

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
            if (entryNum == currentEntry)
            {
                setc(g, 0.6f);
            }
            else
            {
                setc(g, 0.3f);
            }

            fillx(g, 0, yoffs + y, width, entryHeight);
            setc(g, 0.2f);
            lineH(g, yoffs + y, 0, width);

            setc(g, 0.9f);
            txtfit(g, fontId, e, 4, yoffs + y + entryHeight - 3, w - 2);
        }
        else if (yoffs > (vscr->getOffset() + vscr->getVisiblePart()))
        {
            // Went off the visible part

            break;
        }

        yoffs += entryHeight;
        entryNum++;
    }

    g.restoreState();

    setc(g, 0.35f);
    fillx(g, 0, 0, width, entryHeight);
    setc(g, 0.8f);
    txtfit(g, fontId, getObjName(), 3, 12, w - 2);

    setc(g, 0.5f);
    rectx(g, 0, entryHeight, width, height - entryHeight);
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
    currentEntry = (ev.mouseY - y1 + vscr->getOffset())/entryHeight;

    redraw();
}

void ListBoxx::handleMouseWheel(InputEvent& ev)
{
    vscr->handleMouseWheel(ev); //setOffset(vscr->getOffset() - ev.wheelDelta);

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
        vscr->setCoords1(width - 12, entryHeight + 1, 11, height - entryHeight - 2);
        
        vscr->updBounds(entries.size() * entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVisible(false);
    }
}


