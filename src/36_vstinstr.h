
#pragma once


#include "36_instr.h"
#include "36_vst.h"





class VstInstr: public Instrument
{
public:

            VstInstr(char* fullpath, VstInstr* vst);
    virtual ~VstInstr();
            void                addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
            void                checkBounds(Note* gnote, Trigger* tg, long num_frames);
            virtual SubWindow*  createWindow();
            void                deactivateTrigger(Trigger* tg);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
            std::list<Param*>   getParams() { return vst2->getParams(); }
            void                generateData(long num_frames = 0, long mix_buff_frame = 0);
            bool                isLoaded() { return vst2 != NULL; }
            void                load(XmlElement* instrNode);
            bool                onUpdateDisplay();
            void                postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
            long                processTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
            void                postNoteON(int note, float vol);
            void                postNoteOFF(int note, int velocity);
            void                reset();
            void                save(XmlElement* instrNode);
            void                stopAllNotes();
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            bool                setPresetByName(std::string pname);
            bool                setPresetByIndex(long index);
            void                vstProcess(long num_frames, long buff_frame);

private:

            VstMidiEvent        MidiEvents[800];

            long                numEvents;
            int                 muteCount;
            Vst2Plugin*         vst2;
};



 
