

#include "36_globals.h"
#include "36_browser.h"
#include "36_browserlist.h"
#include "36_scroller.h"
#include "36_utils.h"
#include "36_effects.h"
#include "36_vst.h"
#include "36_sampleinstr.h"
#include "36_params.h"
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
#include "36_config.h"
#include "36_textinput.h"
#include "36_listbox.h"

#include <windows.h>
#include <direct.h>






BrwWinObject::BrwWinObject(Browser* brw)
{
    browser = brw;

    WinObject::addObject(browser, 0, 0, 1200, 900);
    //setWH(browser->getW(), browser->getH());

    setWidthHeight(1200, 800);

    //setResiz
}


void BrwWinObject::drawSelf(Graphics& g)
{
    WinObject::fill(g, 0.f);
}

void BrwWinObject::remap()
{
    //browser->setCoords1(0, 0, Gobj::getW(), Gobj::getH());
}

Browser::Browser(std::string dirpath)
{
    ipreview = NULL;

    addObject(fileBox = new BrowserList("File browser", WorkDirectory, BrwList_Files));
    addObject(projectsList1 = new BrowserList("Projects", "", BrwList_Projects));
    addObject(internalList1 = new BrowserList("Internal modules", "", BrwList_InternalModules));
    addObject(sampleList1 = new BrowserList("Samples", WorkDirectory, BrwList_WavSamples));
    addObject(plugList = new BrowserList("Plugins", ".\\Plugins", BrwList_VST2));
    addObject(vstList1 = new BrowserList("VST1", VST_EXT_PATH_1, BrwList_VST2));
    addObject(vstList2 = new BrowserList("VST2", VST_EXT_PATH_2, BrwList_VST2));
}

void Browser::drawSelf(Graphics& g)
{
    
}

void Browser::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    int a = 1;
}

void Browser::remap()
{
    int cx = 0;
    int cy = 0;
    int cw = 200;
    float lstHeight = (float)(height - cy - 1);

    int lw = width/7;

    int xLists = cx;

    confine(xLists, cy, width - lw, height); // no args -> reset bounds

    setObjSpacing(2);

    putStart(xLists, cy);

    putRight(internalList1, lw, lstHeight);
    putRight(sampleList1, lw, lstHeight);
    putRight(plugList, lw, lstHeight);
    putRight(vstList1, lw, lstHeight);
    putRight(vstList2, lw, lstHeight);
    putRight(projectsList1, lw, lstHeight);


    //for (ListBoxx* lb : listBoxes)
    //    putRight(lb, 200, lstHeight);

    confine();

    fileBox->setCoords1(width - lw + 2, cy, lw - 4, lstHeight);
}



