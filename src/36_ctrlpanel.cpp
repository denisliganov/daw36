

#include "36.h"
#include "36_system.h"
#include "36_ctrlpanel.h"
#include "36_configwin.h"
#include "36_renderwin.h"
#include "36_scroller.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_menu.h"
#include "36_button.h"
#include "36_sampleinstr.h"
#include "36_project.h"
#include "36_pattern.h"
#include "36_transport.h"
#include "36_audio_dev.h"
#include "36_draw.h"
#include "36_vu.h"
#include "36_edit.h"
#include "36_playhead.h"
#include "36_init.h"
#include "36_alertbox.h"
#include "36_numbox.h"
#include "36_snapmenu.h"
#include "36_browser.h"
#include "36_dropbox.h"
#include "36_instrpanel.h"




ControlPanel::ControlPanel()
{
    /*
    xButtons = 0;
    int yMenu = 4;
    int buttonWidth = 30;
    int buttonHeight = 30;

    addObject(fileMenu = new DropBox(0, "FILE"), xButtons, yMenu);
    fileMenu->addItem("New");
    fileMenu->addItem("Open");
    fileMenu->addItem("");
    fileMenu->addItem("Save");
    fileMenu->addItem("Save As");
    fileMenu->addItem("");
    fileMenu->addItem("Load Project ");
    //fileMenu->addItem("Render to WAV or MP3");
    fileMenu->addItem("");
    fileMenu->addItem("Render");
    fileMenu->addItem("");
    fileMenu->addItem("Exit");
    xButtons += fileMenu->getW() + MenuButtonsSpacing;
    addObject(editMenu = new DropBox(0, "EDIT"), xButtons, yMenu);
    editMenu->addItem("Undo");
    editMenu->addItem("Redo");
    editMenu->addItem("");
    editMenu->addItem("Delete Song");
    editMenu->addItem("");
    editMenu->addItem("Settings");
    xButtons += editMenu->getW() + MenuButtonsSpacing;
    addObject(helpMenu = new DropBox(0, "HELP"), xButtons, yMenu);
    helpMenu->addItem("Keys Reference");
    helpMenu->addItem("");
    helpMenu->addItem("About M");

    xButtons += helpMenu->getW() + MenuGroupsSpacing;

    addObject(btConfig = new Button36(true), xButtons, yMenu, 30, 30, "bt.cfg");
    btConfig->setLedType(true);
    xButtons += btConfig->getW() + MenuButtonsSpacing;

    addObject(btRender = new Button36(true), xButtons, yMenu, 30, 30, "bt.render");
    btConfig->setLedType(true);
    xButtons += btRender->getW() + MenuGroupsSpacing*2;

    addObject(btPlay = new Button36(true), xButtons, yMenu, 52, buttonHeight, "bt.play");
    xButtons += 52 + MenuButtonsSpacing;

    addObject(btStop = new Button36(false), xButtons, yMenu, 32, buttonHeight, "bt.stop");
    xButtons += 32 + MenuButtonsSpacing;

    addObject(btRec = new Button36(false), xButtons, yMenu, buttonWidth, buttonHeight, "bt.rec");
    xButtons += buttonWidth + MenuButtonsSpacing;

    addObject(btHome = new Button36(false), xButtons, yMenu, buttonWidth, buttonHeight, "bt.home");
    xButtons += buttonWidth + MenuButtonsSpacing;

    addObject(btEnd = new Button36(false), xButtons, yMenu, buttonWidth, buttonHeight, "bt.end");
    xButtons += buttonWidth + MenuGroupsSpacing;

    addObject(bpmBox = new BpmBox(120), xButtons, yMenu, 70, buttonHeight);
    xButtons += bpmBox->getW() + MenuButtonsSpacing;

    addObject(meterBox = new MeterBox(4, 4), xButtons, yMenu, 54, buttonHeight);
    xButtons += meterBox->getW() + MenuButtonsSpacing;

    addObject(octaveBox = new OctaveBox(5), xButtons, yMenu, 30, buttonHeight);
    xButtons += octaveBox->getW() + MenuButtonsSpacing;

    addObject(snapMenu = new SnapMenu(), xButtons, yMenu, 64, buttonHeight);
    xButtons += snapMenu->getW() + MenuButtonsSpacing;

    addObject(timeScreen = new TimeScreen(), xButtons, yMenu, 80, buttonHeight, "pos.display");

    xButtons += timeScreen->getW() + MenuGroupsSpacing*4;

    addObject(btBrowser = new GroupButton(2), xButtons, yMenu, buttonWidth*2, buttonHeight, "bt.browser");
    btBrowser->setLedType(true);
    xButtons += btBrowser->getW() + MenuGroupsSpacing;
    addObject(btBarMode = new GroupButton(2), xButtons, yMenu, buttonWidth, buttonHeight, "bt.bars"); 
    btBarMode->setLedType(true);
    btBarMode->press();
    xButtons += btBarMode->getW() + MenuButtonsSpacing;
    addObject(btVols = new GroupButton(2), xButtons, yMenu,  buttonWidth, buttonHeight, "bt.vols"); 
    btVols->setLedType(true);
    xButtons += btVols->getW() + MenuButtonsSpacing;
    addObject(btPans = new GroupButton(2), xButtons, yMenu,  buttonWidth, buttonHeight, "bt.pans"); 
    btPans->setLedType(true);
    xButtons += btPans->getW() + MenuGroupsSpacing;
    */

    addObject(fileMenu = new DropBox(0, "FILE"));
                fileMenu->addItem("New");
                fileMenu->addItem("Open");
                fileMenu->addItem("");
                fileMenu->addItem("Save");
                fileMenu->addItem("Save As");
                fileMenu->addItem("");
                fileMenu->addItem("Load Project ");
                //fileMenu->addItem("Render to WAV or MP3");
                fileMenu->addItem("");
                fileMenu->addItem("Render");
                fileMenu->addItem("");
                fileMenu->addItem("Exit");
    addObject(editMenu = new DropBox(0, "EDIT"));
                editMenu->addItem("Undo");
                editMenu->addItem("Redo");
                editMenu->addItem("");
                editMenu->addItem("Delete Song");
                editMenu->addItem("");
                editMenu->addItem("Settings");
    addObject(helpMenu = new DropBox(0, "HELP"));
                helpMenu->addItem("Keys Reference");
                helpMenu->addItem("");
                helpMenu->addItem("About M");

    addObject(btConfig = new Button36(true), "bt.cfg");
    btConfig->setLedType(true);
    addObject(btRender = new Button36(true), "bt.render");
    btConfig->setLedType(true);
    addObject(btPlay = new Button36(true), "bt.play");
    addObject(btStop = new Button36(false), "bt.stop");
    addObject(btRec = new Button36(false), "bt.rec");
    addObject(btHome = new Button36(false), "bt.home");
    addObject(btEnd = new Button36(false), "bt.end");
    addObject(bpmBox = new BpmBox(120));
    addObject(meterBox = new MeterBox(4, 4));
    addObject(octaveBox = new OctaveBox(5));
    addObject(snapMenu = new SnapMenu());
    addObject(timeScreen = new TimeScreen(), "pos.display");
    addObject(btBrowser = new GroupButton(2), "bt.browser");
    addObject(btBarMode = new GroupButton(2), "bt.bars"); 
    addObject(btVols = new GroupButton(2), "bt.vols"); 
    addObject(btPans = new GroupButton(2), "bt.pans"); 

    int buttonWidth = 30;
    int buttonHeight = 30;

    setBorder(4);

    putStart(4, 4);
    putRight(fileMenu);
    putRight(editMenu);
    putRight(helpMenu);
    spaceRight();
    putRight(btConfig, buttonWidth, buttonHeight);
    putRight(btRender, buttonWidth, buttonHeight);
    spaceRight();
    spaceRight();
    spaceRight();
    putRight(btPlay, 52, buttonHeight);
    putRight(btStop, 34, buttonHeight);
    putRight(btRec, buttonWidth, buttonHeight);
    putRight(btHome, buttonWidth, buttonHeight);
    putRight(btEnd, buttonWidth, buttonHeight);
    spaceRight();
    putRight(timeScreen, 80, buttonHeight + 2);
    putRight(meterBox, 54, buttonHeight + 2);
    putRight(bpmBox, 70, buttonHeight + 2);
    putRight(snapMenu, 64, buttonHeight + 2);
    putRight(octaveBox, 30, buttonHeight + 2);
    spaceRight();
    spaceRight();
    spaceRight();
    putRight(btBrowser, buttonWidth*2, buttonHeight);
    putRight(btBarMode, buttonWidth, buttonHeight);
    putRight(btVols, buttonWidth, buttonHeight);
    putRight(btPans, buttonWidth, buttonHeight);

    xButtons = wndW + 50;

    // Below controls sizes are flexible, depending on window size

    addObject(btZoomOut = new Button36(false), "bt.zoomout");
    addObject(btZoomIn = new Button36(false), "bt.zoomin");

    addObject(btScrollBack = new Button36(false), "bt.navback");
    addObject(btScrollForth = new Button36(false), "bt.navforth");
}

bool ControlPanel::wavesAreVisible()
{
    return false;

    //return btWaveMode->isPressed();
}

void ControlPanel::remap()
{
    int buttonWidth = 30;
    int yTop = 0;
    int navHeight = height - 16;
    int xStartCoord = xButtons + MenuGroupsSpacing;

    if(width - 30 > (xStartCoord + 30*2))
    {
        if(!btScrollBack->isShown())
        {
            btScrollBack->setEnable(true);
        }

        if(!btScrollForth->isShown()) 
        {
            btScrollForth->setEnable(true);
        }

        if(!MGrid->hscr->isShown()) 
        {
            MGrid->hscr->setEnable((true));
        }

        int bH = navHeight/2 + 1;

        btZoomOut->setCoords1(xStartCoord, yTop, buttonWidth, bH);
        btZoomIn->setCoords1(xStartCoord, yTop + bH - 1, buttonWidth, bH);

        xStartCoord += buttonWidth + 6;

        btScrollBack->setCoords1(xStartCoord, yTop, buttonWidth, navHeight);
        btScrollForth->setCoords1(width - 30, yTop, buttonWidth, navHeight);

        MGrid->hscr->setCoords2(btScrollBack->getX2() + 3, yTop, btScrollForth->getX1() - 3, yTop + navHeight - 1);
    }
    else
    {
        btScrollBack->setEnable(false);
        btScrollForth->setEnable(false);
        MGrid->hscr->setEnable(false);
    }
}

void ControlPanel::drawSelf(Graphics& g)
{
    fill(g, 0.4f);

    setc(g, .25f);

    //lineH(g, 0, 0, width - 1);

    for(int i = 1; i < 40; i++)
    {
        float clr = .55f/(float(i));

        //gSetMonoColor(g, .8f, clr);
        //gSetColorHue(g, .15f, 1, .1f, clr);
        //gFillRectWH(g, x1, y1 + i - 1, width, 1);
    }

/*
    gSetMonoColor(g, 0.75f);
    float alpha = 0.1f;
    for(int c = 0; c < height - 1; c++)
    {
        //g.setColour(Colour(color));
        gSetMonoColor(g, 1.f, alpha);
        gLineHorizontal(g, y1 + c, (float)x1, (float)x2);
        alpha -= 0.0015f;
        if (alpha <= 0)
        {
            break;
        }
    }*/
}

ContextMenu* ControlPanel::createContextMenu()
{
    ContextMenu* menu = new ContextMenu(this);

    menu->addMenuItem("New Project");
    menu->addMenuItem("Save Project");
    menu->addMenuItem("Save Project As...");
    menu->addMenuItem("Render to WAV or MP3");
    menu->addMenuItem("Exit Program");

    return menu;
}

void ControlPanel::handleChildEvent(Gobj* obj, InputEvent& ev)
{
    if (obj == MGrid->hscr)
    {
        MGrid->handleChildEvent(obj, ev);

        MEdit->playHead->updatePosFromFrame();
    }
    else if(obj == btWaveMode)
    {
        MGrid->redraw(true);
    }
    else if(obj == btBrowser)
    {
        if (MBrowser->isON())
        {
            MObject->switchBrowser(false);

            btBrowser->revertToPrevious();
        }
        else
        {
            MObject->switchBrowser(true);
        }
    }
    else if (ev.wheelDelta != 0 && (obj == btZoomIn || obj == btZoomOut))
    {
        if (ev.wheelDelta > 0)
        {
            while (ev.wheelDelta > 0)
            {
                MGrid->adjustScale(1, -1);

                ev.wheelDelta--;
            }
        }
        else
        {
            while (ev.wheelDelta < 0)
            {
                MGrid->adjustScale(-1, -1);

                ev.wheelDelta++;
            }
        }
    }
    else // if(ev.leftClick)
    {
        if (ev.clickDown)
        {
            if(obj == btVols)
            {
                MGrid->setEditMode(GridDisplayMode_Volumes);

                MObject->switchBrowser(false);
            }
            else if(obj == btPans)
            {
                MGrid->setEditMode(GridDisplayMode_Pans);

                MObject->switchBrowser(false);
            }
            else if(obj == btStepMode)
            {
                MGrid->setEditMode(GridDisplayMode_Steps);

                MObject->switchBrowser(false);
            }
            else if(obj == btBarMode)
            {
                MGrid->setEditMode(GridDisplayMode_Bars);

                MObject->switchBrowser(false);
            }
            else if (obj == btScale1)
            {
                MGrid->setPixelsPerTick(2);
            }
            else if (obj == btScale2)
            {
                MGrid->setPixelsPerTick(10);
            }
            else if (obj == btScale3)
            {
                MGrid->setPixelsPerTick(32);
            }
            else if(obj == btHome)
            {
                MTransp->goToHome();
            }
            else if(obj == btEnd)
            {
                MTransp->goToEnd();
            }
            else if(obj == btZoomIn)
            {
                MGrid->adjustScale(1);
            }
            else if(obj == btZoomOut)
            {
                MGrid->adjustScale(-1);
            }
            else if (obj == btScrollBack)
            {
                MGrid->setHoffs(MGrid->getHoffs() - (MTransp->getTicksPerBeat()));
            }
            else if (obj == btScrollForth)
            {
                MGrid->setHoffs(MGrid->getHoffs() + (MTransp->getTicksPerBeat()));
            }
            else if (obj == btPlay)
            {
                MTransp->togglePlayback();
            }
            else if (obj == btStop)
            {
                MTransp->stopPlayback(true);

                btPlay->release();
            }
            else if(obj == btConfig)
            {
                static SubWindow* cw = NULL;

                if(cw == NULL)
                {
                    ConfigWinObject* co = new ConfigWinObject();

                    co->setInvokeButton(btConfig);

                    cw = MObject->addWindow((WinObject*)co);
                }

                cw->setOpen(!cw->isOpen());
            }
            else if(obj == btRender)
            {
                static SubWindow* rw = NULL;

                if(rw == NULL)
                {
                    RenderWinObject* ro = new RenderWinObject();

                    ro->setInvokeButton(btRender);

                    rw = MObject->addWindow((WinObject*)ro);
                }

                rw->setOpen(!rw->isOpen());
            }
            else if (obj == btRec)
            {
                Button36* bt = (Button36*)obj;

                if(bt->isPressed())
                {
                    GRecOn = true;
                }
                else
                {
                    MTransp->stopRecording();
                }
            }
        }
    }

}


