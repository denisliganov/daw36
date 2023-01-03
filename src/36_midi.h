
#pragma once

//
//    Helper MIDI host declaration. Midi host class is responsible for interaction with system MIDI devices.
//
// Anonymous               02/18/2009      Initial creation
//

#include "36_globals.h"


extern JuceMidiWrapper   *midiHost;


/* Controls how many Midi input sources chaotic can work with */
#define MAX_NUM_MIDI_IN_SRC 10

/* Defines how many instruments can be controlled by each Midi input source */
#define MAX_NUM_INSTR_PER_MIDI_IN_SRC 10



typedef struct MIDEVICE_DESCR_T
{
    unsigned char   index;
    char*           name;
}MIDEVICE_DESCR_T;

typedef struct MidiInputRecord
{
    MidiInput*  source;
    int         index;
}MidiInputRecord;


class JuceMidiWrapper;

class JuceMidiInputCallback : public MidiInputCallback
{

public:
    JuceMidiInputCallback(JuceMidiWrapper* Owner) : owner(Owner) {};
    ~JuceMidiInputCallback(){};

    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message);
    void handlePartialSysexMessage (MidiInput* source,
                                    const uint8* messageData,
                                    const int numBytesSoFar,
                                    const double timestamp);
private:
    JuceMidiWrapper* owner;
};

class JuceMidiWrapper
{
friend class JuceMidiInputCallback;

public:

    JuceMidiWrapper();
    ~JuceMidiWrapper();

    void Initialize();

    /* This method is to add instrument to listening loop and map it to particular midi IN device */
    unsigned char Attach(Instrument* instr, unsigned char juce_index);

    /* Remove an instrument from instr array and stop route MIDI events to this instr */
    void Detach(Instrument* instr);

    /* How many Midi input sources exist in a system (num of records in m_DevList) */
    unsigned char GetDeviceCount() const;

    /* returns index and description of chosen midi source */
    const MIDEVICE_DESCR_T* GetDeviceDescr(unsigned char index) const;

    /* Opens MidiSource - it starts message routing loop */
    bool OpenMidiSource(unsigned char juce_index);

    /* Close MidiSource - stops the thread and so on... */
    void CloseMidiSource(unsigned char index);

    /* Check whether MidiSource is open */
    bool isOpen(unsigned char index) const;

private:

    JuceMidiInputCallback* m_MidiInputCallback;
    /* An array of MidiIn devices. Each device can produce MIDI messages and pass it to callback */
    MidiInputRecord m_MidiInputs[MAX_NUM_MIDI_IN_SRC];

    /* Set of instrument arrays. Each instr array is dedicated to particular MidiIn device. 
       So, in theory we can manipulate several instruments by one MIDI source. It means, you can
       preview or record several instruments simultaneusly. Let's say you press a key on Midi keybd
       and it causes few instruments to produce the same note. */
    Instrument* m_InstrSlots[MAX_NUM_MIDI_IN_SRC][MAX_NUM_INSTR_PER_MIDI_IN_SRC];
    unsigned char m_InstrCount[MAX_NUM_MIDI_IN_SRC];

    /* Holds the list of strings representing all available Midi In devices. This array is filled 
       in upon object initialization */
    MIDEVICE_DESCR_T m_DevList[MAX_NUM_MIDI_IN_SRC];
    unsigned char m_DeviceCount;

    /* return index of MidiInput in the source array */
    unsigned char GetSrcIndex(MidiInput* source)
    {
        unsigned char index;

        for (index = 0; index < MAX_NUM_MIDI_IN_SRC; ++index)
        {
            if (m_MidiInputs[index].source == source)
            {
                return index;
            }
        }

        return -1;
    }
};

void    Init_MIDI();


