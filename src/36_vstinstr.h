
#pragma once


#include "36_device.h"
#include "36_vst.h"





class Vst2Module: public Device36
{
public:

            Vst2Module(std::string fullpath);
    virtual ~Vst2Module();

            void                addNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
            void                checkBounds(Note* gnote, Trigger* tg, long num_frames);
    virtual SubWindow*          createWindow();
            Vst2Module*         clone();
            void                deactivateTrigger(Trigger* tg);
            void                fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
        std::list<Parameter*>   getParams() { return vst2->getParams(); }
            void                generateData(float* in_buff, float* out_buff, long num_frames = 0, long mix_buff_frame = 0);
            Vst2Plugin*         getVst2() { return vst2; }
            bool                isLoaded() { return vst2 != NULL; }
            void                load(XmlElement* instrNode);
            long                handleTrigger(Trigger* tg, long num_frames = 0, long buff_frame = 0);
            void                handleParamUpdate(Parameter* param = NULL);
            void                postNoteON(int note, float vol);
            void                postNoteOFF(int note, int velocity);
            void                postProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buff_frame = 0, long mix_buff_frame = 0, long remaining = 0);
            void                processDSP(float* in_buff, float* out_buff, int num_frames);
            void                processEvents(VstEvents *pEvents);
            void                reset();
            void                save(XmlElement* instrNode);
            void                stopAllNotes();
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            bool                setPreset(std::string pname);
            bool                setPreset(long index);
            void                vstProcess(float* in_buff, long num_frames, long buff_frame);

private:

            VstMidiEvent        MidiEvents[800];

            long                numEvents;

            Vst2Plugin*         vst2;
};


 
