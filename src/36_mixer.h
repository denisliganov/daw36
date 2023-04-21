

#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_config.h"


#include <list>


class Mixer : public Gobj
{
public:
            Mixer();
            ~Mixer();

            MixChannel*         addMixChannel(Instr* instr);
            void                cleanBuffers(int num_frames);
            MixChannel*         getMasterChannel()          { return masterChannel; }
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            void                init();
            void                mixAll(int num_frames);
            void                remap();
            void                resetAll();
            void                removeMixChannel(Instr * instr);
            void                updateMixingQueue();

private:
            void                drawSelf(Graphics& g);

            MixChannel*         masterChannel;

    std::list<MixChannel*>      mixQueue;
};


