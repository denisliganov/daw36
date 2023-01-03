


#include <string>

#include "36_brwentry.h"
#include "36_browser.h"
#include "36_config.h"
#include "36_draw.h"
#include "36_text.h"




FileType GetFileTypeByExtension(std::string extension)
{
    if(extension == WavExt)
    {
        return FType_Wave;
    }
    else if (extension == VstExt)
    {
        return FType_VST;
    }
    else if (extension == ProjExt)
    {
        return FType_Projects;
    }
    else
    {
        return FType_Unknown;
    }

}

BrwEntry::BrwEntry(FileType ft, long s, unsigned a, std::string nm, std::string pth) : ftype(ft), size(s), attrs(a), path(pth) 
{ 
    objTitle = nm; 
}

BrwEntry::BrwEntry(DevClass dclass, std::string nm, std::string pth, std::string al) : devClass(dclass), alias(al), path(pth) 
{ 
    objTitle = nm; 
}

BrwEntry::BrwEntry(Device36* preset_dev) : dev(preset_dev) 
{
}

ContextMenu* BrwEntry::createContextMenu()
{
    return NULL;
/*
    Menu* menu = new Menu(Obj_MenuPopup);

    ((Browser*)parent)->setCurrentEntry(listIndex);

    menu->AddItem("Open location");
    menu->AddItem("Delete file");
    menu->AddItem("Load");

    return menu;
    */
}

void BrwEntry::activateContextMenuItem(std::string item)
{
    if(item == "Open location")
    {
        HINSTANCE retval = ShellExecute(WinHWND, "explorer", path.data(), "/select", NULL, SW_SHOWNORMAL);
    }
}

uint32 BrwEntry::getModeColor()
{
    return 0xffDDEEFF;
}

void BrwEntry::brwEntryRect(Graphics& g, int x, int y, int w, int h)
{
    if (w > 0)
    {
        gSetMonoColor(g, 0.4f);
        g.fillRect(x, y, w, h);

        gSetMonoColor(g, 0.35f);
        g.drawHorizontalLine(y, (float)x, float(x + w - 1));
    }
}

void BrwEntry::drawDevEntry(Graphics& g, int x, int y, int w)
{
    brwEntryRect(g, x, y, w - BrwEntryOffset, BrwEntryHeight);

    gSetMonoColor(g, 0.9f);

    gTextFit(g, FontSmall, objTitle, x + 15, y + BrwEntryHeight - 3, w - 20);
}

void BrwEntry::drawFileEntry(Graphics& g, int x, int y, int w)
{
    //if(ftype & viewMask)
    {
        std::string fname;

        brwEntryRect(g, x, y, w - BrwEntryOffset, BrwEntryHeight);

        gSetMonoColor(g, 0.9f);

        fname = objTitle;

        FontId fid = FontSmall;

        gTextFit(g, fid, fname, x + 10, y + BrwEntryHeight - 3, w - 20);
    }
}

void BrwEntry::drawSelf(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(dx1, dy1, dwidth, dheight);
    
    if (isGenerator() || isEffect())
    {
        drawDevEntry(g, x1, y1, width);
    }
    else
    {
        drawFileEntry(g, x1, y1, width);
    }

    // Entry highlight
/*
    if(currIndex == listIndex)
    {
        g.setColour(Colour(0x3cFFFFFF));
    
        gFillRect(g, x1, y1, x2, y2);
    }*/

    g.restoreState();
}

