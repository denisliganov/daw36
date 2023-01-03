

#include "36.h"
#include "36_instr.h"
#include "36_controls.h"
#include "36_events_triggers.h"
#include "36_utils.h"
#include "36_system.h"
#include "36_element.h"
#include "36_pattern.h"
#include "36_params.h"
#include "36_audio_dev.h"
#include "36_instrpanel.h"
#include "36_grid.h"
#include "36_ctrlpanel.h"
#include "36_transport.h"






Trigger::Trigger(Element* elem, Pattern* pattSon, Trigger* tgStarter)
{
    tgworking = starter = muted = broken = toberemoved = outsync = false;
    aaIN = aaOUT = false;
    previewTrigger = false;
    el = NULL;
    tgPatt = NULL;
    group_prev = group_next = NULL;
    noteVal = -1;
    ev = NULL;
    tgsparent = tgsactive = NULL;
    freq_incr_sgn = 1;
    vol_val = pan_val =  0;
    framePhase = 0;
    freq_incr_base = freq_incr_active = 0;
    wt_pos = 0;
    lcount = 0;

    setState(TS_Initial);

    el = elem;

    tgPatt = pattSon;

    if(tgStarter == NULL)
    {
        starter = true;

        tgStart = NULL;
    }
    else
    {
        starter = false;

        tgStart = tgStarter;
    }
}

void Trigger::start(long frame)
{
    if(isActive() == false)
    {
        toberemoved = false;
        tgworking = true;

        switch(el->type)
        {
            case El_SampleNote:
            case El_GenNote:
            {
                Note* note = (Note*)el;

                note->getInstr()->activateTrigger(this);
            }

            break;
        }

        setFrame(frame);

        MAudio->activeTriggers.push_back(this);
    }
}

void Trigger::stop()
{
    if(isActive())
    {
        tgworking = false;

        switch(el->type)
        {
            case El_SampleNote:
            case El_GenNote:

                if(((Note*)el)->getInstr() != NULL)
                {
                    ((Note*)el)->getInstr()->deactivateTrigger(this);
                }

                break;

            case El_Pattern:

                ((Pattern*)el)->deactivate();

                break;
        }

        MAudio->activeTriggers.remove(this);
    }
}

void Trigger::setState(TgState state)
{
    if (state == TS_SoftFinish)
        int dbg = 1;

    if (state == TS_Release)
        int dbg = 2;

    if (state == TS_Sustain)
        int dbg = 2;

    if (tgState == TS_Finished)
        int dbg = 2;

    tgState = state;
}

bool Trigger::isActive()
{
    return tgworking;
}

long Trigger::getFrame()
{
    return framePhase;
}

void Trigger::setFrame(long frame)
{
    framePhase = frame;
}

bool Trigger::isOutOfPlayback()
{
    if(ev->evpatt->getFrame() < ev->ev_frame || ev->evpatt->getFrame() > ev->ev_frame + el->frameLength)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Trigger::checkRemoval()
{
    if(toberemoved == true || el->isDeleted())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Trigger::locate()
{
    tgPatt->placeTrigger(this);
}

void Trigger::relocate()
{
    removeFromEvent();

    locate();
}

void Trigger::removeFromEvent()
{
    tgPatt->removeTrigger(this);
}

Event::Event(float tk, Pattern* pt)
{
    jassert(pt != NULL);

    setTick(tk);
    evpatt = pt;
}

void Event::addTrigger(Trigger* tg)
{
    tg->ev = this;

    triggers.push_back(tg);
}

//
// Returns true if last trigger removed
//
bool Event::removeTrigger(Trigger* tg)
{
    triggers.remove(tg);

    if(triggers.size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Event::activateTriggers(bool deactonly)
{
    for(Trigger* tg : triggers)
    {
        if(!tg->tgPatt->muted && !tg->el->isDeleted())
        {
            if(tg->starter && (tick < 0 || tick > tg->tgPatt->getTickLength()))
            {
                // skip activators that are out of pattern bounds
            }
            else if( !(deactonly && tg->starter) ) // skip starters if asked
            {
                if(tg->el->isNote() && !(tg->el->type == El_Pattern))
                {
                    if(tg->starter)
                    {
                        Note* ii = (Note*)tg->el;

                        if(tg->isActive())
                        {
                            ii->getInstr()->flowTriggers(tg, tg);

                            if(tg->isActive())  tg->stop();     // it could change 
                        }

                         tg->start(0);
                    }
                    else if(tg->tgStart->isActive())
                    {
                        tg->tgStart->stop();
                    }
                }
            }
        }
    }
}

void Event::setTick(float nTick)
{
    tick = nTick;

    ev_frame = MTransp != NULL ? MTransp->getFrameFromTick(nTick) : 0;
}

long Event::getFrame()
{
    return ev_frame;
}




