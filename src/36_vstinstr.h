
#pragma once


#include "36_device.h"
#include "36_vst.h"





class VstInstr: public Device36
{
public:

            VstInstr(char* fullpath, VstInstr* vst);
    virtual ~VstInstr();
            void                addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
            void                checkBounds(Note* gnote, Trigger* tg, long num_frames);
    virtual SubWindow*          createWindow();
            VstInstr*           clone();
            void                deactivateTrigger(Trigger* tg);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
        std::list<Parameter*>   getParams() { return vst2->getParams(); }
            void                generateData(float* in_buff, float* out_buff, long num_frames = 0, long mix_buff_frame = 0);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            bool                isLoaded() { return vst2 != NULL; }
            void                load(XmlElement* instrNode);
            bool                onUpdateDisplay();
            void                postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
            long                handleTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
            void                handleParamUpdate(Parameter* param = NULL);
            void                postNoteON(int note, float vol);
            void                postNoteOFF(int note, int velocity);
            void                processEvents(VstEvents *pEvents);
            void                reset();
            void                save(XmlElement* instrNode);
            void                stopAllNotes();
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            bool                setPresetByName(std::string pname);
            bool                setPresetByIndex(long index);
            void                vstProcess(float* in_buff, long num_frames, long buff_frame);

private:

            VstMidiEvent        MidiEvents[800];

            long                numEvents;

            Vst2Plugin*         vst2;
};


 
