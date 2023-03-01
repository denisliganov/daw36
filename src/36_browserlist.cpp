
#include "36_browserlist.h"
#include "36_config.h"
#include "36_utils.h"
#include "36.h"




BrwListEntry::BrwListEntry(std::string name, std::string path, EntryType entry_type)
{
    setObjName(name);

    filePath = path;

    type = entry_type;
}



BrowserList::BrowserList(std::string name, std::string path, BrwListType t) : ListBoxx(name)
{
    setFontId(FontDix);

    headerHeight = textHeight + 8;

    type = t;

    currDir = path;

    composeList();
}

void BrowserList::composeList()
{
    if (type == BrwList_Files)
    {
        addEntry(new BrwListEntry("***", "", Entry_DiskSelector));

        std::string s = currDir;
        std::string::size_type pos1 = currDir.find_first_of("\\");

        std::string::size_type pos0 = 0;

        while (pos1 != std::string::npos)
        {
            pos0 += pos1;

            std::string path = currDir.substr(0, pos0) + "\\";

            pos0++;

            addEntry(new BrwListEntry("\\" + s.substr(0, pos1) + "\\", path, Entry_LevelDirectory));

            s = s.substr(pos1 + 1);

            pos1 = s.find_first_of("\\");
        }

        scanDirForFiles(currDir, "", false);
    }
    else if (type == BrwList_InternalModules)
    {
        addEntry(new BrwListEntry("1-band Equalizer",  "eff.eq1",       Entry_Native));
        addEntry(new BrwListEntry("3-band Equalizer",  "eff.eq3",       Entry_Native));
        addEntry(new BrwListEntry("Graphic Equalizer", "eff.grapheq",   Entry_Native));
        addEntry(new BrwListEntry("Delay",             "eff.delay",     Entry_Native));
        addEntry(new BrwListEntry("Compressor",        "eff.comp",      Entry_Native));
        addEntry(new BrwListEntry("Reverb",            "eff.reverb",    Entry_Native));
        addEntry(new BrwListEntry("Chorus",            "eff.chorus",    Entry_Native));
        addEntry(new BrwListEntry("Flanger",           "eff.flanger",   Entry_Native));
        addEntry(new BrwListEntry("Phaser",            "eff.phaser",    Entry_Native));
        addEntry(new BrwListEntry("WahWah",            "eff.wah",       Entry_Native));
        addEntry(new BrwListEntry("Distortion",        "eff.dist",      Entry_Native));
        addEntry(new BrwListEntry("BitCrusher",        "eff.bitcrush",  Entry_Native));
        addEntry(new BrwListEntry("Stereoizer",        "eff.stereo",    Entry_Native));
        addEntry(new BrwListEntry("Filter1",           "eff.filter1",   Entry_Native));
        addEntry(new BrwListEntry("Tremolo",           "eff.tremolo",   Entry_Native));
    }
    else if (type == BrwList_WavSamples)
    {
        scanDirForFiles(currDir, "wav", true);
    }
    else if (type == BrwList_VST2 || type == BrwList_VST3)
    {
        scanDirForFiles(currDir, "dll", true);
    }

    currentEntry = -1;
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

            int xc = 5;

            if (type == BrwList_Files)
            {
                setc(g, 1.f);

                if (entry->getType() == Entry_Wave)
                {
                    setc(g, 0xff8AFF8A);
                }
                else if (entry->getType() == Entry_DLL)
                {
                    setc(g, 0xff8A8AFF);
                }
                else if (entry->getType() == Entry_Default)
                {
                    setc(g, .8f);
                }

                if (entry->getType() != Entry_LevelDirectory)
                {
                    xc = 10;
                }
            }
            else if (type == BrwList_InternalModules)
            {
                setc(g, 0xff8AEFFF);
            }
            else if (type == BrwList_WavSamples)
            {
                setc(g, 0xff8AFF8A);
            }
            else if (type == BrwList_VST2)
            {
                setc(g, 0xff8A8AFF);
            }

            txtfit(g, fontId, entry->getObjName(), xc, yoffs + y + entryHeight - 4, w - 2);
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

void BrowserList::deleteEntries()
{
    while(brwEntries.size() > 0)
    {
        delete brwEntries[brwEntries.size() - 1];
        brwEntries.pop_back();
    }
}

void BrowserList::handleMouseDown(InputEvent& ev)
{
    if ((ev.mouseY - y1) > headerHeight)
    {
        int entry = (ev.mouseY - (y1 + headerHeight) + vscr->getOffset())/entryHeight;

        if (entry < brwEntries.size())
        {
            currentEntry = entry;

            parent->handleChildEvent(this, ev);

            redraw();
        }
    }
}

void BrowserList::handleMouseWheel(InputEvent& ev)
{
    vscr->handleMouseWheel(ev); //setOffset(vscr->getOffset() - ev.wheelDelta);

    remapAndRedraw();
}

void BrowserList::handleMouseUp(InputEvent& ev)
{
    if (currentEntry >= 0)
    {
        if (brwEntries[currentEntry]->getType() == Entry_Directory ||
            brwEntries[currentEntry]->getType() == Entry_LevelDirectory ||
            brwEntries[currentEntry]->getType() == Entry_DiskDrive)
        {
            std::string prevDir = "";

            if (brwEntries[currentEntry]->getObjName() == "[..]")
            {
                std::string::size_type pos = currDir.find_last_of("\\");

                if (currDir.size() == pos + 1)
                {
                    currDir.pop_back();

                    pos = currDir.find_last_of("\\");
                }

                prevDir = currDir.substr(pos + 1); // the part after the slash
                currDir = currDir.substr(0, pos);  // the part till the slash
            }
            else
            {
                currDir = brwEntries[currentEntry]->getPath();
            }

            deleteEntries();

            //currDir += "\\";

            composeList();

            remapAndRedraw();
        }
        else if (brwEntries[currentEntry]->getType() == Entry_DiskSelector)
        {
            deleteEntries();

            long drv = GetLogicalDrives();

            long check = 1;
            int num = 1;
            char letter = 0x41;
            char lstr[4] = {0, ':', '\\', 0};

            while(num < 32)
            {
                if(check & drv)
                {
                    lstr[0] = letter;

                    addEntry(new BrwListEntry(lstr, lstr, Entry_DiskDrive));
                }

                letter++;
                check *= 2;
                num++;
            }

            remapAndRedraw();
        }
    }

    parent->handleChildEvent(this, ev);
}

void BrowserList::handleMouseDrag(InputEvent & ev)
{
    if (currentEntry >= 0)
    {
        if (brwEntries[currentEntry]->getType() == Entry_Wave ||
            brwEntries[currentEntry]->getType() == Entry_DLL)
        {
            if(MObject->canDrag(this))
            {
                MObject->dragAdd(brwEntries[currentEntry], ev.mouseX, ev.mouseY);
            }
        }
    }
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

                    if (strcmp(founddata.cFileName, "..") == 0)
                    {
                        continue;
                    }

                    if (!recurs)
                    {
                        EntryType etype = Entry_Directory;

                        //addEntry(new BrwListEntry("[" + fname + "]", scan_path + fname + "\\", etype));
                        addEntry(new BrwListEntry(fname, scan_path + fname + "\\", etype));
                    }

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

                        if(ext == extension || extension == "")
                        {
                            //flist.push_back(fname);

                            EntryType etype = Entry_Default;

                            if(ext == "wav")
                            {
                                etype = Entry_Wave;
                            }
                            else if(ext == "dll")
                            {
                                etype = Entry_DLL;
                            }

                            addEntry(new BrwListEntry(fname, scan_path + fname, etype));

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


