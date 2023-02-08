
#pragma once


#include "36_instr.h"
#include "36_vst.h"



//namespace M {


class VstInstr: public Instrument
{
protected:

            virtual SubWindow*   createWindow();

public:
            Vst2Plugin*     vst2;

            VstInstr(char* fullpath, VstInstr* vst);
    virtual ~VstInstr();
            void                addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
            void                generateData(long num_frames = 0, long mixbuffframe = 0);
            void                vstProcess(long num_frames, long buffframe);
            void                postNoteON(int note, float vol);
            void                postNoteOFF(int note, int velocity);
            void                stopAllNotes();
            void                postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
            void                deactivateTrigger(Trigger* tg);
            long                processTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
            void                checkBounds(Note* gnote, Trigger* tg, long num_frames);
            void                flowTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
            void                save(XmlElement* instrNode);
            void                load(XmlElement* instrNode);
            void                reset();
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            bool                onUpdateDisplay();

private:

            VstMidiEvent    MidiEvents[800];

            long            numevents;
            int             muteCount;
};

//}


 
