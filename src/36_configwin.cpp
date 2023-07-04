

#include "36_audiomanager.h"
#include "36_audio_dev.h"
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
    // Init params and controls

    //addParam(bufferSize = new Parameter("BUFFER", 512, 16384, 2048, Units_Integer));
    //bufferSize->setUnitString("samples");

    addObject(bufferSizeBox = new Knob(new Parameter("BUFFER", 512, 16384, 2048, Units_Integer), false));
    bufferSizeBox->getParam()->setUnitString("samples");
    bufferSizeBox->setTextParams(true, false, false, .4f);


    interpolationSelect = new Parameter("INTERPOLATION", Param_Radio);
    interpolationSelect->addOption("Linear");
    interpolationSelect->addOption("3-point Hermite");
    interpolationSelect->addOption("6-point Polinomial");
    interpolationSelect->addOption("Sinc depth 64");
    interpolationSelect->setCurrentOption(0);


    addObject(interpolationChooserBox = new SelectorBox(interpolationSelect));
    
    interpolationChooserBox->setTextParams(true, false, false, .4f);

    addObject(outputDevices = new ListBoxS("OUTPUT devices"));

    JAudioManager->addDeviceNamesToListBox(*outputDevices);

    addObject(showPanel = new Button36(false, "SHOW PANEL"));
    addObject(inputDevices = new ListBoxx("INPUT devices:"));
    
    inputDevices->addEntry("IN Device1");
    inputDevices->addEntry("IN Device2");

    addObject(midiOutDevices = new ListBoxx("MIDI OUT Devices:"));
    addObject(midiInDevices = new ListBoxx("MIDI IN Devices:"));
    addObject(renderBox = new ListBoxx("RENDER:"));

    renderBox->addEntry("Render to MP3");
    renderBox->addEntry("Render to OGG");
    renderBox->addEntry("Render to WAV");
    renderBox->addEntry("Render to FLAC");

    Parameter* renderFormat = new Parameter("FORMAT", Param_Radio);
    renderFormat->addOption("WAV");
    renderFormat->addOption("OGG");
    renderFormat->addOption("FLAC");
    renderFormat->setCurrentOption(0);

    SelectorBox* renderFormatBox = new SelectorBox(renderFormat);
    
    addObject(renderFormatBox);


    // Position all controls

    int colWidth = 220;

    putRight(outputDevices, colWidth, colWidth);
    spaceBelow();
    putBelow(bufferSizeBox, colWidth, 16);
    spaceBelow();
    putBelow(interpolationChooserBox, colWidth, 16);
    spaceBelow();
    putBelow(showPanel, colWidth, 24);
    returnUp();
    spaceRight();
    putRight(inputDevices, colWidth, colWidth);
    spaceRight();
    putRight(midiOutDevices, colWidth, colWidth);
    spaceRight();
    putRight(renderBox, colWidth, colWidth/1.5);
    spaceBelow();
    putBelow(renderFormatBox, colWidth, 24);
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

    //showPanel->setVis(false);

    setWH(getW(), getH());

    updateObjectsVisibility();

    setObjName("Configuration");
}

void ConfigWinObject::drawSelf(Graphics& g)
{
    fill(g, 0.36f);
}


void ConfigWinObject::updateObjectsVisibility()
{
    JuceAudioDeviceManager::AudioDeviceSetup setup;
    JAudioManager->getAudioDeviceSetup(setup);

    if (JAudioManager->getCurrentAudioDevice() != NULL)
    {
        if (JAudioManager->getCurrentAudioDevice()->hasControlPanel())
        {
            showPanel->setVis(true);
        }
        else
        {
            showPanel->setVis(false);
        }

        String tname = JAudioManager->getCurrentAudioDevice()->getTypeName();
        
        if (tname == T("ASIO"))
        {
            bufferSizeBox->setVis(false);
        }
        else
        {
            bufferSizeBox->setVis(true);

            bufferSizeBox->getParam()->setValue(JAudioManager->getCurrentAudioDevice()->getCurrentBufferSizeSamples());
        }
    }
    else
    {
        showPanel->setVis(false);
    }

    redraw();
}



void ConfigWinObject::switchAudioDevice()
{
/*
    if (juceAudioDeviceDropDown->getSelectedId() < 0)
    {
        //audioDeviceManager->setAudioDevice (String::empty, 0, 0, 0, 0, true);
        audioDeviceManager->closeAudioDevice();
    }
    else*/
    {
        JuceAudioDeviceManager::AudioDeviceSetup oldSetup;
        JuceAudioDeviceManager::AudioDeviceSetup setup;

        JAudioManager->getAudioDeviceSetup(setup);

        oldSetup = setup;

        setup.outputDeviceName = outputDevices->getCurrentName();

        setup.bufferSize = DEFAULT_BUFFER_SIZE;

        AudioIODeviceType* type = JAudioManager->getDeviceTypeObjectByDeviceName(setup.outputDeviceName);

        String error (JAudioManager->setAudioDeviceSetup(setup, true));

        if (error.isNotEmpty())
        {
#if JUCE_WIN32
            if (setup.inputChannels.countNumberOfSetBits() > 0
                  && setup.outputChannels.countNumberOfSetBits() > 0)
            {
                // in DSound, some machines lose their primary input device when a mic
                // is removed, and this also buggers up our attempt at opening an output
                // device, so this is a workaround that doesn't fail in that case.
                BitArray noInputs;
                setup.inputChannels = noInputs;
                error = JAudioManager->setAudioDeviceSetup(setup, true);
            }
#endif
            if (error.isNotEmpty())
                AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                             T("Error while opening \"")
                                                + outputDevices->getCurrentName()
                                                + T("\""),
                                             error);
        }
    }

    updateObjectsVisibility();
}

void ConfigWinObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    if (obj == bufferSizeBox)
    {
        if (!ev.clickDown)
        {
            MAudio->setBufferSize(bufferSizeBox->getParam()->getOutVal());
        }
    }
    else if (obj == outputDevices)
    {
        switchAudioDevice();
    }
    else if (showPanel == obj)
    {
        JAudioManager->getCurrentAudioDevice()->showControlPanel();
    }
}



void ConfigWinObject::handleParamUpdate(Parameter * param)
{
    //if (param == bufferSize)
    {
        //MAudio->setBufferSize(param->getOutVal());
    }

    redraw();

    //if(holderWindow)
    {
        //holderWindow->repaint();
    }
}


