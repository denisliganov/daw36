
#include "36_listbox.h"



ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    setFontId(FontSmall);

    scrWidth = 12;

    entryHeight = headerHeight;
}

void ListBoxx::drawSelf(Graphics& g)
{
    setc(g, .25f);
    fillx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.15f);
    rectx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.4f);
    fillx(g, 0, 0, width, headerHeight - 2);

    setc(g, 1.f);
    txtfit(g, FontBold, getObjName(), 3, headerHeight - 5, width);

    g.saveState();
    g.reduceClipRegion(x1 + 1, y1 + headerHeight + 1, width - 2, height - headerHeight - 2);

    int w = vscr->isActive() ? width : width - scrWidth;
    int y = headerHeight;

    int yoffs = -vscr->getOffset();
    int scrWidth = 10;
    int entryNum = 0;

    for (auto e : entries)
    {
        if (yoffs + headerHeight >= 0)
        {
            // Within the visible part
/*
            setc(g, .6f);
            rectx(g, 0, yoffs, width, headerHeight);
            setc(g, .8f);
            txtfit(g, fontId, e, 2, yoffs + headerHeight - 1, width - 2); 
*/
            if (entryNum == currentEntry)
            {
                setc(g, 0.5f);
            }
            else
            {
                setc(g, 0.34f);
            }

            fillx(g, 0, yoffs + y, w, entryHeight - 1);

            //setc(g, 0.2f);
            //lineH(g, yoffs + y, 0, w);

            setc(g, 1.f);
            txtfit(g, fontId, e, 4, yoffs + y + entryHeight - 4, w - 2);
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
    if ((ev.mouseY - y1) > headerHeight)
    {
        int entry = (ev.mouseY - (y1 + headerHeight) + vscr->getOffset())/entryHeight;

        if (entry < entries.size())
        {
            currentEntry = entry;

            parent->handleChildEvent(this, ev);
        }

        redraw();
    }
}

void ListBoxx::handleMouseWheel(InputEvent& ev)
{
    vscr->handleMouseWheel(ev); //setOffset(vscr->getOffset() - ev.wheelDelta);

    remapAndRedraw();
}

void ListBoxx::handleMouseUp(InputEvent& ev)
{
    parent->handleChildEvent(this, ev);
}

void ListBoxx::remap()
{
    if (entries.size() * entryHeight > height)
    {
        vscr->setCoords1(width - 12, headerHeight + 1, 12, height - headerHeight - 2);

        vscr->updBounds(entries.size() * entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVis(false);
    }
}


