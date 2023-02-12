
#include "36_configwin.h"
#include "36_slider.h"
#include "36_knob.h"
#include "36_params.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"
#include "36_listbox.h"
#include "36_parambox.h"



ConfigObject::ConfigObject()
{

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
    addParam(bufferSize = new Parameter("Audio buffer size", 2048, 512, 16384-512, Units_Integer));

    bufferSize->setUnitString("samples");

    Gobj::addObject(knob1 = new Knob(hue));
    Gobj::addObject(knob2 = new Knob(saturation));
    Gobj::addObject(knob3 = new Knob(brightness));

    Gobj::addObject(buffSizeBox = new ParamBox(bufferSize));

    interpolationSelect = new ParamRadio("Interpolation (sample playback)");
    interpolationSelect->addOption("Linear");
    interpolationSelect->addOption("3-point Hermire");
    interpolationSelect->addOption("6-point Polinomial");
    interpolationSelect->addOption("Sinc depth 64");

    Gobj::addObject(chooserBox = new RadioBox(interpolationSelect));

    Gobj::addObject(outputDevices = new ListBoxx("Output devices"));
                outputDevices->addEntry("Output Device 1");
                outputDevices->addEntry("Output Device 2");

    showASIOPanel = new Button36(false, "Show ASIO panel");

    addObject(inputDevices = new ListBoxx("Input devices"));
                inputDevices->addEntry("IN Device1");
                inputDevices->addEntry("IN Device2");

    addObject(midiOutDevices = new ListBoxx("MIDI OUT devices"));
                midiOutDevices->addEntry("Entry1");
                midiOutDevices->addEntry("Entry2");
                midiOutDevices->addEntry("Entry3");

    addObject(midiInDevices = new ListBoxx("MIDI IN devices"));
                midiInDevices->addEntry("Entry4Entry4Entry4Entry4Entry4");
                midiInDevices->addEntry("Entry5");
                midiInDevices->addEntry("Entry6Entry4Entry4");
                midiInDevices->addEntry("Entry7Entry4Entry4Entry4Entry4Entry4");
                midiInDevices->addEntry("Entry8Entry4");
                midiInDevices->addEntry("Entry9");
                midiInDevices->addEntry("Entry10");
                midiInDevices->addEntry("Entry11");


    // Position all controls

    putRight(outputDevices, 280, 120);
    putBelow(inputDevices, 280, 120);
    spaceBelow();
    putBelow(midiOutDevices, 280, 120);
    putBelow(midiInDevices, 280, 120);
    goTop();
    spaceRight();
    putRight(buffSizeBox, 200, 20);
    putBelow(showASIOPanel, 200, 20);
    spaceBelow();
    putBelow(chooserBox, 200, chooserBox->getH());


/*
    putBelow(knob1, 55, 70);
    putBelow(knob2, 55, 70);
    putBelow(knob3, 55, 70);
*/
    finalizePuts();

    setName("Configuration");
}

void ConfigObject::drawSelf(Graphics& g)
{
    fill(g, 0.4f);
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


