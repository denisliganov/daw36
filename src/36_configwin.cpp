
#include "36_configwin.h"
#include "36_slider.h"
#include "36_knob.h"
#include "36_params.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"
#include "36_listbox.h"




ConfigObject::ConfigObject()
{
    border = 10;
    xstart = ystart = border;
    groupSpacing = 6;
    smallSpacing = 3;
    wndW = border;
    wndH = border;

/*
    Gobj::addObject(slider1 = new Slider36(false), 10, 50, 120, 20);
    Gobj::addObject(slider2 = new Slider36(false), 10, 72, 120, 20);
    Gobj::addObject(slider3 = new Slider36(false), 10, 94, 120, 20);

    Gobj::addObject(listBox = new ListBoxx("List1"), 200, 50, 120, 100);

    Gobj::addObject(knob1 = new Knob(new Parameter(0.5f)), 140, 190, 40, 40);
    Gobj::addObject(knob2 = new Knob(new Parameter(0.5f)), 180, 190, 20, 20);
*/

    horizPut = false;

    placeObject(slider1 = new Slider36(false), 40, 20);
    placeObject(slider2 = new Slider36(false), 40, 20);
    placeObject(slider3 = new Slider36(false), 40, 20);

    slider1->addParam(hue = new Parameter(globalHue, 0, 1));
    slider2->addParam(saturation = new Parameter(0.2f, 0, 1));
    slider3->addParam(brightness = new Parameter(0, -1.f, 1.9f));

    hue->addControl(slider1);
    saturation->addControl(slider2);
    brightness->addControl(slider3);

    horizPut = true;

    placeObject(listBox = new ListBoxx("List1"), 120, 100);

    horizPut = false;

    placeObject(knob1 = new Knob(new Parameter(0.5f)), 35, 35);
    placeObject(knob2 = new Knob(new Parameter(0.5f)), 22, 22);
    placeObject(knob3 = new Knob(new Parameter(0.5f)), 11, 11);
    placeObject(knob4 = new Knob(new Parameter(0.5f)), 11, 11);

    //horizPut = true;

    resetGroup();
    resetPut();

    placeObject(slider4 = new Slider36(false), 60, 20);
    placeObject(slider5 = new Slider36(false), 60, 20);
    placeObject(slider6 = new Slider36(false), 60, 20);

    listBox->addEntry("Entry1");
    listBox->addEntry("Entry2");
    listBox->addEntry("Entry3");
    listBox->addEntry("Entry4Entry4Entry4Entry4Entry4");
    listBox->addEntry("Entry5");
    listBox->addEntry("Entry6Entry4Entry4");
    listBox->addEntry("Entry7Entry4Entry4Entry4Entry4Entry4");
    listBox->addEntry("Entry8Entry4");
    listBox->addEntry("Entry9");
    listBox->addEntry("Entry10");
    listBox->addEntry("Entry11");

    addParam(hue);
    addParam(saturation);
    addParam(brightness);

    WinObject::setWidthHeight(wndW + border, wndH + border);
}

void ConfigObject::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void ConfigObject::placeObject(Gobj* obj, int ow, int oh)
{
    Gobj::addObject(obj, xstart, ystart, ow, oh);

    if (horizPut)
    {
        xstart += ow + smallSpacing;

        if (xstart > wndW)
        {
            wndW = xstart;
        }

        if (ystart + oh + smallSpacing > wndH)
        {
            wndH = ystart + oh + smallSpacing;
        }
    }
    else
    {
        ystart += oh + smallSpacing;

        if (ystart > wndH)
        {
            wndH = ystart;
        }

        if (xstart + ow + smallSpacing > wndW)
        {
            wndW = xstart + ow + smallSpacing;
        }
    }
}

void ConfigObject::resetPut()
{
    if (horizPut)
    {
        xstart = border;
        ystart = wndH;
//        wndH = ymax + groupSpacing;
    }
    else
    {
        xstart = wndW;
        ystart = border;
    }
}

void ConfigObject::resetGroup()
{
    if (horizPut)
    {
        xstart += groupSpacing;
    }
    else
    {
        ystart += groupSpacing;
    }

    if (xstart > wndW)
    {
        wndW = xstart;
    }

    if (ystart > wndH)
    {
        wndH = ystart;
    }
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


