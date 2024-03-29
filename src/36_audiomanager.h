
#pragma once

#include "36_globals.h"






class JuceAudioCallback : public AudioIODeviceCallback
{
            void                audioDeviceStopped();
            void                audioDeviceAboutToStart (AudioIODevice* device);
            void                audioDeviceIOCallback (const float** inputChannelData, int totalNumInputChannels, float** outputChannelData, int totalNumOutputChannels, int numSamples);
};



/********* End of inlined file: juce_ComboBox.h *********/

/**
    Manages the state of some audio and midi i/o devices.

    This class keeps tracks of a currently-selected audio device, through
    with which it continuously streams data from an audio callback, as well as
    one or more midi inputs.

    The idea is that your application will create one global instance of this object,
    and let it take care of creating and deleting specific types of audio devices
    internally. So when the device is changed, your callbacks will just keep running
    without having to worry about this.

    The manager can save and reload all of its device settings as XML, which
    makes it very easy for you to save and reload the audio setup of your
    application.

    And to make it easy to let the user change its settings, there's a component
    to do just that - the AudioDeviceSelectorComponent class, which contains a set of
    device selection/sample-rate/latency controls.

    To use an AudioDeviceManager, create one, and use initialise() to set it up. Then
    call addAudioCallback() to register your audio callback with it, and use that to process
    your audio data.

    The manager also acts as a handy hub for incoming midi messages, allowing a
    listener to register for messages from either a specific midi device, or from whatever
    the current default midi input device is. The listener then doesn't have to worry about
    re-registering with different midi devices if they are changed or deleted.

    And yet another neat trick is that amount of CPU time being used is measured and
    available with the getCpuUsage() method.

    The AudioDeviceManager is a ChangeBroadcaster, and will send a change message to
    listeners whenever one of its settings is changed.

    @see AudioDeviceSelectorComponent, AudioIODevice, AudioIODeviceType
*/
class JUCE_API  JuceAudioDeviceManager  : public ChangeBroadcaster
{
public:

    /** Creates a default AudioDeviceManager.

        Initially no audio device will be selected. You should call the initialise() method
        and register an audio callback with setAudioCallback() before it'll be able to
        actually make any noise.
    */
    JuceAudioDeviceManager();

    /** Destructor. */
    ~JuceAudioDeviceManager();

    /**
        This structure holds a set of properties describing the current audio setup.

        @see AudioDeviceManager::setAudioDeviceSetup()
    */
    struct JUCE_API  AudioDeviceSetup
    {
        AudioDeviceSetup();
        bool operator== (const AudioDeviceSetup& other) const;

        /** The name of the audio device used for output.
            This may be the same as the input device.
        */
        String outputDeviceName;

        /** The name of the audio device used for input.
            This may be the same as the output device.
        */
        String inputDeviceName;

        /** The current sample rate.
            This rate is used for both the input and output devices.
        */
        double sampleRate;

        /** The buffer size, in samples.
            This buffer size is used for both the input and output devices.
        */
        int bufferSize;

        /** The set of active input channels.
            The bits that are set in this array indicate the channels of the
            input device that are active.
        */
        BitArray inputChannels;

        /** If this is true, it indicates that the inputChannels array
            should be ignored, and instead, the device's default channels
            should be used.
        */
        bool useDefaultInputChannels;

        /** The set of active output channels.
            The bits that are set in this array indicate the channels of the
            input device that are active.
        */
        BitArray outputChannels;

        /** If this is true, it indicates that the outputChannels array
            should be ignored, and instead, the device's default channels
            should be used.
        */
        bool useDefaultOutputChannels;
    };

    /** Opens a set of audio devices ready for use.

        This will attempt to open either a default audio device, or one that was
        previously saved as XML.

        @param numInputChannelsNeeded       a minimum number of input channels needed
                                            by your app.
        @param numOutputChannelsNeeded      a minimum number of output channels to open
        @param savedState                   either a previously-saved state that was produced
                                            by createStateXml(), or 0 if you want the manager
                                            to choose the best device to open.
        @param selectDefaultDeviceOnFailure if true, then if the device specified in the XML
                                            fails to open, then a default device will be used
                                            instead. If false, then on failure, no device is
                                            opened.
        @param preferredDefaultDeviceName   if this is not empty, and there's a device with this
                                            name, then that will be used as the default device
                                            (assuming that there wasn't one specified in the XML).
                                            The string can actually be a simple wildcard, containing "*"
                                            and "?" characters
        @param preferredSetupOptions        if this is non-null, the structure will be used as the
                                            set of preferred settings when opening the device. If you
                                            use this parameter, the preferredDefaultDeviceName
                                            field will be ignored

        @returns an error message if anything went wrong, or an empty string if it worked ok.
    */
    const String initialise (const int numInputChannelsNeeded,
                             const int numOutputChannelsNeeded,
                             const XmlElement* const savedState,
                             const bool selectDefaultDeviceOnFailure,
                             const String& preferredDefaultDeviceName = String::empty,
                             const AudioDeviceSetup* preferredSetupOptions = 0);

    /** Returns some XML representing the current state of the manager.

        This stores the current device, its samplerate, block size, etc, and
        can be restored later with initialise().
    */
    XmlElement* createStateXml() const;

    /** Returns the current device properties that are in use.

        @see setAudioDeviceSetup
    */
    void getAudioDeviceSetup (AudioDeviceSetup& setup);

    /** Changes the current device or its settings.

        If you want to change a device property, like the current sample rate or
        block size, you can call getAudioDeviceSetup() to retrieve the current
        settings, then tweak the appropriate fields in the AudioDeviceSetup structure,
        and pass it back into this method to apply the new settings.

        @param newSetup             the settings that you'd like to use
        @param treatAsChosenDevice  if this is true and if the device opens correctly, these new
                                    settings will be taken as having been explicitly chosen by the
                                    user, and the next time createStateXml() is called, these settings
                                    will be returned. If it's false, then the device is treated as a
                                    temporary or default device, and a call to createStateXml() will
                                    return either the last settings that were made with treatAsChosenDevice
                                    as true, or the last XML settings that were passed into initialise().
        @returns an error message if anything went wrong, or an empty string if it worked ok.

        @see getAudioDeviceSetup
    */
    const String setAudioDeviceSetup (const AudioDeviceSetup& newSetup,
                                      const bool treatAsChosenDevice);

    /** Returns the currently-active audio device. */
    AudioIODevice* getCurrentAudioDevice() const throw()                { return currentAudioDevice; }

    /** Returns the type of audio device currently in use.
        @see setCurrentAudioDeviceType
    */
    const String getCurrentAudioDeviceType() const throw()              { return currentDeviceType; }

    /** Changes the class of audio device being used.

        This switches between, e.g. ASIO and DirectSound. On the Mac you probably won't ever call
        this because there's only one type: CoreAudio.

        For a list of types, see getAvailableDeviceTypes().
    */
    void setCurrentAudioDeviceType (const String& type,
                                    const bool treatAsChosenDevice);

    /** Closes the currently-open device.

        You can call restartLastAudioDevice() later to reopen it in the same state
        that it was just in.
    */
    void closeAudioDevice();

    /** Tries to reload the last audio device that was running.

        Note that this only reloads the last device that was running before
        closeAudioDevice() was called - it doesn't reload any kind of saved-state,
        and can only be called after a device has been opened with SetAudioDevice().

        If a device is already open, this call will do nothing.
    */
    void restartLastAudioDevice();

    /** Registers an audio callback to be used.

        The manager will redirect callbacks from whatever audio device is currently
        in use to all registered callback objects. If more than one callback is
        active, they will all be given the same input data, and their outputs will
        be summed.

        If necessary, this method will invoke audioDeviceAboutToStart() on the callback
        object before returning.

        To remove a callback, use removeAudioCallback().
    */
    void addAudioCallback (AudioIODeviceCallback* newCallback);

    /** Deregisters a previously added callback.

        If necessary, this method will invoke audioDeviceStopped() on the callback
        object before returning.

        @see addAudioCallback
    */
    void removeAudioCallback (AudioIODeviceCallback* callback);

    /** Returns the average proportion of available CPU being spent inside the audio callbacks.

        Returns a value between 0 and 1.0
    */
    double getCpuUsage() const;

    /** Enables or disables a midi input device.

        The list of devices can be obtained with the MidiInput::getDevices() method.

        Any incoming messages from enabled input devices will be forwarded on to all the
        listeners that have been registered with the addMidiInputCallback() method. They
        can either register for messages from a particular device, or from just the
        "default" midi input.

        Routing the midi input via an AudioDeviceManager means that when a listener
        registers for the default midi input, this default device can be changed by the
        manager without the listeners having to know about it or re-register.

        It also means that a listener can stay registered for a midi input that is disabled
        or not present, so that when the input is re-enabled, the listener will start
        receiving messages again.

        @see addMidiInputCallback, isMidiInputEnabled
    */
    void setMidiInputEnabled (const String& midiInputDeviceName,
                              const bool enabled);

    /** Returns true if a given midi input device is being used.

        @see setMidiInputEnabled
    */
    bool isMidiInputEnabled (const String& midiInputDeviceName) const;

    /** Registers a listener for callbacks when midi events arrive from a midi input.

        The device name can be empty to indicate that it wants events from whatever the
        current "default" device is. Or it can be the name of one of the midi input devices
        (see MidiInput::getDevices() for the names).

        Only devices which are enabled (see the setMidiInputEnabled() method) will have their
        events forwarded on to listeners.
    */
    void addMidiInputCallback (const String& midiInputDeviceName,
                               MidiInputCallback* callback);

    /** Removes a listener that was previously registered with addMidiInputCallback().
    */
    void removeMidiInputCallback (const String& midiInputDeviceName,
                                  MidiInputCallback* callback);

    /** Sets a midi output device to use as the default.

        The list of devices can be obtained with the MidiOutput::getDevices() method.

        The specified device will be opened automatically and can be retrieved with the
        getDefaultMidiOutput() method.

        Pass in an empty string to deselect all devices. For the default device, you
        can use MidiOutput::getDevices() [MidiOutput::getDefaultDeviceIndex()].

        @see getDefaultMidiOutput, getDefaultMidiOutputName
    */
    void setDefaultMidiOutput (const String& deviceName);

    /** Returns the name of the default midi output.

        @see setDefaultMidiOutput, getDefaultMidiOutput
    */
    const String getDefaultMidiOutputName() const throw()           { return defaultMidiOutputName; }

    /** Returns the current default midi output device.

        If no device has been selected, or the device can't be opened, this will
        return 0.

        @see getDefaultMidiOutputName
    */
    MidiOutput* getDefaultMidiOutput() const throw()                { return defaultMidiOutput; }

    /** Just adds the list of device names to a combo box.

        The only reason this is in this class is so that it can divide DSound
        and ASIO devices into labelled sections, which makes it look much neater.
    */
 //   void addDeviceNamesToComboBox (AComboBox& combo) const;

    void addDeviceNamesToListBox(ListBoxS& lb) const;

    /** Returns a list of the types of device supported.
    */
    const OwnedArray <AudioIODeviceType>& getAvailableDeviceTypes();

    /** Creates a list of available types.

        This will add a set of new AudioIODeviceType objects to the specified list, to
        represent each available types of device.

        You can override this if your app needs to do something specific, like avoid
        using DirectSound devices, etc.
    */
    virtual void createAudioDeviceTypes (OwnedArray <AudioIODeviceType>& types);

    /** Plays a beep through the current audio device.

        This is here to allow the audio setup UI panels to easily include a "test"
        button so that the user can check where the audio is coming from.
    */
    void playTestSound();

    /** Turns on level-measuring.

        When enabled, the device manager will measure the peak input level
        across all channels, and you can get this level by calling getCurrentInputLevel().

        This is mainly intended for audio setup UI panels to use to create a mic
        level display, so that the user can check that they've selected the right
        device.

        A simple filter is used to make the level decay smoothly, but this is
        only intended for giving rough feedback, and not for any kind of accurate
        measurement.
    */
    void enableInputLevelMeasurement (const bool enableMeasurement);

    /** Returns the current input level.

        To use this, you must first enable it by calling enableInputLevelMeasurement().

        See enableInputLevelMeasurement() for more info.
    */
    double getCurrentInputLevel() const;

	AudioIODeviceType* getDeviceTypeObjectByDeviceName(const String& objTitle);

    juce_UseDebuggingNewOperator

private:

    OwnedArray <AudioIODeviceType> availableDeviceTypes;
    OwnedArray <AudioDeviceSetup> lastDeviceTypeConfigs;

    AudioDeviceSetup currentSetup;
    AudioIODevice* currentAudioDevice;
    SortedSet <AudioIODeviceCallback*> callbacks;
    int numInputChansNeeded, numOutputChansNeeded;
    String currentDeviceType;
    BitArray inputChannels, outputChannels;
    XmlElement* lastExplicitSettings;
    mutable bool listNeedsScanning;
    bool useInputNames;
    int inputLevelMeasurementEnabledCount;
    double inputLevel;
    AudioSampleBuffer* testSound;
    int testSoundPosition;
    AudioSampleBuffer tempBuffer;

    StringArray midiInsFromXml;
    OwnedArray <MidiInput> enabledMidiInputs;
    Array <MidiInputCallback*> midiCallbacks;
    Array <MidiInput*> midiCallbackDevices;
    String defaultMidiOutputName;
    MidiOutput* defaultMidiOutput;
    CriticalSection audioCallbackLock, midiCallbackLock;

    double cpuUsageMs, timeToCpuScale;

    class CallbackHandler  : public AudioIODeviceCallback,
                             public MidiInputCallback
    {
    public:
        JuceAudioDeviceManager* owner;

        void audioDeviceIOCallback (const float** inputChannelData,
                                    int totalNumInputChannels,
                                    float** outputChannelData,
                                    int totalNumOutputChannels,
                                    int numSamples);

        void audioDeviceAboutToStart (AudioIODevice*);

        void audioDeviceStopped();

        void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message);
    };

    CallbackHandler callbackHandler;
    friend class CallbackHandler;

    void audioDeviceIOCallbackInt (const float** inputChannelData,
                                   int totalNumInputChannels,
                                   float** outputChannelData,
                                   int totalNumOutputChannels,
                                   int numSamples);
    void audioDeviceAboutToStartInt (AudioIODevice* const device);
    void audioDeviceStoppedInt();

    void handleIncomingMidiMessageInt (MidiInput* source, const MidiMessage& message);

    const String restartDevice (int blockSizeToUse, double sampleRateToUse,
                                const BitArray& ins, const BitArray& outs);
    void stopDevice();

    void updateXml();

    void createDeviceTypesIfNeeded();
    void scanDevicesIfNeeded();
    void deleteCurrentDevice();
    double chooseBestSampleRate (double preferred) const;
    AudioIODeviceType* getCurrentDeviceTypeObject() const;

    void insertDefaultDeviceNames (AudioDeviceSetup& setup) const;

    AudioIODeviceType* findType (const String& inputName, const String& outputName);

    JuceAudioDeviceManager (const JuceAudioDeviceManager&);
    const JuceAudioDeviceManager& operator= (const JuceAudioDeviceManager&);
};


