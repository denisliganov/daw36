
#include "36_listbox.h"








ListBoxx::ListBoxx(std::string name)
{
    setObjName(name);

    setFontId(FontSmall);

    scrWidth = 12;

    entryHeight = textHeight + 5;

    addObject(vscr = new Scroller(true));
    addObject(hscr = new Scroller(false));
}

void ListBoxx::drawSelf(Graphics& g)
{
    setc(g, .25f);
    fillx(g, 0, headerHeight, width, height - headerHeight);

    //setc(g, 0.15f);
    //rectx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.4f);
    fillx(g, 0, 0, width, headerHeight - 2);

    setc(g, 1.f);
    txtfit(g, FontSmall, getObjName(), 3, headerHeight - 5, width);

    g.saveState();
    g.reduceClipRegion(x1 + 1, y1 + headerHeight + 1, width - 2, height - headerHeight - 2);

    int w = vscr->isActive() ? width : width - scrWidth;
    int y = headerHeight;

    int yoffs = -vscr->getOffset();
    int scrWidth = 10;
    int entryNum = 0;

    if (entries.size())
    {
        for (auto e : entries)
        {
            if (yoffs + headerHeight >= 0)
            {
                // Within the visible part

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

                //gTextFitS(g, fontId, e, 4, yoffs + y + entryHeight - 4, w - 2);
            }
            else if (yoffs > (vscr->getOffset() + vscr->getVisiblePart()))
            {
                // Went off the visible part

                break;
            }

            yoffs += entryHeight;
            entryNum++;
        }
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
        vscr->setCoords1(width - 12, headerHeight, 12, height - (headerHeight));

        vscr->updBounds(entries.size() * entryHeight + 3*entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVis(false);
    }
}



ListBoxS::ListBoxS(std::string name)
{
    setObjName(name);

    setFontId(FontSmall);

    scrWidth = 12;

    entryHeight = textHeight + 5;

    addObject(vscr = new Scroller(true));
    addObject(hscr = new Scroller(false));
}

void ListBoxS::addEntry(String entry,bool set_current)
{ 
    entries.push_back(entry); 

    if (set_current) 
        curr = entries.size()-1; 

    remap(); 
}

bool ListBoxS::isEntryHeading(int entry)
{
    if (entries[entry].startsWith(T("--==")))
        return true;
    else
        return false;
}

void ListBoxS::drawSelf(Graphics& g)
{
    setc(g, .25f);
    fillx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.4f);
    fillx(g, 0, 0, width, headerHeight - 2);

    setc(g, 1.f);
    txtfit(g, FontSmall, getObjName(), 3, headerHeight - 5, width);

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
            if (entryNum == curr)
            {
                //setc(g, 0.5f);

                setc(g, 0xffFFFF40, .5f, 1.f);
            }
            else if (isEntryHeading(entryNum))
            {
                setc(g, 0.25f);
            }
            else
            {
                setc(g, 0.34f);
            }

            fillx(g, 0, yoffs + y, w, entryHeight - 1);

            if (entryNum == curr)
            {
                setc(g, 0xffFFFF40);
            }
            else
            {
                setc(g, 1.f);
            }

            String str = isEntryHeading(entryNum) ? T("     ") + e.substring(4) : e;

            gTextS(g, fontId, str, 14, yoffs + y + entryHeight + 4);
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

void ListBoxS::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    if (obj == vscr)
    {
        remapAndRedraw();
    }
}

void ListBoxS::handleMouseDrag(InputEvent& ev)
{
}

void ListBoxS::handleMouseDown(InputEvent& ev)
{
    if ((ev.mouseY - y1) > headerHeight)
    {
        int entry = (ev.mouseY - (y1 + headerHeight) + vscr->getOffset())/entryHeight;

        if (entry < entries.size() && !isEntryHeading(entry))
        {
            curr = entry;

            parent->handleChildEvent(this, ev);
        }

        redraw();
    }
}

void ListBoxS::handleMouseWheel(InputEvent& ev)
{
    vscr->handleMouseWheel(ev); //setOffset(vscr->getOffset() - ev.wheelDelta);

    remapAndRedraw();
}

void ListBoxS::handleMouseUp(InputEvent& ev)
{
    parent->handleChildEvent(this, ev);
}

void ListBoxS::remap()
{
    if (entries.size() * entryHeight > height)
    {
        vscr->setCoords1(width - 12, headerHeight, 12, height - (headerHeight));

        vscr->updBounds(entries.size() * entryHeight + 3*entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVis(false);
    }
}


