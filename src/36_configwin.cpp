

#include "36_audiomanager.h"
#include "36_configwin.h"
#include "36_knob.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"
#include "36_listbox.h"
#include "36_knob.h"
#include "36_params.h"


ConfigWinObject::ConfigWinObject()
{
    //Gobj::addObject(buffSizeBox = new Slider36(false), 10, 50, 120, 20);
    //Gobj::addObject(pbox2 = new Slider36(false), 10, 72, 120, 20);
    //Gobj::addObject(pbox3 = new Slider36(false), 10, 94, 120, 20);
    //Gobj::addObject(midiDevices = new ListBoxx("List1"), 200, 50, 120, 100);
    //Gobj::addObject(knob1 = new Knob(new Parameter(0.5f)), 140, 190, 40, 40);
    //Gobj::addObject(knob2 = new Knob(new Parameter(0.5f)), 180, 190, 20, 20);


    // Init params and controls

    addParam(hue = new Parameter("Hue", globalHue, 0, 1));
    addParam(saturation = new Parameter("Sat", 0, 1, 0.2f));
    addParam(brightness = new Parameter("Brightness", -1.f, .9f, 0));
    WinObject::addObject(knob1 = new Knob(hue));
    WinObject::addObject(knob2 = new Knob(saturation));
    WinObject::addObject(knob3 = new Knob(brightness));

    addParam(bufferSize = new Parameter("BUFFER SIZE", 512, 16384, 2048, Units_Integer));
    bufferSize->setUnitString("samples");
    WinObject::addObject(bufferSizeBox = new Knob(bufferSize, false));

    bufferSizeBox->setTextParams(true, false, true, .5f);

    interpolationSelect = new Parameter("INTERPOLATION", Param_Radio);
    interpolationSelect->addOption("Linear");
    interpolationSelect->addOption("3-point Hermire");
    interpolationSelect->addOption("6-point Polinomial");
    interpolationSelect->addOption("Sinc depth 64");
    interpolationSelect->setCurrentOption(0);


    WinObject::addObject(interpolationChooserBox = new SelectorBox(interpolationSelect));

    interpolationChooserBox->setTextParams(true, false, true, .5f);

    WinObject::addObject(outputDevices = new ListBoxx("OUTPUT devices"));
//                outputDevices->addEntry("Output Device 1");
//                outputDevices->addEntry("Output Device 2");

    JAudManager->addDeviceNamesToListBox(*outputDevices);

    showASIOPanel = new Button36(false, "Show ASIO panel");

    WinObject::addObject(inputDevices = new ListBoxx("INPUT devices:"));
    inputDevices->addEntry("IN Device1");
    inputDevices->addEntry("IN Device2");

    WinObject::addObject(midiOutDevices = new ListBoxx("MIDI OUT Devices:"));
    WinObject::addObject(midiInDevices = new ListBoxx("MIDI IN Devices:"));
    WinObject::addObject(renderBox = new ListBoxx("RENDER:"));
    renderBox->addEntry("Render to MP3");
    renderBox->addEntry("Render to OGG");
    renderBox->addEntry("Render to WAV");
    renderBox->addEntry("Render to FLAC");

    int colWidth = 220;

    // Position all controls

    putRight(outputDevices, colWidth, colWidth);
    spaceBelow();
    putBelow(interpolationChooserBox, colWidth, 16);
    putBelow(bufferSizeBox, colWidth, 16);
    returnUp();
    spaceRight();
    putRight(inputDevices, colWidth, colWidth);
    spaceRight();
    putRight(midiOutDevices, colWidth, colWidth);
    spaceRight();
    putRight(renderBox, colWidth, colWidth/1.5);
    spaceRight();


    //putRight(midiInDevices, colWidth, 120);
    //spaceRight();
    returnLeft();

    spaceBelow(6);
    
    //putRight(buffSizeBox, 120, 30);
    //putBelow(showASIOPanel, 120, 20);
    //spaceBelow();
    //putBelow(chooserBox, 120, chooserBox->getH());
    //spaceRight();
    //returnUp();

    finalizePuts();

    setWidthHeight(Gobj::getW(), Gobj::getH());

    setName("Configuration");
}

void ConfigWinObject::drawSelf(Graphics& g)
{
    Gobj::fill(g, 0.4f);
}

void ConfigWinObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void ConfigWinObject::handleParamUpdate(Parameter * param)
{
/*
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
    }*/

    Gobj::redraw();

    if(holderWindow)
    {
        holderWindow->repaint();
    }
}


