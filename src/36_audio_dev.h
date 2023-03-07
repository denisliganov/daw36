

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

            void                addPreviewTrigger(Trigger* tg);
            float               getSampleRate();
            float               getInvertedSampleRate();
            int                 getBufferSize();
            void                handleMetronome(long framesPerBuffer, bool pre);
            void                initMetronome();
            bool                isNotePlayingOnPreview(int note_val);
            void                generalCallBack(const void* inputBuffer, void* outputBuffer, long framesPerBuffer);
            void                mixMaster(const void* inputBuffer, void* outputBuffer, long totalFrames);
            void                processPreviews();
            void                releasePreviewByElement(Element* el);
            void                releasePreviewByNote(int note_val);
            void                resetProcessing(bool resetmix);
            void                removePreview();
            void                releaseAllPreviews();
            void                setSampleRate(float smpRate);
            void                setBufferSize(unsigned bufSize);

protected:

            Sample*             barSample = NULL;
            Sample*             beatSample = NULL;

            Pattern*            prevPattern;
            Event*              previewEvent;
            Trigger*            dummyTrigger;

            bool                metroOn;
            bool                globalMute;
            bool                mixMute;

            int                 bufferSize;
            float               sampleRate;

            Parameter*           MasterVol;
};

