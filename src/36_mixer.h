

#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_config.h"




class Mixer : public Gobj
{
public:
            Mixer();
            ~Mixer();

            MixChannel*         addMixChannel(Instrument* instr);
            void                cleanBuffers(int num_frames);
            void                drawSelf(Graphics& g);
            Eff*                getCurrentEffect()          { return currentEffect; }
            MixChannel*         getMasterChannel()          { return masterChannel; }
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            void                init();
            void                mixAll(int num_frames);
            void                remap();
            void                resetAll();
            void                setCurrentEffect(Eff* eff);
            void                updateChannelIndexes();
            void                removeMixChannel(Instrument * instr);

protected:
            Eff*                currentEffect;
            MixChannel*         masterChannel;
            MixChannel*         sendChannel[NUM_SENDS];
};


