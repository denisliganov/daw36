

#include "36_init.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_history.h"
#include "36_browser.h"
#include "36_instrpanel.h"
#include "36_draw.h"
#include "36_transport.h"
#include "36_audio_dev.h"
#include "36.h"
#include "36_window.h"
#include "36_utils.h"
#include "36_vst.h"
#include "36_configwin.h"
#include "36_renderer.h"
#include "36_grid.h"
#include "36_dragndrop.h"


MainWindow*         MWindow = NULL;
SubWindow*          ConfigWindow = NULL;
SubWindow*          BrowserWindow = NULL;

// Various mouse cursor shapes
MouseCursor*        cursCopy;
MouseCursor*        cursClone;
MouseCursor*        cursSlide;
MouseCursor*        cursBrush;
MouseCursor*        cursSelect;

// _MKeyboard types
HCURSOR             VSCursor;           // vertical resize
HCURSOR             HSCursor;           // horizontal resize
HCURSOR             ArrowCursor;        // plain arrow


// Main window handle
HWND                WinHWND;

// Full path to program's working directory
char *              WorkDirectory;

std::string         WorkDir;


void        GetCurrentDir();
void        LoadCursorImages();
void        StartEditor();
void        LoadDefaultInstruments();

void ExitProgram()
{
    MProject.releaseAllOnExit();

    delete MAudio;

    // delete MDragDrop;

    // delete MObject;        // Not needed, Juce will delete content component

    if (MWindow != NULL)
    {
        delete MWindow;
    }

    JUCEApplication::quit();
}

void InitializeAndStartProgram()
{
    //Splash_Create();

    GetCurrentDir();

    gInitGraphics();

    LoadCursorImages();

    InitWavetables();

    MKeys = new KeyHandler();

    MTransp = new Transport(80, 4, 4);

    MAudio = new Audio36(DEFAULT_SAMPLE_RATE);

    MObject = new MainWinObject();

    MWindow = new MainWindow(MObject);

    WinHWND = (HWND)MWindow->getWindowHandle();

    VstHost = new Vst2Host(WinHWND);

    MTransp->propagateChanges();

    InitComplete = true;

    //MObject->resized();

    //MObject->addHighlight(MDragDrop = new DragAndDrop());

    MInstrPanel->showFX();

    // Splash_Delete();

    StartEditor();
}

void HandleCommandLine(const String& arg_str)
{
    // Remove quotes first, as they can prevent from loading

    char* fn = (char*)malloc(arg_str.length());
    arg_str.copyToBuffer(fn, arg_str.length());

    char* k = fn;
    char* a = strchr(fn, '\"');

    while(a != NULL)
    {
        if(a == fn)  k += 1;
    
       *a = 0;
        a = strchr(k, '\"');
    }

    File f(k);

    if(f.exists())
    {
        MProject.loadProject(&f);
    }
}

void HandleAnotherInstance(const String& command_line)
{
    MWindow->showAlertBox("Restriction warning");

    File f(command_line);

    if(f.exists())
    {
        MProject.loadProject(&f);
    }
}

void HandleSystremQuitRequest()
{
    MProject.askAndSave();
    MProject.saveSettings();
}

void StartEditor()
{
    Octave = 5;
    BaseNote = 60;

    GPlaying = false;
    Render_isWorking = false;
    GRecOn = false;

    MHistory = new EditHistory();

    LoadDefaultInstruments();

    MProject.init();
}

void InitDirectories()
{
    if(WorkDirectory[0] >= 0x61)
    {
        WorkDirectory[0] -= 0x20;
    }

    SetCurrentDirectory(WorkDirectory);

    // Create required subdirectories if they don't exist

    File frendered(String((const char*)WorkDirectory) + T("Rendered"));
    File fprojects(String((const char*)WorkDirectory) + T("Projects"));
    File fplugins(String((const char*)WorkDirectory) + T("Plugins"));
    File fpresets(String((const char*)WorkDirectory) + T("Presets"));
    File fsamples(String((const char*)WorkDirectory) + T("Samples"));

    if(!frendered.exists())
    {
        frendered.createDirectory();
    }

    if(!fprojects.exists())
    {
        fprojects.createDirectory();
    }

    if(!fplugins.exists())
    {
        fplugins.createDirectory();
    }

    if(!fpresets.exists())
    {
        fpresets.createDirectory();
    }

    if(!fsamples.exists())
    {
        fsamples.createDirectory();
    }
}

void GetCurrentDir()
{
    //First, lets check how long is our working path

    int length = ::GetCurrentDirectory(0, NULL);

    WorkDirectory = NULL;

    if (length != 0)
    {
        WorkDirectory = (char*)malloc(length + 1);
    }

    if (WorkDirectory != NULL)
    {
        ::GetCurrentDirectory(length, WorkDirectory);
    }

    strcat(WorkDirectory, "\\");
}

void GetStartupDir()
{
    //ExtractFilePath(Application->ExeName);
    //GetFullPathName(szWorkingDirectory, );

    char name[2222];
    GetModuleFileName(NULL, name, 2222);

    WorkDirectory = (char*)malloc(strlen(name));

    char* sc;
    char* last_sc;

    sc = last_sc = name;

    while(sc != NULL)
    {
        sc = strchr(sc, '\\');

        if(sc != NULL)
        {
            last_sc = sc;
            sc++;
        }
    }

    if(last_sc != name)
    {
        last_sc++;
       *last_sc = 0;
    }

    strcpy(WorkDirectory, name);
}



void LoadDefaultInstruments()
{
    MInstrPanel->addVst("Plugins\\mda Piano.dll", NULL);
    MInstrPanel->addVst("Plugins\\mda DX10.dll", NULL);
    MInstrPanel->addSample("Samples\\Kicked.wav");

    MInstrPanel->addSample("Samples\\808.wav");
    MInstrPanel->addSample("Samples\\Kik 2.wav");
    MInstrPanel->addSample("Samples\\Clave.wav");
    MInstrPanel->addSample("Samples\\Timbale 2.wav");
    MInstrPanel->addSample("Samples\\Closed Hat.wav");
    MInstrPanel->addSample("Samples\\Dark Hat.wav");
    MInstrPanel->addSample("Samples\\Jazzy Hat.wav");
    MInstrPanel->addSample("Samples\\snar_04m1.wav");
    MInstrPanel->addSample("Samples\\Crash 2.wav");
    MInstrPanel->addSample("Samples\\SBALUN.wav");
    MInstrPanel->addSample("Samples\\SarodC5.wav");
    MInstrPanel->addSample("Samples\\SBAP.wav");
    MInstrPanel->addSample("Samples\\Snare Tail.wav");
    MInstrPanel->addSample("Samples\\Zheng_64.wav");
    MInstrPanel->addSample("Samples\\test1.wav");
    MInstrPanel->addSample("Samples\\SBA4.wav");
    MInstrPanel->addSample("Samples\\test.wav");

    // todo: dummy instrument


    /*
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);
    MInstrPanel->addInstrument(NULL, NULL);

    MInstrPanel->addInstrument(NULL, NULL);*/

    /*
    MInstrPanel->addSample("Samples\\Clap.wav");
    MInstrPanel->addSample("Samples\\Rasta.wav");
    MInstrPanel->addSample("Samples\\Pipa_66.wav");
    MInstrPanel->addSample("Samples\\SarodC5.wav");
    MInstrPanel->addSample("Samples\\Gtr Hi Chuck.wav");
    MInstrPanel->addSample("Samples\\starpad.wav");
    MInstrPanel->addSample("Samples\\Long OpnClsd Hat.wav");
    MInstrPanel->addSample("Samples\\Nother Rim.wav");
    MInstrPanel->addSample("Samples\\Rim 2.wav");
    MInstrPanel->addSample("Samples\\Kicked.wav");
    MInstrPanel->addSample("Samples\\SBAF.wav");
    MInstrPanel->addSample("Samples\\Blockhead.wav");
    MInstrPanel->addSample("Samples\\Horn Hit.wav");
    MInstrPanel->addSample("Samples\\Timbale 2.wav");
    MInstrPanel->addSample("Samples\\Closed Hat.wav");
    MInstrPanel->addSample("Samples\\Dark Hat.wav");*/

    /*
    MInstrPanel->addInstrument(NULL);
    MInstrPanel->addInstrument(NULL);
    MInstrPanel->addInstrument(NULL);*/
    MInstrPanel->addInstrument(NULL, true);

    MInstrPanel->setCurrInstr(0);


   // MInstrPanel->setCurrInstr(MInstrPanel->getInstrs()[0]);

    //MInstrPanel->addSample("Samples\\Cymatics - Light Free Sample Pack\\Bonus Samples\\Drum Loops\\Cymatics - San Holo Free Full Drum Loop 2 - 120 BPM.wav");

/*
    MInstrPanel->addSample("Samples\\Jazzy Hat.wav");
    MInstrPanel->addSample("Samples\\snar_04m1.wav");
    MInstrPanel->addSample("Samples\\Crash 2.wav");
    MInstrPanel->addSample("Samples\\Rim 2.wav");
    MInstrPanel->addSample("Samples\\Kicked.wav");
    MInstrPanel->addSample("Samples\\808.wav");
    MInstrPanel->addSample("Samples\\Kik 2.wav");
*/
//    _MInstrPanel->addSample("Samples\\Snare Tail.wav");
//    _MInstrPanel->addSample("Samples\\CircleBrsh 2.wav");
 //   _MInstrPanel->addSample("Samples\\DABACLOS.wav");
 //   _MInstrPanel->addSample("Samples\\Pipa_66.wav");
 //   _MInstrPanel->addSample("Samples\\SBA4.wav");
//    _MInstrPanel->addSample("Samples\\SBACLOS.wav");
//    _MInstrPanel->addSample("Samples\\SBAF.wav");
//    _MInstrPanel->addSample("Samples\\SBALUN.wav");
//    _MInstrPanel->addSample("Samples\\SBAP.wav");

    //_MInstrPanel->addSample("Samples\\Zheng_64.wav");
    //_MInstrPanel->addSample("Samples\\Zheng_76.wav");
    //_MInstrPanel->addSample("Samples\\DABA.wav");
    //_MInstrPanel->addSample("Samples\\Horn Hit.wav");
    //_MInstrPanel->addSample("Samples\\Siren Loop.wav");

    // Set first as current

    //if (MInstrPanel->instrs.size() > 0)
    //{
    //    MInstrPanel->setCurrInstr(MInstrPanel->instrs.front());
    //}

    //MInstrPanel->setCurrInstr(NULL);
}

void LoadCursorImages()
{
    // Initialize arrow cursors

    cursCopy = new MouseCursor(*img_curscopy, 0, 0);
    cursClone = new MouseCursor(*img_cursclone, 0, 0);
    cursSlide = new MouseCursor(*img_cursslide, 0, 0);
    cursBrush = new MouseCursor(*img_cursbrush, 0, 0);
    cursSelect = new MouseCursor(*img_cursselect, 0, 0);

    VSCursor    = LoadCursor(NULL, IDC_SIZENS);
    HSCursor    = LoadCursor(NULL, IDC_SIZEWE);
    ArrowCursor = LoadCursor(NULL, IDC_ARROW);
}


void ToggleConfigWindow()
{
    
}

