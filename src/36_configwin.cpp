
#include "36_configwin.h"
#include "36_slider.h"
#include "36_params.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"
#include "36_listbox.h"




ConfigObject::ConfigObject()
{
    Gobj::addObject(slider1 = new Slider36(false), 10, 50, 120, 20);
    Gobj::addObject(slider2 = new Slider36(false), 10, 72, 120, 20);
    Gobj::addObject(slider3 = new Slider36(false), 10, 94, 120, 20);

    Gobj::addObject(listBox = new ListBoxx("List1"), 200, 50, 120, 100);

    listBox->addEntry("Entry1");
    listBox->addEntry("Entry2");
    listBox->addEntry("Entry3");
    listBox->addEntry("Entry4");
    listBox->addEntry("Entry5");
    listBox->addEntry("Entry6");
    listBox->addEntry("Entry7");
    listBox->addEntry("Entry8");
    listBox->addEntry("Entry9");
    /*
    listBox->addEntry("Entry10");
    listBox->addEntry("Entry11");
*/

    slider1->addParam(hue = new Parameter(globalHue, 0, 1));
    slider2->addParam(saturation = new Parameter(0.2f, 0, 1));
    slider3->addParam(brightness = new Parameter(0, -1.f, 1.9f));

    hue->addControl(slider1);
    saturation->addControl(slider2);
    brightness->addControl(slider3);

    addParam(hue);
    addParam(saturation);
    addParam(brightness);

    WinObject::setWidthHeight(500, 400);
}

void ConfigObject::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void ConfigObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void ConfigObject::handleParamUpdate(Param * param)
{
    if(param == hue)
    {
        globalHue = hue->getOutVal();

        MWindow->repaint();

        MObject->redraw();
        MGrid->redraw(false, true);
    }
    else if(param == saturation)
    {
        globalSaturation = saturation->getOutVal();

        MWindow->repaint();

        MObject->redraw();
        MGrid->redraw(false, true);
    }
    else if(param == brightness)
    {
        brightnessCoeff = 2 - (1 + brightness->getOutVal());

        MWindow->repaint();

        MObject->redraw();
        MGrid->redraw(false, true);
    }

    Gobj::redraw();

    if(holderWindow)
        holderWindow->repaint();
}


