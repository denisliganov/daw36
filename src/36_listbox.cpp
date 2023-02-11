
#include "36_listbox.h"



ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    setFontId(FontSmall);

    border = 1;

    scrWidth = 12;
}

void ListBoxx::drawSelf(Graphics& g)
{
    setc(g, .1f);
    fillx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.5f);
    rectx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.35f);
    fillx(g, 0, 0, width, headerHeight - 2);

    setc(g, 0.9f);
    txtfit(g, FontBold, getObjName(), 3, headerHeight - 5, width);

    g.saveState();
    g.reduceClipRegion(x1 + 1, y1 + headerHeight + 1, width - 2, height - headerHeight - 1);

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
                setc(g, 0.3f);
            }

            fillx(g, 0, yoffs + y, w, headerHeight);
            setc(g, 0.2f);
            lineH(g, yoffs + y, 0, w);

            setc(g, 1.f);
            txtfit(g, fontId, e, 4, yoffs + y + headerHeight - 3, w - 2);
        }
        else if (yoffs > (vscr->getOffset() + vscr->getVisiblePart()))
        {
            // Went off the visible part

            break;
        }

        yoffs += headerHeight;
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
        int entry = (ev.mouseY - (y1 + headerHeight) + vscr->getOffset())/headerHeight;

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
    //if (vscr->isActive())
    if (entries.size() * headerHeight > height)
    {
        vscr->setCoords1(width - 12, headerHeight + 1, 11, height - headerHeight - 2);

        vscr->updBounds(entries.size() * headerHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVis(false);
    }
}


