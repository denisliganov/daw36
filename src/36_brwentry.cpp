


#include <string>

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
    objName = nm; 
}

BrwEntry::BrwEntry(DevClass dclass, std::string nm, std::string pth, std::string al) : devClass(dclass), alias(al), path(pth) 
{ 
    objName = nm; 
}

BrwEntry::BrwEntry(Device36* preset_dev)
{
    dev = preset_dev;
}

void BrwEntry::activateMenuItem(std::string item)
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

void BrwEntry::drawDevEntry(Graphics& g, int x, int y, int w)
{
    setc(g, 0.4f);
    fillx(g, 0, 0, width - 1, BrwEntryHeight);
    setc(g, 0.35f);
    lineH(g, 0, 0, width - 1);

    setc(g, 0.9f);
    txtfit(g, FontSmall, objName, 15, BrwEntryHeight - 3, width - 20);
}

void BrwEntry::drawFileEntry(Graphics& g, int x, int y, int w)
{
    //if(ftype & viewMask)
    {
        std::string fname;

        setc(g, 0.4f);
        fillx(g, 0, 0, width - 1, BrwEntryHeight);
        setc(g, 0.35f);
        lineH(g, 0, 0, width - 1);

        setc(g, 0.9f);

        fname = objName;

        FontId fid = FontSmall;

        txtfit(g, fid, fname, 10, BrwEntryHeight - 3, width - 20);
    }
}

void BrwEntry::drawSelf(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(dx1, dy1, dwidth, dheight);
    
    drawFileEntry(g, x1, y1, width);

    // Entry highlight
/*
    if(currIndex == listIndex)
    {
        g.setColour(Colour(0x3cFFFFFF));
    
        gFillRect(g, x1, y1, x2, y2);
    }*/

    g.restoreState();
}

