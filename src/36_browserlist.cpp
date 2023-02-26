
#include "36_browserlist.h"
#include "36_config.h"
#include "36_utils.h"



BrwListEntry::BrwListEntry(std::string name, std::string path, EntryType entry_type)
{
    setObjName(name);

    filePath = path;

    type = entry_type;
}



BrowserList::BrowserList(std::string name) : ListBoxx(name)
{
    setFontId(FontDix);

    headerHeight = textHeight + 8;

    currDir = WorkDirectory;

    scanDirForFiles(currDir, "", false);
}

void BrowserList::drawSelf(Graphics& g)
{
    setc(g, .25f);
    fillx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.05f);
    rectx(g, 0, headerHeight, width, height - headerHeight);

    setc(g, 0.4f);
    fillx(g, 0, 0, width, headerHeight);

    setc(g, 1.f);
    txtfit(g, FontBold, getObjName(), 6, headerHeight - 7, width);

    g.saveState();
    g.reduceClipRegion(x1 + 1, y1 + headerHeight + 2, width - 2, height - (headerHeight + 2));

    int w = vscr->isActive() ? width : width - scrWidth;
    int y = headerHeight;

    int yoffs = -vscr->getOffset();
    int scrWidth = 10;
    int entryNum = 0;

    for (auto entry : brwEntries)
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
                setc(g, 0.4f);
            }
            else
            {
                setc(g, 0.3f);
            }

            fillx(g, 0, yoffs + y, w, entryHeight - 1);

            //setc(g, 0.2f);
            //lineH(g, yoffs + y, 0, w);

            if (entry->getType() == Entry_Directory)
            {
                setc(g, 1.f);
            }
            else
            {
                setc(g, .8f);
            }
            txtfit(g, fontId, entry->getObjName(), 4, yoffs + y + entryHeight - 4, w - 2);
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


void BrowserList::handleMouseDown(InputEvent& ev)
{
    if ((ev.mouseY - y1) > headerHeight)
    {
        int entry = (ev.mouseY - (y1 + headerHeight) + vscr->getOffset())/entryHeight;

        if (entry < brwEntries.size())
        {
            currentEntry = entry;

            if (ev.doubleClick)
            {
                if (brwEntries[currentEntry]->getType() == Entry_Directory)
                {
                    currDir = brwEntries[currentEntry]->getPath();

                    while(brwEntries.size() > 0)
                    {
                        delete brwEntries[brwEntries.size() - 1];
                        brwEntries.pop_back();
                    }

                    scanDirForFiles(currDir + "\\", "", false);

                    remapAndRedraw();
                }
            }

            parent->handleChildEvent(this, ev);
        }

        redraw();
    }
}

void BrowserList::handleMouseWheel(InputEvent& ev)
{
    vscr->handleMouseWheel(ev); //setOffset(vscr->getOffset() - ev.wheelDelta);

    remapAndRedraw();
}

void BrowserList::handleMouseUp(InputEvent& ev)
{
    parent->handleChildEvent(this, ev);
}

void BrowserList::remap()
{
    if (brwEntries.size() * entryHeight > height)
    {
        vscr->setCoords1(width - 12, headerHeight, 12, height - (headerHeight));

        vscr->updBounds(brwEntries.size() * entryHeight + 3*entryHeight, height, vscr->getOffset());
    }
    else
    {
        vscr->setVis(false);
    }
}

void BrowserList::scanDirForFiles(std::string scan_path, std::string extension, bool recurs)
{
    char tempPath[MAX_PATH_LENGTH];

    strcpy(tempPath, (char*)scan_path.data());
    strcat(tempPath, "*.*");

    WIN32_FIND_DATA founddata = {0};

    HANDLE fhandle = FindFirstFile(tempPath, &founddata);

    if(fhandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(strcmp(founddata.cFileName, ".") != 0 && !(founddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {
                if(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    std::string fname = founddata.cFileName;
                    addEntry(new BrwListEntry("[" + fname + "]", scan_path + fname, Entry_Directory));

                    if(recurs)
                    {
                        char dirpath[MAX_PATH_LENGTH];

                        if(strcmp(founddata.cFileName, ".") != 0 && strcmp(founddata.cFileName, "..") != 0)
                        {
                            sprintf(dirpath, "%s%s%s", (char*)scan_path.data(), founddata.cFileName, "\\");

                            scanDirForFiles(dirpath, extension, recurs);
                        }
                    }
                }
            }
        }while(FindNextFile(fhandle, &founddata));

        fhandle = FindFirstFile(tempPath, &founddata);

        do
        {
            if(strcmp(founddata.cFileName, ".") != 0 && !(founddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {
                if(!(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    std::string fname = founddata.cFileName;
                    auto const extpos = fname.find_last_of('.');

                    //if(extpos != std::string::npos)
                    {
                        std::string ext = fname.substr(extpos + 1);

                        ToLowerCase((char*)ext.data());

                        //if(ext == extension || extension == "")
                        {
                            //flist.push_back(fname);

                            BrwListEntry* fileEntry = new BrwListEntry(fname, scan_path + fname, Entry_Default);

                            addEntry(fileEntry);

                            //fileEntry->setObjName(fname);
                            //fileEntry->path = scan_path + fname;
                            //fileEntry->size = (founddata.nFileSizeHigh * (MAXDWORD)) + founddata.nFileSizeLow;
                            //fileEntry->attrs = founddata.dwFileAttributes;
                            //fileEntry->ftype = GetFileTypeByExtension(extension);
                            //fileEntry->listIndex = -1;
                            //addObject(fileEntry);
                            //entries[browsingMode].push_back(fileEntry);
                        }
                    }
                }
            }
        }while(FindNextFile(fhandle, &founddata));

        FindClose(fhandle);
    }
}


