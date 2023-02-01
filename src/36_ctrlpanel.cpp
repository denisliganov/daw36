

#include "36.h"
#include "36_system.h"
#include "36_ctrlpanel.h"
#include "36_scroller.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_menu.h"
#include "36_button.h"
#include "36_sampleinstr.h"
#include "36_mixer.h"
#include "36_project.h"
#include "36_pattern.h"
#include "36_transport.h"
#include "36_audio_dev.h"
#include "36_draw.h"
#include "36_juce_windows.h"
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
    int yMenu = 4;
    int xMenu = 0;

    addObject(fileMenu = new DropBox(0, "FILE"), xMenu, yMenu);
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
    xMenu += 40;
    addObject(editMenu = new DropBox(0, "EDIT"), xMenu, yMenu);
    editMenu->addItem("Undo");
    editMenu->addItem("Redo");
    editMenu->addItem("");
    editMenu->addItem("Delete Song");
    editMenu->addItem("");
    editMenu->addItem("Settings");
    xMenu += 41;
    addObject(helpMenu = new DropBox(0, "HELP"), xMenu, yMenu);
    helpMenu->addItem("Keys Reference");
    helpMenu->addItem("");
    helpMenu->addItem("About M");

    //xMenu += 140;
    //addObject(brwMenu = new DropBox(0, "BROWSER"), xMenu, yMenu);
    addObject(btConfig = new Button36(true), 180, 0, 30, 30, "bt.cfg");
    btConfig->setLedType(true);

    int buttonWidth = 30;
    int buttonHeight = 30;

    //addObject(colorChange = new Button36(false), xControls, yControls, buttonWidth, buttonHeight, "bt.clr");
    //xControls += buttonWidth + 1;
    //addObject(trkSizeUp = new Button36(false), xControls, yControls, buttonWidth, buttonHeight/2, "bt.clr");
    //addObject(trkSizeDown = new Button36(false), xControls, yControls + buttonHeight/2, buttonWidth, buttonHeight/2, "bt.clr");
    //xControls += buttonWidth + 1;

    xShowHide = 0;
    yShowHide = CtrlPanelHeight - 28;

    addObject(btBrowser = new GroupButton(2), "bt.browser");
    btBrowser->setLedType(true);
    //addObject(btStepMode = new GroupButton(2), "bt.steps"); 
    //btStepMode->setLedType(true);
    addObject(btBarMode = new GroupButton(2), "bt.bars"); 
    btBarMode->setLedType(true);
    addObject(btVols = new GroupButton(2), "bt.vols"); 
    btVols->setLedType(true);
    addObject(btPans = new GroupButton(2), "bt.pans"); 
    btPans->setLedType(true);

    btBarMode->press();

    addObject(btWaveMode = new Button36(true), "bt.showwaves"); 
    btWaveMode->setLedType(true);
    addObject(btMouseMode = new Button36(true), "bt.mousemode");
    btMouseMode->setLedType(true);


    int yControls = 4;
    xControls = 400;

    addObject(btPlay = new Button36(true), xControls, yControls, 52, buttonHeight, "bt.play");
    xControls += 52 + 4;
    addObject(btStop = new Button36(false), xControls, yControls, 32, buttonHeight, "bt.stop");
    xControls += 32 + 4;
    addObject(btRec = new Button36(false), xControls, yControls, 28, buttonHeight, "bt.rec");
    xControls += 28 + 4;
    addObject(btHome = new Button36(false), xControls, yControls, 28, buttonHeight, "bt.home");
    xControls += 28 + 4;
    addObject(btEnd = new Button36(false), xControls, yControls, 28, buttonHeight, "bt.end");
    xControls += 28 + 20;

    addObject(bpmBox = new BpmBox(120), xControls, yControls, 70, buttonHeight);
    xControls += bpmBox->getW() + 5;
    addObject(meterBox = new MeterBox(4, 4), xControls, yControls, 54, buttonHeight);
    xControls += meterBox->getW() + 5;
    addObject(octaveBox = new OctaveBox(5), xControls, yControls, 30, buttonHeight);
    xControls += octaveBox->getW() + 5;
    addObject(snapMenu = new SnapMenu(), xControls, yControls, 64, buttonHeight);

    xControls += snapMenu->getW() + 5;
    addObject(timeScreen = new TimeScreen(), xControls, yControls, 80, buttonHeight, "pos.display");
    xControls += timeScreen->getW() + 5;

    addObject(btZoomOut = new Button36(false), "bt.zoomout");
    addObject(btZoomIn = new Button36(false), "bt.zoomin");

    addObject(btScrollBack = new Button36(false), "bt.navback");
    addObject(btScrollForth = new Button36(false), "bt.navforth");
}

bool ControlPanel::wavesAreVisible()
{
    return btWaveMode->isPressed();
}

void ControlPanel::remap()
{
    int buttonWidth = 30;
    int yTop = 0;
    int navHeight = height - 16;
    int xStartCoord = xControls + 150;

    if(width - 30 > (xStartCoord + 30*2))
    {
        if(!btScrollBack->isshown())
        {
            btScrollBack->setEnable(true);
        }

        if(!btScrollForth->isshown()) 
        {
            btScrollForth->setEnable(true);
        }

        if(!MGrid->hscr->isshown()) 
        {
            MGrid->hscr->setEnable((true));
        }

/*
        int bH = 23;
        btScale1->setCoords1(xStartCoord, yTop, buttonWidth, bH);
        btScale2->setCoords1(xStartCoord, yTop + bH - 1, buttonWidth, bH);
        btScale3->setCoords1(xStartCoord, yTop + bH*2 - 2, buttonWidth, bH);
        xStartCoord += buttonWidth + 6;
*/

        int bH = navHeight/2 + 1;

        btZoomOut->setCoords1(xStartCoord, yTop, buttonWidth, bH);
        btZoomIn->setCoords1(xStartCoord, yTop + bH - 1, buttonWidth, bH);

        xStartCoord += buttonWidth + 6;

        btScrollBack->setCoords1(xStartCoord, yTop, buttonWidth, navHeight);
        btScrollForth->setCoords1(width - 30, yTop, buttonWidth, navHeight);

        xStartCoord = xControls + 80;

        MGrid->hscr->setCoords2(btScrollBack->getX2() + 3, yTop, btScrollForth->getX1() - 3, yTop + navHeight - 1);

        int xEditButtons = 150;
        int yEditButtons = 4;
        int buttonHeight = 30;

        btConfig->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        xEditButtons += buttonWidth + 10;

        btBrowser->setCoords1(xEditButtons, yEditButtons, buttonWidth*2, buttonHeight); 
        xEditButtons += buttonWidth*2 + 1;
        //btStepMode->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        //xEditButtons += buttonWidth + 1;
        btBarMode->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        xEditButtons += buttonWidth + 1;
        //btWaveMode->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        //xEditButtons += buttonWidth + 1;
        btVols->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        xEditButtons += buttonWidth + 1;
        btPans->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight); 
        xEditButtons += buttonWidth + 1;
        btMouseMode->setCoords1(xEditButtons, yEditButtons, buttonWidth, buttonHeight);
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

    lineH(g, 0, 0, width - 1);

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
        if (MBrowser->isEnabled())
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
                ToggleConfigWindow();
            }
            else if(obj == btRender)
            {
                //RenderWnd->setVisible(!RenderWnd->isVisible());
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


