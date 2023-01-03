

#pragma once


#include "36_globals.h"

#include <list>
#include <mutex>




extern HANDLE           AudioMutex;             // windep
extern HANDLE           MixerMutex;             // windep

extern std::mutex       MixMutex;

class Audio36
{
public:

            Audio36(float smpRate);
            ~Audio36();

            std::list<Pattern*>         players;
            std::list<Trigger*>         activeTriggers;
            std::list<Trigger*>         activeCommands;

            void            setSampleRate(float smpRate);
            float           getSampleRate();
            float           getInvertedSampleRate();
            void            setBufferSize(unsigned bufSize);
            int             getBufferSize();
            void            handleMetronome(long framesPerBuffer, bool pre);
            void            mixMaster(const void* inputBuffer, void* outputBuffer, long totalFrames);
            void            generalCallBack(const void* inputBuffer, void* outputBuffer, long framesPerBuffer);
            void            resetProcessing(bool resetmix);

// Preview:

            void            addPreviewTrigger(Trigger* tg);
            void            removePreview();
            void            releaseAllPreviews();
            void            processPreviews();
            void            initMetronome();
            void            releasePreviewByElement(Element* el);
            void            releasePreviewByNote(int note_val);
            bool            isNotePlayingOnPreview(int note_val);

protected:

            Sample*         barSample = NULL;
            Sample*         beatSample = NULL;

            Pattern*        prevPattern;
            Event*          previewEvent;
            Trigger*        dummyTrigger;

            bool            metroOn;
            bool            globalMute;
            bool            mixMute;

            int             bufferSize;
            float           sampleRate;

            Parameter*      MasterVol;

};

