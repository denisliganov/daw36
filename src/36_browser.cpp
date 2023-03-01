

#include "36_globals.h"
#include "36_browser.h"
#include "36_browserlist.h"
#include "36_scroller.h"
#include "36_utils.h"
#include "36_effects.h"
#include "36_vstinstr.h"
#include "36_sampleinstr.h"
#include "36_paramnum.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_note.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_menu.h"
#include "36_audio_dev.h"
#include "36_draw.h"
#include "36.h"
#include "36_button.h"
#include "36_brwentry.h"
#include "36_config.h"
#include "36_textinput.h"
#include "36_listbox.h"

#include "juce_amalgamated.h"


#include <windows.h>
#include <direct.h>



#define WINDEPFILE
#define JUCEDEPFILE



#define BrwScrollerWidth    (18)


//
//

class ScanThread  : public ThreadWithProgressWindow
{
public:

    ScanThread() : ThreadWithProgressWindow (T("Scanning for new plugins..."), false, false)
    {
        setStatusMessage (T("Scanning for new plugins..."));
    }

    ~ScanThread() {}

    void run()
    {
        //setProgress (-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..
        //setStatusMessage (T("Preparing to do some stuff..."));
        //wait (2000);

        MBrowser->devScanProc(this);

        //const int thingsToDo = 10;

        // must check this as often as possible, because this is
        // how we know if the user's pressed 'cancel'
        //if (threadShouldExit())
            //break;

        // this will update the progress bar on the dialog box
        //setProgress (i / (double) thingsToDo);
    }
};


Browser::Browser(std::string dirpath)
{
    path = dirpath;

    currEntry = NULL;
    currIndex = -1;
    ipreview = NULL;
    plugsscanned = false;

    addObject(fileBox = new BrowserList("File browser", WorkDirectory, BrwList_Files));
    addObject(projectsList1 = new BrowserList("Projects", "", BrwList_Projects));
    addObject(internalList1 = new BrowserList("Internal modules", "", BrwList_InternalModules));
    addObject(sampleList1 = new BrowserList("Samples", WorkDirectory, BrwList_WavSamples));
    addObject(plugList = new BrowserList("Plugins", WorkDirectory, BrwList_VST2));
    addObject(vstList1 = new BrowserList("VST1", VST_EXT_PATH_1, BrwList_VST2));
    addObject(vstList2 = new BrowserList("VST2", VST_EXT_PATH_2, BrwList_VST2));
}

void Browser::activateMenuItem(std::string item)
{
    if(item == "Rescan plugins")
    {
        rescanDevices();
    }
    else if(item == "deleteObject project")
    {
        File f(currEntry->path.data());

        f.deleteFile();
    }
    else if(item == "Open project")
    {
        File f(currEntry->path.data());

        MProject.loadProject(&f);
    }
}

void Browser::addSearchDir(std::string dir, bool folders, bool recursive)
{
    directories.push_back(dir);

    auto extpos = dir.find_last_of('\\');

    if(extpos != std::string::npos)
    {
        std::string dirName = dir.substr(extpos + 1);

        if (dirName == "")
        {
            std::string dir1 = dir;
            dir1.pop_back();
            extpos = dir1.find_last_of('\\');
            dirName = dir1.substr(extpos + 1);
        }

        remapAndRedraw();
    }

}

void Browser::clearVstFile()
{
    char   list_path[MAX_PATH_LENGTH] = {0};
    // Check whether file-list of already scanned plugins exists
    sprintf(list_path, "%s%s", ".\\", PLUGIN_LIST_FILENAME);
    DeleteFile(list_path);
}

ContextMenu* Browser::createContextMenu()
{
    if(browsingMode == Browse_Presets)
    {
        return NULL;
    }
    else
    {
        redraw();

        //setCurrentEntry(brwIndex);

        ContextMenu* menu = new ContextMenu(this);

        if(browsingMode == Browse_ExternalDevs)
        {
            menu->addMenuItem("Rescan Plugins");
        }
        else if(browsingMode == Browse_Samples)
        {
            menu->addMenuItem("Load as instrument");
        }
        else if(browsingMode == Browse_Presets && entries[browsingMode].size() > 0)
        {
            BrwEntry* prd = getEntryByIndex(brwIndex);

            if(prd != NULL)
            {
                Device36* module = (Device36*)(prd->dev);

                if(module->isInternal())
                {
                    menu->addMenuItem("deleteObject preset");
                }
            }
        }
        else if(browsingMode == Browse_Projects && entries[browsingMode].size() > 0)
        {
            if(getEntryByIndex(brwIndex) != NULL)
            {
                menu->addMenuItem("Open project");
                menu->addMenuItem("deleteObject project");
            }
        }
        else
        {
            delete menu;

            menu = NULL;
        }

        return menu;
    }
}

void Browser::drawSelf(Graphics& g)
{
    //setc(g, 0.2f);
    //gFillRect(g, x1, y1 + MainLineHeight + 8, x2, y2);

    setc(g, 0.15f);
    fillx(g, 0, MainLineHeight + 1, width, height);

    setc(g, 0.2f);
    fillx(g, 0, 0, width, MainLineHeight);

    setc(g, 0.28f);
    lineH(g, 0, 0, width - 1);

    //gPanelRect(g, x1, y2 - BottomPadHeight + 1, x2, y2);
}

void Browser::devScanProc(ScanThread* thread)
{
    char            temppath[MAX_PATH_LENGTH]         = {0};
    // char          working_directory[MAX_PATH_STRING] = {0};
    // int           drive                              = _getdrive();
    BrwEntry       *entry                              = NULL;
    FILE*           fhandle                            = 0;
    char            mode                               = 0; // 0 - reading+ writing; 1 - writing;
    bool            updlist                     = false;
    char           *envpath                           = NULL;
    char            regpath[MAX_PATH_LENGTH]           = {0};
    DWORD           rPathSize                          = MAX_PATH_LENGTH - 1;
    DWORD           rType                              = REG_SZ;
    HKEY            hResult                            = 0;


    fhandle = rescanFromVstFile();

    //for some reason, file with plugins wasn't created - return immediately

    if(fhandle == 0)
    {
        MessageBox(NULL, "Cannot create/open file", "System error", MB_OK | MB_ICONERROR);
        return ;
    }

    // scan local plugin folder

    //_getdcwd(drive, working_directory, MAX_PATH_STRING - 1 );
    //sprintf_s(temppath, MAX_PATH_STRING, "%s%s", szWorkingDirectory, LOCAL_PLUGIN_FOLDER);

    sprintf_s(temppath, MAX_PATH_LENGTH, "%s", LOCAL_PLUGIN_FOLDER);

    scanDirForDevs(temppath, mode, fhandle, thread);

    // Check whether environment has VST_PATH variable set

    envpath = getenv( "VST_PATH" );

    if (envpath != NULL)
    {
        memset(temppath, 0, sizeof(temppath));
        sprintf(temppath,"%s%s", envpath, "\\");

        if ( (_stricmp(temppath, VST_EXT_PATH_1) != 0) && (_stricmp(temppath, VST_EXT_PATH_2) != 0) )
        {
            scanDirForDevs(temppath, mode, fhandle, thread);
        }
    }

    /* Scan special Steinberg's folder for plugins */
    // SHGetValue(HKEY_LOCAL_MACHINE, "Software\\VST","VSTPluginsPath",&rType,&regpath, &rPathSize);
    // RegGetValue(HKEY_LOCAL_MACHINE, "Software\\VST", "VSTPluginsPath",RRF_RT_ANY,NULL, &regpath, &rPathSize);

    RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\VST", &hResult);

    if (hResult != 0)
    {
        RegQueryValueEx(hResult, "VSTPluginsPath", NULL, NULL, (LPBYTE)&regpath, &rPathSize);
        RegCloseKey(hResult);
    }

    if ( (rPathSize != 0) && (regpath[0] != 0) )
    {
        memset(temppath, 0, sizeof(temppath));
        sprintf(temppath,"%s%s", regpath, "\\");

        if ( (_stricmp(temppath, VST_EXT_PATH_1) != 0) && (_stricmp(temppath, VST_EXT_PATH_2) != 0) )
        {
            scanDirForDevs(temppath, mode, fhandle, thread);
        }
    }

    // Last place to check for VST

    memset(temppath, 0, sizeof(temppath));

    strcpy(temppath, VST_EXT_PATH_1);

    scanDirForDevs(temppath, mode, fhandle, thread);

    memset(temppath, 0, sizeof(temppath));

    strcpy(temppath, VST_EXT_PATH_2);

    scanDirForDevs(temppath, mode, fhandle, thread);

    fclose(fhandle);

    fhandle = 0;

    _chdir(WorkDirectory);

    // Phase 2: go through the list and remove old plugins which don't exist

start:

    for(auto entry : entries[browsingMode])
    {
        if(entry->isExternal())
        {
            if ((fhandle = fopen(entry->path.data(), "rb")) == 0 )
            {
                removeEntry(entry);

                updlist = true;

                goto start;
            }
            else
            {
                fclose(fhandle);
                fhandle = 0;
            }
        }
    }

    // Now re-write list-file with new list

    if (updlist)
    {
        fhandle = fopen(PLUGIN_LIST_FILENAME, "wb+");

        if (fhandle != NULL)
        {
            for(BrwEntry* entry : entries[browsingMode])
            {
                if (entry->path.rfind("internal://") == std::string::npos)
                {
                     fwrite(entry, sizeof(BrwEntry),1, fhandle);
                }
            }

            fclose(fhandle);
        }
    }
}


BrwEntry* Browser::getEntryByIndex(int index)
{
    for(BrwEntry* be : entries[browsingMode])
    {
        if(be->listIndex == index)
        {
            return be;
        }
    }

    return NULL;
}

BrwEntry* Browser::getEntryByPath(char* path)
{
    for(auto entry : entries[browsingMode])
    {
        if (entry->path == path)
        {
            return entry;
        }
    }

    return NULL;
}

void Browser::handleChildEvent(Gobj * obj, InputEvent& ev)
{

}

void Browser::handleMouseUp(InputEvent& ev)
{

}

void Browser::handleMouseDown(InputEvent& ev)
{

}

void Browser::handleMouseWheel(InputEvent& ev)
{
    float offsdelta = -(float)ev.wheelDelta*(BrwEntryHeight*2 + 3);

    //setVoffs(getVoffs() + offsdelta);
}

void Browser::handleMouseDrag(InputEvent& ev)
{
    
}

bool Browser::isFileMode()
{
    return (browsingMode == Browse_Samples || browsingMode == Browse_Projects);
}

bool Browser::isDevMode()
{
    return (browsingMode == Browse_InternalDevs || browsingMode == Browse_ExternalDevs);
}

void Browser::prreviewSample(bool down)
{
    if(down)
    {
        BrwEntry* entry = getEntryByIndex(brwIndex);

        if(entry->ftype == FType_Wave)
        {
            ipreview = (Instrument*)MInstrPanel->addSample((char*)entry->path.data(), true);

            if(ipreview != NULL)
            {
                ipreview->selfNote->preview();
            }
        }
    }
    else
    {
        if(ipreview != NULL)
        {
            MAudio->releaseAllPreviews();

            MInstrPanel->deleteInstrument(ipreview);

            ipreview = NULL;
        }
    }
}

void Browser::remap()
{
    int cx = 0;
    int cy = 0;
    int cw = 200;
    float lstHeight = (float)(height - cy - 1);

    int xLists = cx;

    int fileBrwWidth = 300;

    confine(xLists, cy, width - fileBrwWidth, height); // no args -> reset bounds

    setObjSpacing(2);

    putStart(xLists, cy);

    putRight(internalList1, 250, lstHeight);
    putRight(sampleList1, 250, lstHeight);
    putRight(plugList, 250, lstHeight);
    putRight(vstList1, 250, lstHeight);
    putRight(vstList2, 250, lstHeight);
    putRight(projectsList1, 250, lstHeight);


    //for (ListBoxx* lb : listBoxes)
    //    putRight(lb, 200, lstHeight);

    confine();

    fileBox->setCoords1(width - fileBrwWidth + 2, cy, fileBrwWidth - 4, lstHeight);
}

void Browser::removeEntry(BrwEntry * entry)
{
    entries[browsingMode].remove(entry);

    deleteObject(entry);
}

void Browser::rescanDevices( )
{
    //Stop_PortAudio();

    //clearVstFile();

    ScanThread PlugScan;

    PlugScan.runThread();

    //Start_PortAudio();
}

void Browser::removeSearchDir(std::string dir)
{
    directories.remove(dir);
}

FILE* Browser::rescanFromVstFile()
{
    FILE*       fhandle                            = 0;
    char        listpath[MAX_PATH_LENGTH]         = {0};
    char        mode                               = 0; // 0 - reading+ writing; 1 - writing;

    BrwEntry    listentry ;
    BrwEntry   *tmpentry                         = NULL;

    // Check whether file-list of already scanned plugins exists
    sprintf(listpath, "%s%s", ".\\", PLUGIN_LIST_FILENAME);

    if ((fhandle = fopen(listpath, "rb+")) == 0) //opens for reading and writing
    {
        //Opens for writing
        fhandle = fopen(listpath, "wb+");

        mode = 1;
    }
    else if (fhandle != 0)
    {
    /*
        // Let's read all plugin records from the file
        while (fread(&listentry, sizeof(BrwEntry), 1, fhandle) != 0)
        {
            tmpentry = new BrwEntry;
            memcpy(tmpentry, &listentry, sizeof(BrwEntry));
            addEntry(tmpentry);
        }
*/

        //initialize file and variable for further usage
        tmpentry = NULL;

        fseek(fhandle, 0, SEEK_END);
    }

    return fhandle;
}


void Browser::scanDirForDevs(char *path, char mode, FILE* fhandle, ScanThread* thread)
{
    char temppath[MAX_PATH_LENGTH*5] = {0};
    sprintf(temppath, "%s%s", path, "*.dll\0");

    WIN32_FIND_DATA founddata = {0};

    HANDLE shandle = FindFirstFile(temppath, &founddata);

    if(shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            char    filename[MAX_PATH_LENGTH] = {0};
            char    name[MAX_NAME_LENGTH] = {0};

            sprintf(filename, "%s%s", path, founddata.cFileName);
            memset(name, 0, MAX_NAME_LENGTH* sizeof(char));

            if(mode == 0)
            {
                if (getEntryByPath(filename) != NULL)
                {
                    continue;
                }
            }

            //thread->setStatusMessage(thread->getStatusMessage + String(strrchr(filename, '\\') + 1);

            thread->setStatusMessage("Scanning: " + String(strrchr(filename, '\\') + 1));

            DevClass dclass;
            std::string dname;
            std::string dpath    = filename;

            bool isgen = false;

            if (VstHost->checkModule(filename, &isgen, name) == true)
            {
                if ((name != NULL) && (name[0] != 0))
                {
                    dname = name;
                }
                else
                {
                    char * name = NULL;
                    name = strrchr(filename, '\\');
                    dname = ++name;
                }

                if(isgen)
                {
                    dclass = DevClass_GenVst;
                }
                else
                {
                    dclass = DevClass_EffVst;
                }
            }
            else // Something wrong with module, probably invalid
            {
                char * name = strrchr(filename, '\\');

                if (name != NULL)
                {
                    dname = ++name;
                }
                else
                {
                    dname = "stub";
                }

                dclass = DevClass_Invalid;
            }

            char * nm = strrchr(filename, '\\');
            
            if (nm != NULL)
            {
                dname = ++nm;
            }
            else
            {
                dname = "stub";
            }
            
            dclass = DevClass_Default;

            //BrwEntry* dentry = addEntry(dclass, dname, dpath, "vst");

            // Store in effect list file

            //fwrite(dentry, sizeof(BrwEntry), 1, fhandle);
        }
        while (FindNextFile(shandle, &founddata));

        FindClose(shandle);
    }

    sprintf(temppath, "%s%s", path, "*.*\0");

    shandle = FindFirstFile(temppath, &founddata);

    if(shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(founddata.cFileName, ".") != 0 && strcmp(founddata.cFileName, "..") != 0)
                {
                    sprintf(temppath, "%s%s%s", path, founddata.cFileName, "\\");

                    scanDirForDevs(temppath, mode, fhandle, thread);
                }
            }
        }
        while (FindNextFile(shandle, &founddata));

        FindClose(shandle);
    }
}

void  Browser::scanDirForFiles(std::string scan_path, std::string extension, bool recurs, std::vector<std::string>& flist)
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
                    if(recurs)
                    {
                        char dirpath[MAX_PATH_LENGTH];

                        if(strcmp(founddata.cFileName, ".") != 0 && strcmp(founddata.cFileName, "..") != 0)
                        {
                            sprintf(dirpath, "%s%s%s", (char*)scan_path.data(), founddata.cFileName, "\\");

                            scanDirForFiles(dirpath, extension, recurs, flist);
                        }
                    }
                }
                else 
                {
                    std::string fname = founddata.cFileName;
                    auto const extpos = fname.find_last_of('.');

                    if(extpos != std::string::npos)
                    {
                        std::string ext = fname.substr(extpos + 1);

                        ToLowerCase((char*)ext.data());

                        if(ext == extension || extension == "")
                        {
                            flist.push_back(fname);

                            BrwEntry* fileEntry = new BrwEntry;

                            fileEntry->setObjName(fname);

                            fileEntry->path = scan_path + fname;
                            fileEntry->size = (founddata.nFileSizeHigh * (MAXDWORD)) + founddata.nFileSizeLow;
                            fileEntry->attrs = founddata.dwFileAttributes;
                            fileEntry->ftype = GetFileTypeByExtension(extension);
                            fileEntry->listIndex = -1;

                            addObject(fileEntry);

                            entries[browsingMode].push_back(fileEntry);
                        }
                    }
                }
            }
        }while(FindNextFile(fhandle, &founddata));

        FindClose(fhandle);
    }
}

void Browser::updateEntries()
{
    WIN32_FIND_DATA founddata = {0};
    int             cur_index = 0;

    SetCurrentDirectory(WorkDirectory);

    if(browsingMode == Browse_ExternalDevs)
    {
        File vstfile(PLUGIN_LIST_FILENAME);
        bool vstfileexists = vstfile.exists();

        if(plugsscanned == false)
        {
            plugsscanned = true;

            if(vstfileexists)
            {
                rescanFromVstFile();
            }
            else
            {
                rescanDevices();
            }
         }
    }

    // if(currDir != NULL)
    //    ::SetCurrentDirectory(currDir);
}

