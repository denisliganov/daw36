
#pragma once

#include "36_globals.h"
#include "36_env.h"



typedef enum
{
    TS_Initial,
    TS_Sustain,
    TS_Release,
    TS_SoftFinish,
    TS_Finished
}TgState;



class Trigger
{
public:

            Trigger(Element* elem, Pattern* pattSon, Trigger* starter);

            bool                starter;            // Whether this trigger activates or deactivates an element
            bool                previewTrigger;
            bool                muted;
            bool                broken;             // Interrupted by break or other symbol
            bool                aaIN;               // Antialiasing helpers
            bool                aaOUT;              // Antialiasing helpers
            bool                outsync;            // When this trigger is outsync'ed with playback (per lining for example)
            bool                tgworking;
            bool                toberemoved;
            TgState             tgState;
            Event*              ev;
            Element*            el;
            Trigger*            tgStart;            // Reference to the activator if this is a deactivator, otherwise NULL
            Trigger*            tgsactive;
            Trigger*            tgsparent;
            Pattern*            tgPatt;
            EnvPoint*           ep1;
            float               lcount;
            float               volValue;
            float               panValue;
            float               volBase;
            float               panBase;
            float               freq;               // Constant part of the frequency
            float               auxbuff[1024];
            float               prev_value;
            float               aaBaseVal;
            float               cf1;
            float               cf2;
            float               envVal1;
            double              freq_incr_base;
            double              freq_incr_active;
            double              wt_pos;             // Wavetable position integer
            double              env_phase1;
            int                 aaFilledCount;
            int                 aaCount;
            int                 auCount;
            int                 noteVal;
            int                 freq_incr_sgn;
            long                framePhase;

            //
            Trigger*            group_prev;         // For group lists (envelopes, slidenotes, etc.)
            Trigger*            group_next;


            void                start(long frame);
            void                stop();
            void                setState(TgState state);
            long                getFrame();
            void                locate();
            void                relocate();
            void                setFrame(long frame);
            bool                checkRemoval();
            bool                isOutOfPlayback();
            void                removeFromEvent();
            bool                isActive();
};


class Event
{
public:

            long            ev_frame;
            float           tick;
            Pattern*        evpatt;

            std::list<Trigger*> triggers;

            Event(float tk, Pattern* pt);
            void            addTrigger(Trigger* c);
            bool            removeTrigger(Trigger* c);
            void            setTick(float nTick);
            float           getTick() { return tick; }
            void            activateTriggers(bool deactonly = false);
            long            getFrame();
};




