
#include "36_audiomanager.h"
#include "36_audio_dev.h"
#include "36_listbox.h"
#include "36_project.h"
#include "36_utils.h"



static float    AudioBuffer[MAX_BUFF_SIZE*2];





void JuceAudioCallback::audioDeviceIOCallback(const float** inputChannelData, int totalNumInputChannels, float** outputChannelData, int totalNumOutputChannels, int numSamples)
{
    //Random rnd(1);

    if(totalNumOutputChannels == 2)
    {
        MAudio->generalCallBack(NULL, AudioBuffer, numSamples);

        for(int fc = 0; fc < numSamples; fc++)
        {
            outputChannelData[0][fc] = AudioBuffer[fc*2];
            outputChannelData[1][fc] = AudioBuffer[fc*2 + 1];
        }
    }
}

void JuceAudioCallback::audioDeviceAboutToStart(AudioIODevice * device)
{
}

void JuceAudioCallback::audioDeviceStopped()
{
}




JuceAudioDeviceManager::AudioDeviceSetup::AudioDeviceSetup()
    : sampleRate (0),
      bufferSize (0),
      useDefaultInputChannels (true),
      useDefaultOutputChannels (true)
{
}

bool JuceAudioDeviceManager::AudioDeviceSetup::operator== (const JuceAudioDeviceManager::AudioDeviceSetup& other) const
{
    return outputDeviceName == other.outputDeviceName
            && inputDeviceName == other.inputDeviceName
            && sampleRate == other.sampleRate
            && bufferSize == other.bufferSize
            && inputChannels == other.inputChannels
            && useDefaultInputChannels == other.useDefaultInputChannels
            && outputChannels == other.outputChannels
            && useDefaultOutputChannels == other.useDefaultOutputChannels;
}

JuceAudioDeviceManager::JuceAudioDeviceManager()
    : currentAudioDevice (0),
      numInputChansNeeded (0),
      numOutputChansNeeded (2),
      lastExplicitSettings (0),
      listNeedsScanning (true),
      useInputNames (false),
      inputLevelMeasurementEnabledCount (0),
      inputLevel (0),
      testSound (0),
      tempBuffer (2, 2),
      defaultMidiOutput (0),
      cpuUsageMs (0),
      timeToCpuScale (0)
{
    callbackHandler.owner = this;
}

JuceAudioDeviceManager::~JuceAudioDeviceManager()
{
    deleteAndZero (currentAudioDevice);
    deleteAndZero (defaultMidiOutput);

    delete lastExplicitSettings;
    delete testSound;
}

void JuceAudioDeviceManager::createDeviceTypesIfNeeded()
{
    if (availableDeviceTypes.size() == 0)
    {
        createAudioDeviceTypes (availableDeviceTypes);

        while (lastDeviceTypeConfigs.size() < availableDeviceTypes.size())
            lastDeviceTypeConfigs.add (new AudioDeviceSetup());

        if (availableDeviceTypes.size() > 0)
            currentDeviceType = availableDeviceTypes.getUnchecked(0)->getTypeName();
    }
}

const OwnedArray <AudioIODeviceType>& JuceAudioDeviceManager::getAvailableDeviceTypes()
{
    scanDevicesIfNeeded();
    return availableDeviceTypes;
}

#ifdef USE_OLD_JUCE

/*
extern AudioIODeviceType* juce_createDefaultAudioIODeviceType();

#if JUCE_WIN32 && JUCE_ASIO
    extern AudioIODeviceType* juce_createASIOAudioIODeviceType();
#endif

#if JUCE_WIN32 && JUCE_WDM_AUDIO
    extern AudioIODeviceType* juce_createWDMAudioIODeviceType();
#endif
*/

void JuceAudioDeviceManager::createAudioDeviceTypes (OwnedArray <AudioIODeviceType>& list)
{
    AudioIODeviceType* const defaultDeviceType = juce_createDefaultAudioIODeviceType();

    if (defaultDeviceType != 0)
        list.add (defaultDeviceType);

#if JUCE_WIN32 && JUCE_ASIO
    list.add (juce_createASIOAudioIODeviceType());
#endif

#if JUCE_WIN32 && JUCE_WDM_AUDIO
    list.add (juce_createWDMAudioIODeviceType());
#endif
}

#else

void JuceAudioDeviceManager::createAudioDeviceTypes (OwnedArray <AudioIODeviceType>& list)
{
	#if JUCE_WINDOWS
	 #if JUCE_WASAPI
	 if (SystemStats::getOperatingSystemType() >= SystemStats::WinVista)
		 list.add (juce_createAudioIODeviceType_WASAPI());
	 #endif

	 #if JUCE_DIRECTSOUND
	 list.add (juce_createAudioIODeviceType_DirectSound());
	 #endif

	 #if JUCE_ASIO
	 list.add (juce_createAudioIODeviceType_ASIO());
	 #endif
	#endif

	#if JUCE_MAC
	 list.add (juce_createAudioIODeviceType_CoreAudio());
	#endif

	#if JUCE_IPHONE
	 list.add (juce_createAudioIODeviceType_iPhoneAudio());
	#endif

	#if JUCE_LINUX && JUCE_ALSA
	 list.add (juce_createAudioIODeviceType_ALSA());
	#endif

	#if JUCE_LINUX && JUCE_JACK
	 list.add (juce_createAudioIODeviceType_JACK());
	#endif
}

#endif

const String JuceAudioDeviceManager::initialise (const int numInputChannelsNeeded,
                                             const int numOutputChannelsNeeded,
                                             const XmlElement* const e,
                                             const bool selectDefaultDeviceOnFailure,
                                             const String& preferredDefaultDeviceName,
                                             const AudioDeviceSetup* preferredSetupOptions)
{
    scanDevicesIfNeeded();

    numInputChansNeeded = numInputChannelsNeeded;
    numOutputChansNeeded = numOutputChannelsNeeded;

    if (e != 0 && e->hasTagName (T("DEVICESETUP")))
    {
        delete lastExplicitSettings;
        lastExplicitSettings = new XmlElement (*e);

        String error;
        AudioDeviceSetup setup;

        if (preferredSetupOptions != 0)
            setup = *preferredSetupOptions;

        if (e->getStringAttribute (T("audioDeviceName")).isNotEmpty())
        {
            setup.inputDeviceName = setup.outputDeviceName
                = e->getStringAttribute (T("audioDeviceName"));
        }
        else
        {
            setup.inputDeviceName = e->getStringAttribute (T("audioInputDeviceName"));
            setup.outputDeviceName = e->getStringAttribute (T("audioOutputDeviceName"));
        }

        currentDeviceType = e->getStringAttribute (T("deviceType"));
        if (currentDeviceType.isEmpty())
        {
            AudioIODeviceType* const type = findType (setup.inputDeviceName, setup.outputDeviceName);

            if (type != 0)
                currentDeviceType = type->getTypeName();
            else if (availableDeviceTypes.size() > 0)
                currentDeviceType = availableDeviceTypes[0]->getTypeName();
        }

        setup.bufferSize = e->getIntAttribute (T("audioDeviceBufferSize"));
        setup.sampleRate = e->getDoubleAttribute (T("audioDeviceRate"));

        setup.inputChannels.parseString (e->getStringAttribute (T("audioDeviceInChans"), T("11")), 2);
        setup.outputChannels.parseString (e->getStringAttribute (T("audioDeviceOutChans"), T("11")), 2);

        setup.useDefaultInputChannels = ! e->hasAttribute (T("audioDeviceInChans"));
        setup.useDefaultOutputChannels = ! e->hasAttribute (T("audioDeviceOutChans"));

        error = setAudioDeviceSetup (setup, true);

        midiInsFromXml.clear();
        forEachXmlChildElementWithTagName (*e, c, T("MIDIINPUT"))
            midiInsFromXml.add (c->getStringAttribute (T("name")));

        const StringArray allMidiIns (MidiInput::getDevices());

        for (int i = allMidiIns.size(); --i >= 0;)
            setMidiInputEnabled (allMidiIns[i], midiInsFromXml.contains (allMidiIns[i]));

        if (error.isNotEmpty() && selectDefaultDeviceOnFailure)
            error = initialise (numInputChannelsNeeded, numOutputChannelsNeeded, 0,
                                false, preferredDefaultDeviceName);

        setDefaultMidiOutput (e->getStringAttribute (T("defaultMidiOutput")));

        return error;
    }
    else
    {
        AudioDeviceSetup setup;

        if (preferredSetupOptions != 0)
        {
            setup = *preferredSetupOptions;
        }
        else if (preferredDefaultDeviceName.isNotEmpty())
        {
            for (int j = availableDeviceTypes.size(); --j >= 0;)
            {
                AudioIODeviceType* const type = availableDeviceTypes.getUnchecked(j);

                StringArray outs (type->getDeviceNames (false));

                int i;
                for (i = 0; i < outs.size(); ++i)
                {
                    if (outs[i].matchesWildcard (preferredDefaultDeviceName, true))
                    {
                        setup.outputDeviceName = outs[i];
                        break;
                    }
                }

                StringArray ins (type->getDeviceNames (true));

                for (i = 0; i < ins.size(); ++i)
                {
                    if (ins[i].matchesWildcard (preferredDefaultDeviceName, true))
                    {
                        setup.inputDeviceName = ins[i];
                        break;
                    }
                }
            }
        }

        insertDefaultDeviceNames (setup);
        return setAudioDeviceSetup (setup, false);
    }
}

void JuceAudioDeviceManager::insertDefaultDeviceNames (AudioDeviceSetup& setup) const
{
    AudioIODeviceType* type = getCurrentDeviceTypeObject();
    if (type != 0)
    {
        if (setup.outputDeviceName.isEmpty())
            setup.outputDeviceName = type->getDeviceNames (false) [type->getDefaultDeviceIndex (false)];

        if (setup.inputDeviceName.isEmpty())
            setup.inputDeviceName = type->getDeviceNames (true) [type->getDefaultDeviceIndex (true)];
    }
}

XmlElement* JuceAudioDeviceManager::createStateXml() const
{
    return lastExplicitSettings != 0 ? new XmlElement (*lastExplicitSettings) : 0;
}

void JuceAudioDeviceManager::scanDevicesIfNeeded()
{
    if (listNeedsScanning)
    {
        listNeedsScanning = false;

        createDeviceTypesIfNeeded();

        for (int i = availableDeviceTypes.size(); --i >= 0;)
            availableDeviceTypes.getUnchecked(i)->scanForDevices();
    }
}

AudioIODeviceType* JuceAudioDeviceManager::findType (const String& inputName, const String& outputName)
{
    scanDevicesIfNeeded();

    for (int i = availableDeviceTypes.size(); --i >= 0;)
    {
        AudioIODeviceType* const type = availableDeviceTypes.getUnchecked(i);

        if ((inputName.isNotEmpty() && type->getDeviceNames (true).contains (inputName, true))
            || (outputName.isNotEmpty() && type->getDeviceNames (false).contains (outputName, true)))
        {
            return type;
        }
    }

    return 0;
}

/*
void JuceAudioDeviceManager::addDeviceNamesToComboBox (AComboBox& combo) const
{
    int n = 0;

    for (int i = 0; i < availableDeviceTypes.size(); ++i)
    {
        AudioIODeviceType* const type = availableDeviceTypes[i];

        if (availableDeviceTypes.size() > 1)
            combo.addSectionHeading (type->getTypeName() + T(" devices:"));

        const StringArray names (type->getDeviceNames (useInputNames));

        for (int j = 0; j < names.size(); ++j)
            combo.addItem (names[j], ++n);

        combo.addSeparator();
    }

    combo.addItem (TRANS("<< no audio device >>"), -1);
}
*/

void JuceAudioDeviceManager::addDeviceNamesToListBox (ListBoxS& box) const
{
    int n = 0;

    for (int i = 0; i < availableDeviceTypes.size(); ++i)
    {
        AudioIODeviceType* const type = availableDeviceTypes[i];

        if (availableDeviceTypes.size() > 1)
            box.addEntry(T("--==") + type->getTypeName() + T(" devices:"));
            //    combo.addSectionHeading (type->getTypeName() + T(" devices:"));

        const StringArray names (type->getDeviceNames (useInputNames));

        for (int j = 0; j < names.size(); ++j)
        {
            box.addEntry(names[j], (getCurrentAudioDevice()->getName() == names[j]));
        }
     }

    //lb.addItem (TRANS("<< no audio device >>"), -1);
}

void JuceAudioDeviceManager::getAudioDeviceSetup (AudioDeviceSetup& setup)
{
    setup = currentSetup;
}

void JuceAudioDeviceManager::deleteCurrentDevice()
{
    deleteAndZero (currentAudioDevice);
    currentSetup.inputDeviceName = String::empty;
    currentSetup.outputDeviceName = String::empty;
}

void JuceAudioDeviceManager::setCurrentAudioDeviceType (const String& type,
                                                    const bool treatAsChosenDevice)
{
    for (int i = 0; i < availableDeviceTypes.size(); ++i)
    {
        if (availableDeviceTypes.getUnchecked(i)->getTypeName() == type
             && currentDeviceType != type)
        {
            currentDeviceType = type;

            AudioDeviceSetup s (*lastDeviceTypeConfigs.getUnchecked(i));
            insertDefaultDeviceNames (s);

            setAudioDeviceSetup (s, treatAsChosenDevice);

            sendChangeMessage (this);
            break;
        }
    }
}

AudioIODeviceType* JuceAudioDeviceManager::getCurrentDeviceTypeObject() const
{
    for (int i = 0; i < availableDeviceTypes.size(); ++i)
        if (availableDeviceTypes[i]->getTypeName() == currentDeviceType)
            return availableDeviceTypes[i];

    return availableDeviceTypes[0];
}

AudioIODeviceType* JuceAudioDeviceManager::getDeviceTypeObjectByDeviceName(const String& objTitle)
{
    for (int i = 0; i < availableDeviceTypes.size(); ++i)
    {
        if(availableDeviceTypes[i]->getDeviceNames().contains(objTitle))
        {
            return availableDeviceTypes[i];
        }
    }

    return availableDeviceTypes[0];
}

const String JuceAudioDeviceManager::setAudioDeviceSetup (const AudioDeviceSetup& newSetup,
                                                      const bool treatAsChosenDevice)
{
    jassert (&newSetup != &currentSetup);    // this will have no effect

    if (newSetup == currentSetup && currentAudioDevice != 0)
        return String::empty;

    if (! (newSetup == currentSetup))
        sendChangeMessage (this);

    stopDevice();

    const String newInputDeviceName (numInputChansNeeded == 0 ? String::empty : newSetup.inputDeviceName);
    const String newOutputDeviceName (numOutputChansNeeded == 0 ? String::empty : newSetup.outputDeviceName);

    String error;
    AudioIODeviceType* type = getDeviceTypeObjectByDeviceName(newOutputDeviceName);

    if (type == 0 || newSetup.outputDeviceName.isEmpty())
    {
        deleteCurrentDevice();

        if (treatAsChosenDevice)
            updateXml();

        return String::empty;
    }

    if (currentSetup.outputDeviceName != newOutputDeviceName
         || currentAudioDevice == 0)
    {
        deleteCurrentDevice();
        scanDevicesIfNeeded();

        if (newOutputDeviceName.isNotEmpty()
             && ! type->getDeviceNames (false).contains (newOutputDeviceName))
        {
            return "No such device: " + newOutputDeviceName;
        }

        if (newInputDeviceName.isNotEmpty()
             && ! type->getDeviceNames (true).contains (newInputDeviceName))
        {
            return "No such device: " + newInputDeviceName;
        }

        currentAudioDevice = type->createDevice (newOutputDeviceName, newInputDeviceName);

        if (currentAudioDevice == 0)
            error = "Can't open the audio device!\n\nThis may be because another application is currently using the same device - if so, you should close any other applications and try again!";
        else
            error = currentAudioDevice->getLastError();

        if (error.isNotEmpty())
        {
            deleteCurrentDevice();
            return error;
        }

        if (newSetup.useDefaultInputChannels)
        {
            inputChannels.clear();
            inputChannels.setRange (0, numInputChansNeeded, true);
        }

        if (newSetup.useDefaultOutputChannels)
        {
            outputChannels.clear();
            outputChannels.setRange (0, numOutputChansNeeded, true);
        }

        if (newInputDeviceName.isEmpty())
            inputChannels.clear();

        if (newOutputDeviceName.isEmpty())
            outputChannels.clear();
    }

    if (! newSetup.useDefaultInputChannels)
        inputChannels = newSetup.inputChannels;

    if (! newSetup.useDefaultOutputChannels)
        outputChannels = newSetup.outputChannels;

    currentSetup = newSetup;

    currentSetup.sampleRate = chooseBestSampleRate (newSetup.sampleRate);

    error = currentAudioDevice->open (inputChannels,
                                      outputChannels,
                                      currentSetup.sampleRate,
                                      currentSetup.bufferSize);

    if (error.isEmpty())
    {
        currentDeviceType = currentAudioDevice->getTypeName();

        currentAudioDevice->start (&callbackHandler);

        currentSetup.sampleRate = currentAudioDevice->getCurrentSampleRate();
        currentSetup.bufferSize = currentAudioDevice->getCurrentBufferSizeSamples();
        currentSetup.inputChannels = currentAudioDevice->getActiveInputChannels();
        currentSetup.outputChannels = currentAudioDevice->getActiveOutputChannels();

        for (int i = 0; i < availableDeviceTypes.size(); ++i)
            if (availableDeviceTypes.getUnchecked (i)->getTypeName() == currentDeviceType)
                *(lastDeviceTypeConfigs.getUnchecked (i)) = currentSetup;

        if (treatAsChosenDevice)
            updateXml();
    }
    else
    {
        deleteCurrentDevice();
    }

    return error;
}

double JuceAudioDeviceManager::chooseBestSampleRate (double rate) const
{
    jassert (currentAudioDevice != 0);

    if (rate > 0)
    {
        bool ok = false;

        for (int i = currentAudioDevice->getNumSampleRates(); --i >= 0;)
        {
            const double sr = currentAudioDevice->getSampleRate (i);

            if (sr == rate)
                ok = true;
        }

        if (! ok)
            rate = 0;
    }

    if (rate == 0)
    {
        double lowestAbove44 = 0.0;

        for (int i = currentAudioDevice->getNumSampleRates(); --i >= 0;)
        {
            const double sr = currentAudioDevice->getSampleRate (i);

            if (sr >= 44100.0 && (lowestAbove44 == 0 || sr < lowestAbove44))
                lowestAbove44 = sr;
        }

        if (lowestAbove44 == 0.0)
            rate = currentAudioDevice->getSampleRate (0);
        else
            rate = lowestAbove44;
    }

    return rate;
}

void JuceAudioDeviceManager::stopDevice()
{
    if (currentAudioDevice != 0)
        currentAudioDevice->stop();
}

void JuceAudioDeviceManager::closeAudioDevice()
{
    stopDevice();
    deleteAndZero (currentAudioDevice);
}

void JuceAudioDeviceManager::restartLastAudioDevice()
{
    if (currentAudioDevice == 0)
    {
        if (currentSetup.inputDeviceName.isEmpty()
              && currentSetup.outputDeviceName.isEmpty())
        {
            // This method will only reload the last device that was running
            // before closeAudioDevice() was called - you need to actually open
            // one first, with setAudioDevice().
            jassertfalse
            return;
        }

        AudioDeviceSetup s (currentSetup);
        setAudioDeviceSetup (s, false);
    }
}

void JuceAudioDeviceManager::updateXml()
{
    delete lastExplicitSettings;

    lastExplicitSettings = new XmlElement (T("DEVICESETUP"));

    lastExplicitSettings->setAttribute (T("deviceType"), currentDeviceType);
    lastExplicitSettings->setAttribute (T("audioOutputDeviceName"), currentSetup.outputDeviceName);
    lastExplicitSettings->setAttribute (T("audioInputDeviceName"), currentSetup.inputDeviceName);

    if (currentAudioDevice != 0)
    {
        lastExplicitSettings->setAttribute (T("audioDeviceRate"), currentAudioDevice->getCurrentSampleRate());

        if (currentAudioDevice->getDefaultBufferSize() != currentAudioDevice->getCurrentBufferSizeSamples())
            lastExplicitSettings->setAttribute (T("audioDeviceBufferSize"), currentAudioDevice->getCurrentBufferSizeSamples());

        if (! currentSetup.useDefaultInputChannels)
            lastExplicitSettings->setAttribute (T("audioDeviceInChans"), currentSetup.inputChannels.toString (2));

        if (! currentSetup.useDefaultOutputChannels)
            lastExplicitSettings->setAttribute (T("audioDeviceOutChans"), currentSetup.outputChannels.toString (2));
    }

    for (int i = 0; i < enabledMidiInputs.size(); ++i)
    {
        XmlElement* const m = new XmlElement (T("MIDIINPUT"));
        m->setAttribute (T("name"), enabledMidiInputs[i]->getName());

        lastExplicitSettings->addChildElement (m);
    }

    if (midiInsFromXml.size() > 0)
    {
        // Add any midi devices that have been enabled before, but which aren't currently
        // open because the device has been disconnected.
        const StringArray availableMidiDevices (MidiInput::getDevices());

        for (int i = 0; i < midiInsFromXml.size(); ++i)
        {
            if (! availableMidiDevices.contains (midiInsFromXml[i], true))
            {
                XmlElement* const m = new XmlElement (T("MIDIINPUT"));
                m->setAttribute (T("name"), midiInsFromXml[i]);

                lastExplicitSettings->addChildElement (m);
            }
        }
    }

    if (defaultMidiOutputName.isNotEmpty())
        lastExplicitSettings->setAttribute (T("defaultMidiOutput"), defaultMidiOutputName);
}

void JuceAudioDeviceManager::addAudioCallback (AudioIODeviceCallback* newCallback)
{
    {
        const ScopedLock sl (audioCallbackLock);
        if (callbacks.contains (newCallback))
            return;
    }

    if (currentAudioDevice != 0 && newCallback != 0)
        newCallback->audioDeviceAboutToStart (currentAudioDevice);

    const ScopedLock sl (audioCallbackLock);
    callbacks.add (newCallback);
}

void JuceAudioDeviceManager::removeAudioCallback (AudioIODeviceCallback* callback)
{
    if (callback != 0)
    {
        bool needsDeinitialising = currentAudioDevice != 0;

        {
            const ScopedLock sl (audioCallbackLock);

            needsDeinitialising = needsDeinitialising && callbacks.contains (callback);
            callbacks.removeValue (callback);
        }

        if (needsDeinitialising)
            callback->audioDeviceStopped();
    }
}

void JuceAudioDeviceManager::audioDeviceIOCallbackInt (const float** inputChannelData,
                                                   int numInputChannels,
                                                   float** outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples)
{
    const ScopedLock sl (audioCallbackLock);

    if (inputLevelMeasurementEnabledCount > 0)
    {
        for (int j = 0; j < numSamples; ++j)
        {
            float s = 0;

            for (int i = 0; i < numInputChannels; ++i)
                s += fabsf (inputChannelData[i][j]);

            s /= numInputChannels;

            const double decayFactor = 0.99992;

            if (s > inputLevel)
                inputLevel = s;
            else if (inputLevel > 0.001f)
                inputLevel *= decayFactor;
            else
                inputLevel = 0;
        }
    }

    if (callbacks.size() > 0)
    {
        const double callbackStartTime = Time::getMillisecondCounterHiRes();

        tempBuffer.setSize (jmax (1, numOutputChannels), jmax (1, numSamples), false, false, true);

        callbacks.getUnchecked(0)->audioDeviceIOCallback (inputChannelData, numInputChannels,
                                                          outputChannelData, numOutputChannels, numSamples);

        float** const tempChans = tempBuffer.getArrayOfChannels();

        for (int i = callbacks.size(); --i > 0;)
        {
            callbacks.getUnchecked(i)->audioDeviceIOCallback (inputChannelData, numInputChannels,
                                                              tempChans, numOutputChannels, numSamples);

            for (int chan = 0; chan < numOutputChannels; ++chan)
            {
                const float* const src = tempChans [chan];
                float* const dst = outputChannelData [chan];

                if (src != 0 && dst != 0)
                    for (int j = 0; j < numSamples; ++j)
                        dst[j] += src[j];
            }
        }

        const double msTaken = Time::getMillisecondCounterHiRes() - callbackStartTime;
        const double filterAmount = 0.2;
        cpuUsageMs += filterAmount * (msTaken - cpuUsageMs);
    }
    else
    {
        for (int i = 0; i < numOutputChannels; ++i)
            zeromem (outputChannelData[i], sizeof (float) * numSamples);
    }

    if (testSound != 0)
    {
        const int numSamps = jmin (numSamples, testSound->getNumSamples() - testSoundPosition);
        const float* const src = testSound->getSampleData (0, testSoundPosition);

        for (int i = 0; i < numOutputChannels; ++i)
            for (int j = 0; j < numSamps; ++j)
                outputChannelData [i][j] += src[j];

        testSoundPosition += numSamps;
        if (testSoundPosition >= testSound->getNumSamples())
        {
            delete testSound;
            testSound = 0;
        }
    }
}

void JuceAudioDeviceManager::audioDeviceAboutToStartInt (AudioIODevice* const device)
{
    cpuUsageMs = 0;

    const double sampleRate = device->getCurrentSampleRate();
    const int blockSize = device->getCurrentBufferSizeSamples();

    if (sampleRate > 0.0 && blockSize > 0)
    {
        const double msPerBlock = 1000.0 * blockSize / sampleRate;
        timeToCpuScale = (msPerBlock > 0.0) ? (1.0 / msPerBlock) : 0.0;
    }

    {
        const ScopedLock sl (audioCallbackLock);
        for (int i = callbacks.size(); --i >= 0;)
            callbacks.getUnchecked(i)->audioDeviceAboutToStart (device);
    }

    sendChangeMessage (this);
}

void JuceAudioDeviceManager::audioDeviceStoppedInt()
{
    cpuUsageMs = 0;
    timeToCpuScale = 0;
    sendChangeMessage (this);

    const ScopedLock sl (audioCallbackLock);
    for (int i = callbacks.size(); --i >= 0;)
        callbacks.getUnchecked(i)->audioDeviceStopped();
}

double JuceAudioDeviceManager::getCpuUsage() const
{
    return jlimit (0.0, 1.0, timeToCpuScale * cpuUsageMs);
}

void JuceAudioDeviceManager::setMidiInputEnabled (const String& name,
                                              const bool enabled)
{
    if (enabled != isMidiInputEnabled (name))
    {
        if (enabled)
        {
            const int index = MidiInput::getDevices().indexOf (name);

            if (index >= 0)
            {
                MidiInput* const min = MidiInput::openDevice (index, &callbackHandler);

                if (min != 0)
                {
                    enabledMidiInputs.add (min);
                    min->start();
                }
            }
        }
        else
        {
            for (int i = enabledMidiInputs.size(); --i >= 0;)
                if (enabledMidiInputs[i]->getName() == name)
                    enabledMidiInputs.remove (i);
        }

        updateXml();
        sendChangeMessage (this);
    }
}

bool JuceAudioDeviceManager::isMidiInputEnabled (const String& name) const
{
    for (int i = enabledMidiInputs.size(); --i >= 0;)
        if (enabledMidiInputs[i]->getName() == name)
            return true;

    return false;
}

void JuceAudioDeviceManager::addMidiInputCallback (const String& name,
                                               MidiInputCallback* callback)
{
    removeMidiInputCallback (name, callback);

    if (name.isEmpty())
    {
        midiCallbacks.add (callback);
        midiCallbackDevices.add (0);
    }
    else
    {
        for (int i = enabledMidiInputs.size(); --i >= 0;)
        {
            if (enabledMidiInputs[i]->getName() == name)
            {
                const ScopedLock sl (midiCallbackLock);
                midiCallbacks.add (callback);
                midiCallbackDevices.add (enabledMidiInputs[i]);
                break;
            }
        }
    }
}

void JuceAudioDeviceManager::removeMidiInputCallback (const String& name,
                                                  MidiInputCallback* /*callback*/)
{
    const ScopedLock sl (midiCallbackLock);

    for (int i = midiCallbacks.size(); --i >= 0;)
    {
        String objTitle;

        if (midiCallbackDevices.getUnchecked(i) != 0)
            objTitle = midiCallbackDevices.getUnchecked(i)->getName();

        if (objTitle == name)
        {
            midiCallbacks.remove (i);
            midiCallbackDevices.remove (i);
        }
    }
}

void JuceAudioDeviceManager::handleIncomingMidiMessageInt (MidiInput* source,
                                                       const MidiMessage& message)
{
    if (! message.isActiveSense())
    {
        const bool isDefaultSource = (source == 0 || source == enabledMidiInputs.getFirst());

        const ScopedLock sl (midiCallbackLock);

        for (int i = midiCallbackDevices.size(); --i >= 0;)
        {
            MidiInput* const md = midiCallbackDevices.getUnchecked(i);

            if (md == source || (md == 0 /*&& isDefaultSource*/))
                midiCallbacks.getUnchecked(i)->handleIncomingMidiMessage (source, message);
        }
    }
}

void JuceAudioDeviceManager::setDefaultMidiOutput (const String& deviceName)
{
    if (defaultMidiOutputName != deviceName)
    {
        SortedSet <AudioIODeviceCallback*> oldCallbacks;

        {
            const ScopedLock sl (audioCallbackLock);

            oldCallbacks = callbacks;

            callbacks.clear();
        }

        if (currentAudioDevice != 0)
        {
            for (int i = oldCallbacks.size(); --i >= 0;)
            {
                oldCallbacks.getUnchecked(i)->audioDeviceStopped();
            }
        }

        deleteAndZero (defaultMidiOutput);

        defaultMidiOutputName = deviceName;

        if (deviceName.isNotEmpty())
        {
            defaultMidiOutput = MidiOutput::openDevice (MidiOutput::getDevices().indexOf (deviceName));
        }

        if (currentAudioDevice != 0)
        {
            for (int i = oldCallbacks.size(); --i >= 0;)
            {
                oldCallbacks.getUnchecked(i)->audioDeviceAboutToStart (currentAudioDevice);
            }
        }

        const ScopedLock sl (audioCallbackLock);
        callbacks = oldCallbacks;

        updateXml();
        sendChangeMessage (this);
    }
}

void JuceAudioDeviceManager::CallbackHandler::audioDeviceIOCallback (const float** inputChannelData,
                                                                 int numInputChannels,
                                                                 float** outputChannelData,
                                                                 int numOutputChannels,
                                                                 int numSamples)
{
    owner->audioDeviceIOCallbackInt (inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples);
}

void JuceAudioDeviceManager::CallbackHandler::audioDeviceAboutToStart (AudioIODevice* device)
{
    owner->audioDeviceAboutToStartInt (device);
}

void JuceAudioDeviceManager::CallbackHandler::audioDeviceStopped()
{
    owner->audioDeviceStoppedInt();
}

void JuceAudioDeviceManager::CallbackHandler::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    owner->handleIncomingMidiMessageInt (source, message);
}

void JuceAudioDeviceManager::playTestSound()
{
    audioCallbackLock.enter();

    AudioSampleBuffer* oldSound = testSound;

    testSound = 0;

    audioCallbackLock.exit();

    delete oldSound;

    testSoundPosition = 0;

    if (currentAudioDevice != 0)
    {
        const double sampleRate = currentAudioDevice->getCurrentSampleRate();
        const int soundLength = (int) sampleRate;

        AudioSampleBuffer* const newSound = new AudioSampleBuffer (1, soundLength);

        float* samples = newSound->getSampleData (0);

        const double frequency = MidiMessage::getMidiNoteInHertz (80);
        const float amplitude = 0.5f;

        const double phasePerSample = double_Pi * 2.0 / (sampleRate / frequency);

        for (int i = 0; i < soundLength; ++i)
        {
            samples[i] = amplitude * (float) sin (i * phasePerSample);
        }

        newSound->applyGainRamp (0, 0, soundLength / 10, 0.0f, 1.0f);
        newSound->applyGainRamp (0, soundLength - soundLength / 4, soundLength / 4, 1.0f, 0.0f);

        const ScopedLock sl (audioCallbackLock);

        testSound = newSound;
    }
}

void JuceAudioDeviceManager::enableInputLevelMeasurement (const bool enableMeasurement)
{
    const ScopedLock sl (audioCallbackLock);

    if (enableMeasurement)
    {
        ++inputLevelMeasurementEnabledCount;
    }
    else
    {
        --inputLevelMeasurementEnabledCount;
    }

    inputLevel = 0;
}

double JuceAudioDeviceManager::getCurrentInputLevel() const
{
    jassert (inputLevelMeasurementEnabledCount > 0); // you need to call enableInputLevelMeasurement() before using this!

    return inputLevel;
}

