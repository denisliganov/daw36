

//
// Main manager module
//

#include "stdafx.h"
//#include "resource.h"



#include "36.h"
#include "36_pattern.h"
#include "36_instrpanel.h"
#include "36_ctrlpanel.h"
#include "36_helper.h"
#include "36_edit.h"
#include "36_grid.h"
#include "36_browser.h"
#include "36_history.h"
#include "36_mixer.h"
#include "36_transport.h"
#include "36_lane.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_config.h"
#include "36_button.h"
#include "36_auxkeys.h"
#include "36_audio_dev.h"
#include "36_macros.h"
#include "36_dragndrop.h"


#include <list>



///////////////
/////////////////////
//////
///
//
/// G l o b a l s


int                 Octave;
int                 BaseNote;

bool                GPlaying;
bool                GRecOn;

XmlElement*         xmlAudioSettings = NULL;


int                 AuxCtrlWidth;
int                 AuxHeight;
int                 BottomPadHeight;
int                 CtrlPanelHeight;
int                 FxPanelMaxWidth;
int                 FxPanelScrollerWidth;
int                 FxPanelBottomHeight;
int                 GridScrollWidth;
int                 InstrScrollerWidth;
int                 InstrWidth;
int                 InstrHeight;
int                 InstrControlWidth;
int                 LeftGap;
int                 LineNumWidth;
int                 MainLineHeight;
int                 MenuButtonsSpacing;
int                 MenuGroupsSpacing;
int                 MixerHeight;
int                 MixerTopHeight;
int                 MixChanWidth;
int                 MixChannelPadHeight;
int                 PanelGap;


Instrument*         SoloInstr;
MixChannel*         SoloMixChannel;

Pattern*            MPattern;
ControlPanel*       MCtrllPanel;
LanePanel*          MLanePanel;
InstrPanel*         MInstrPanel;
Browser*            MBrowser;
Mixer*              MMixer;
MainEdit*           MEdit;
Audio36*            MAudio;
EditHistory*        MHistory;
Transport*          MTransp;
Grid*               MGrid;
HelperPanel*        MHelperPanel;
KeyHandler*         MKeys;
MainWinObject*      MObject;
TextCursor*         MCursor;

bool                InitComplete = false;


MainWinObject::MainWinObject()
{
    width = DAW_DEFAULT_WINDOW_WIDTH;
    height = DAW_DEFAULT_WINDOW_HEIGHT;

    SoloInstr = NULL;
    SoloMixChannel = NULL;

    AuxCtrlWidth =          0;
    AuxHeight =             40;
    CtrlPanelHeight =       50;
    GridScrollWidth =       16;
    InstrControlWidth =     220;
    InstrScrollerWidth =    16;
    InstrHeight =           32;
    FxPanelMaxWidth =       380;
    FxPanelScrollerWidth =  10;
    FxPanelBottomHeight =   130;
    LeftGap =               26;
    LineNumWidth =          18;
    MixerHeight =           500;
    MixerTopHeight =        16;
    MixChanWidth =          70;
    MixChannelPadHeight =   80;
    MainLineHeight =        30;
    PanelGap =              1;
    MenuButtonsSpacing =    3;
    MenuGroupsSpacing =     20;

    mainX1 = InstrControlWidth;
    mainX2 = width - 1;
    mainY1 = MainLineHeight + CtrlPanelHeight + 1;    // _MGrid upper coordinate
    mainY2 = height - AuxHeight;

    addObject(MCtrllPanel = new ControlPanel);
    addObject(MEdit = new MainEdit);
    addObject(MHelperPanel = new HelperPanel);
    addObject(MLanePanel = new LanePanel(MEdit->grid));
    addObject(MInstrPanel = new InstrPanel(MMixer = new Mixer));
    addObject(MBrowser = new Browser(WorkDirectory));

    MMixer->setEnable(false);
    MHelperPanel->setEnable(true);
    MInstrPanel->setEnable(true);
    MBrowser->setEnable(false);
    MCtrllPanel->setEnable(true);
    MEdit->setEnable(true);

    MGrid->grabTextCursor(100, 10);

    MLanePanel->setCoords1(mainX1, height - AuxHeight, mainX2 - mainX1 + 1, AuxHeight);
    MMixer->setCoords1(mainX1, height - MixerHeight, mainX2 - mainX1 + 1, MixerHeight);

    MLanePanel->setEnable(false);
    currAuxPanel = NULL;
    //MLanePanel->setType(Lane_Keys);

    setMode(DragMode_Default);

    scrollTimer = new ScrollTimer();

    InputEvent ev = {};
}

MainWinObject::~MainWinObject()
{
    delete scrollTimer;
}

void MainWinObject::handleWindowResize(int wx, int wh)
{
    setCoords1(0, 0, wx, wh);

    mainX2 = width - AuxCtrlWidth - 1;

    remap();
}

void MainWinObject::remap()
{
    if (!InitComplete)
    {
        return;
    }

    if(currAuxPanel != NULL)
    {
        mainY2 = height - currAuxPanel->getH();
    }
    else if (currAuxPanel == NULL)
    {
        mainY2 = height - 1;
    }

    MCtrllPanel->setCoords2(0, mainY1 - MainLineHeight - CtrlPanelHeight - 1, mainX2, mainY1 - MainLineHeight - 1);

    if(MLanePanel->isON())
    {
        MLanePanel->setCoords2(mainX1 + 1, mainY2, mainX2, height - 1);
    }

    MInstrPanel->setCoords2(0, mainY1 - MainLineHeight + PanelGap, mainX1 - 1, mainY2 - 1);

    if (MEdit->isON())
    {
        MEdit->setCoords2(mainX1 + 2, mainY1 - MainLineHeight + PanelGap, mainX2, mainY2 - 1);
    }
    else if (MBrowser->isON())
    {
        MBrowser->setCoords2(mainX1 + 2, mainY1 - MainLineHeight + PanelGap, mainX2, mainY2 - 1);
    }

    if(AuxCtrlWidth > 0)
    {
        MHelperPanel->setEnable(true);
        MHelperPanel->setCoords1(mainX2 + PanelGap + 1, 0, AuxCtrlWidth, height - 1);
    }
    else
    {
        MHelperPanel->setEnable(false);
    }

    if(!MProject.isLoading() && MGrid != NULL && MWindow != NULL)
    {
        MWindow->setResizeLimits(MGrid->getX1() + 7, MGrid->getY1() + AuxHeight + MainLineHeight + 8, 32768, 32768);
    }

    
    if(mainX1 <= InstrControlWidth)
    {
        if (!MInstrPanel->btHideFX->isPressed())
        {
            MInstrPanel->btHideFX->setEnable(false);
            MInstrPanel->btShowFX->setEnable(true);
        }
    }
    else
    {
        if (!MInstrPanel->btShowFX->isPressed())
        {
            MInstrPanel->btShowFX->setEnable(false);
            MInstrPanel->btHideFX->setEnable(true);
        }
    }

    redraw();
}

void MainWinObject::drawSelf(Graphics& g)
{
    g.fillAll(Colours::black);

    if(mainX1 - PanelGap - 1 > 0)
    {
    //    gPanelRect(g, 0, height - BottomPadHeight + PanelGap, mainX1 - PanelGap, height - 1);
    }
}

void MainWinObject::updCursorImage(unsigned flags)
{
    if (getMode() == DragMode_DragX0 || getMode() == DragMode_DragX1 || getMode() == DragMode_DragX2)
    {
        setCursor(Cursor_LeftRight);
    }
    else if (getMode() == DragMode_DragY1 || getMode() == DragMode_DragY2)
    {
        setCursor(Cursor_UpDown);
    }
    else
    {
        setCursor(Cursor_Arrow);
    }
}

void MainWinObject::updActiveObject(InputEvent& ev)
{
    // check objects

    WinObject::updActiveObject(ev);

    if (ev.mouseX <= mainX1 + 5 && ev.mouseX >= mainX1 - 5 && ev.mouseY >= mainY1 - MainLineHeight && ev.mouseY <= mainY1)
    {
        // Deprecated this
        //setMode(DragMode_DragX1);
    }
    else if (ev.mouseX > mainX1 - InstrControlWidth - 4 && ev.mouseX < mainX1 - InstrControlWidth + 4 && ev.mouseY >= mainY1 - MainLineHeight && ev.mouseY <= mainY1)
    {
        //setMode(DragMode_DragX0);
    }
    else if (ev.mouseX > mainX2 - 2 && ev.mouseX < mainX2 + 4 && ev.mouseY >= 0 && ev.mouseY <= mainY1)
    {
        setMode(DragMode_DragX2);
    }
    /*
    else if (ev.mouseY <= mainY1 - MainLineHeight + 3 && ev.mouseY >= mainY1 - MainLineHeight - 3 && ev.mouseX < mainX2)
    {
        setMode(DragMode_DragY1);
    }*/
    else if (ev.mouseY <= mainY2 + 5 && ev.mouseY >= mainY2 - 5 && ev.mouseX > mainX1 && ev.mouseX < mainX2)
    {
        setMode(DragMode_DragY2);
    }
    else
    {
        setMode(DragMode_Default);
    }

    if(mode != DragMode_Default)
    {
        activeObj = NULL;
    }

    updCursorImage(ev.keyFlags);
}

void MainWinObject::handleMouseUp(InputEvent& ev)
{
    WinObject::handleMouseUp(ev);

    MAudio->releaseAllPreviews();

    MHistory->newGroup();
}

void MainWinObject::handleMouseDrag(InputEvent& ev)
{
    WinObject::handleMouseDrag(ev);

    if(!drag->isActive())
    {
        if(getMode() == DragMode_DragX0)
        {
            if(abs(ev.mouseX) < LeftGap)
            {
                setMainX1(InstrControlWidth);
            }
            else if(ev.mouseX < FxPanelMaxWidth)
            {
                setMainX1(ev.mouseX + InstrControlWidth);
            }
            else
            {
                setMainX1(FxPanelMaxWidth + InstrControlWidth);
            }
        }
        else if(getMode() == DragMode_DragX1)
        {
            int max = FxPanelMaxWidth + InstrControlWidth;

            if(ev.mouseX < max)
            {
                if(abs(ev.mouseX - (InstrControlWidth)) < 20)
                {
                    setMainX1(InstrControlWidth);
                }
                else
                {
                    setMainX1(ev.mouseX);
                }
            }
            else
            {
                setMainX1(max);
            }
        }
        else if(getMode() == DragMode_DragX2)
        {
            if(abs(ev.mouseX) > width - 300)
            {
                setMainX2(ev.mouseX);

                AuxCtrlWidth = width - mainX2;
            }
            else
            {
                AuxCtrlWidth = 300;

                setMainX2(width - AuxCtrlWidth);
            }
        }
        else if(getMode() == DragMode_DragY1)
        {
            if(ev.mouseY < (CtrlPanelHeight))
            {
                setMainY1(ev.mouseY + MainLineHeight + 1);
            }
            else
            {
                setMainY1(CtrlPanelHeight + MainLineHeight + 1);
            }

            if(height - mainY1 - MainLineHeight - 1 < AuxHeight)
            {
                AuxHeight = height - mainY1 - MainLineHeight - 1;

                setMainY2(height - AuxHeight);
            }
        }
        else if(getMode() == DragMode_DragY2)
        {
            if(ev.mouseY > MGrid->getY1())
            {
                if(ev.mouseY < height - 30)
                {
                    setMainY2(ev.mouseY);
                }
                else
                {
                    setMainY2(height - 30);
                }
            }
            else
            {
                setMainY2(MGrid->getY1());
            }
        }
    }
}

void MainWinObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    ///
}

void MainWinObject::setMainX1(int main_x1)
{
    mainX1 = main_x1;

    if(mainX2 < mainX1 + LeftGap)
    {
        mainX2 = mainX1 + LeftGap;
    }

    remap();
}

void MainWinObject::setMainX2(int main_x2)
{
    mainX2 = main_x2;

    LIMIT(mainX2, 0, width - 1);

    remap();
}

void MainWinObject::setMainY1(int main_y1)
{
    mainY1 = main_y1;

    remap();
}

void MainWinObject::setMainY2(int main_y2)
{
    mainY2 = main_y2;

    if(currAuxPanel != NULL)
    {
        currAuxPanel->setCoords1(-1, -1, width, height - mainY2);
    }

    remap();
}

void MainWinObject::switchBrowser(bool on)
{
    if (on)
    {
        MEdit->setEnable(false);
        MBrowser->setEnable(true);

        remapAndRedraw();
    }
    else
    {
        MEdit->setEnable(true);
        MBrowser->setEnable(false);

        remapAndRedraw();
    }
}

void MainWinObject::handleClose()
{
    // When the user presses the close button, we'll tell the app to quit. This 
    // window will be deleted by the shutdown() method

    if(!MProject.askAndSave())
    {
        MProject.saveSettings();

        JUCEApplication::quit();
    }
}

