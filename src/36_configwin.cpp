
#include "36_configwin.h"
#include "36_slider.h"
#include "36_knob.h"
#include "36_params.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"
#include "36_listbox.h"
#include "36_numbox.h"



ConfigObject::ConfigObject()
{
    border = 10;
    xstart = ystart = border;
    groupSpacing = 8;
    smallSpacing = 4;
    wndW = border;
    wndH = border;
    horizPut = false;


/*
    Gobj::addObject(buffSizeBox = new Slider36(false), 10, 50, 120, 20);
    Gobj::addObject(pbox2 = new Slider36(false), 10, 72, 120, 20);
    Gobj::addObject(pbox3 = new Slider36(false), 10, 94, 120, 20);

    Gobj::addObject(midiDevices = new ListBoxx("List1"), 200, 50, 120, 100);

    Gobj::addObject(knob1 = new Knob(new Parameter(0.5f)), 140, 190, 40, 40);
    Gobj::addObject(knob2 = new Knob(new Parameter(0.5f)), 180, 190, 20, 20);
*/

    // Init params and controls

    addParam(hue = new Parameter("hue", globalHue, 0, 1));
    addParam(saturation = new Parameter("sat", 0.2f, 0, 1));
    addParam(brightness = new Parameter("brightness", 0, -1.f, 1.9f));
    addParam(bufferSize = new Parameter("buffsize", 2048, 512, 16384-512));

    knob1 = new Knob(hue);
    knob2 = new Knob(saturation);
    knob3 = new Knob(brightness);
    buffSizeBox = new ParamBox(bufferSize);

    pr = new ParamRadio("Radio1");
    pr->addOption("Opt1");
    pr->addOption("Opt2");
    pr->addOption("Opt3");
    pr->addOption("Opt4");
    pr->addOption("Opt15");

    chooserBox = new RadioBox(pr);

    outputDevices = new ListBoxx("Output devices");
    outputDevices->addEntry("Device1");
    outputDevices->addEntry("Device2");

    midiDevices = new ListBoxx("MIDI devices");
    midiDevices->addEntry("Entry1");
    midiDevices->addEntry("Entry2");
    midiDevices->addEntry("Entry3");
    midiDevices->addEntry("Entry4Entry4Entry4Entry4Entry4");
    midiDevices->addEntry("Entry5");
    midiDevices->addEntry("Entry6Entry4Entry4");
    midiDevices->addEntry("Entry7Entry4Entry4Entry4Entry4Entry4");
    midiDevices->addEntry("Entry8Entry4");
    midiDevices->addEntry("Entry9");
    midiDevices->addEntry("Entry10");
    midiDevices->addEntry("Entry11");


    // Place all controls


    placeObject(outputDevices, 220, 300);
    placeObject(buffSizeBox, 180, 20);

    resetPut();

    horizPut = true;

    resetGroup();

    placeObject(midiDevices, 220, 100);

    resetGroup();

    placeObject(chooserBox, 100, chooserBox->getH());

    resetGroup();

    placeObject(knob1, 55, 70);
    placeObject(knob2, 55, 70);
    placeObject(knob3, 55, 70);

    resetGroup();
    resetPut();

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
    {
        holderWindow->repaint();
    }
}


