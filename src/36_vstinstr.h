
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
            void                checkBounds(Note* gnote, Trigger* tg, long num_frames);
            void                deactivateTrigger(Trigger* tg);
            void                flowTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
            void                generateData(long num_frames = 0, long mixbuffframe = 0);
            void                load(XmlElement* instrNode);
            bool                onUpdateDisplay();
            void                postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
            long                processTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
            void                postNoteON(int note, float vol);
            void                postNoteOFF(int note, int velocity);
            void                reset();
            void                save(XmlElement* instrNode);
            void                stopAllNotes();
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            void                vstProcess(long num_frames, long buffframe);

private:

            VstMidiEvent        MidiEvents[800];

            long                numEvents;
            int                 muteCount;
};

//}


 
