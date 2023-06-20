

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

    WinObject::addObject(outputDevices = new ListBoxS("OUTPUT devices"));
//                outputDevices->addEntry("Output Device 1");
//                outputDevices->addEntry("Output Device 2");

    JAudioManager->addDeviceNamesToListBox(*outputDevices);

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
    if (obj == bufferSizeBox)
    {
        if (!ev.clickDown)
        {
            MAudio->setBufferSize(bufferSize->getOutVal());
        }
    }
    else if (obj == outputDevices)
    {
        int a = 1;

        
    }
}

#if 0
{
    if (juceAudioDeviceDropDown->getSelectedId() < 0)
    {
        //audioDeviceManager->setAudioDevice (String::empty, 0, 0, 0, 0, true);
        audioDeviceManager->closeAudioDevice();
    }
    else
    {
        CAudioDeviceManager::AudioDeviceSetup oldSetup;
        CAudioDeviceManager::AudioDeviceSetup setup;

        audioDeviceManager->getAudioDeviceSetup(setup);

        oldSetup = setup;

        setup.outputDeviceName = juceAudioDeviceDropDown->getText();

        setup.bufferSize = DEFAULT_BUFFER_SIZE;

        AudioIODeviceType* type = audioDeviceManager->getDeviceTypeObjectByDeviceName(setup.outputDeviceName);

        String error (audioDeviceManager->setAudioDeviceSetup(setup, true));

        //String error (audioDeviceManager->setAudioDevice (juceAudioDeviceDropDown->getText(),
        //                                            0, 0, 0, 0, true));

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
                error = audioDeviceManager->setAudioDeviceSetup(setup, true);
            }
#endif
            if (error.isNotEmpty())
                AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                             T("Error while opening \"")
                                                + juceAudioDeviceDropDown->getText()
                                                + T("\""),
                                             error);
        }
    }

    if(audioDeviceManager->getCurrentAudioDevice() != NULL)
    {
        CAudioDeviceManager::AudioDeviceSetup setup;
        audioDeviceManager->getAudioDeviceSetup(setup);
        juceAudioDeviceDropDown->setText(setup.outputDeviceName, true);
    }
    else
    {
        juceAudioDeviceDropDown->setSelectedId(-1, true);
    }

    UpdateComponentsVisibility();
}
#endif


void ConfigWinObject::handleParamUpdate(Parameter * param)
{
    if (param == bufferSize)
    {
        //MAudio->setBufferSize(param->getOutVal());
    }

    Gobj::redraw();

    if(holderWindow)
    {
        holderWindow->repaint();
    }
}


