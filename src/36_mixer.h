

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
            MixChannel*         getMasterChannel()          { return masterChannel; }
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            void                init();
            void                mixAll(int num_frames);
            void                remap();
            void                resetAll();
            void                removeMixChannel(Instrument * instr);

private:
            void                drawSelf(Graphics& g);

            MixChannel*         masterChannel;
            MixChannel*         sendChannel[NUM_SENDS];
};


