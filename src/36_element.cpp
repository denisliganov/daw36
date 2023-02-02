


#include "36_element.h"
#include "36_sampleinstr.h"
#include "36_pattern.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_ctrlpanel.h"
#include "36_instr.h"
#include "36_utils.h"
#include "36_edit.h"
#include "36_params.h"
#include "36_menu.h"
#include "36_draw.h"
#include "36_audio_dev.h"
#include "36.h"
#include "36_transport.h"
#include "36_events_triggers.h"




Element::Element()
{
    dev = NULL;
    patt = NULL;

    line = 0;
    selected = false;
    deleted = false;
    calculated = false;
}

Element::~Element()
{
    ///
}

Element* Element::clone()
{
    return NULL;
}

Element* Element::clone(Pattern* ptBase)
{
    return NULL;
}

void Element::recalc()
{
    calcframes();

    calculated = true;

    if (patt)
    {
        relocateTriggers();
    }
}

void Element::calcframes()
{
    ticklen = tick2 - tick1;

    frame1 = MTransp != NULL ? MTransp->getFrameFromTick(tick1) : 0;

    frame2 = MTransp != NULL ? MTransp->getFrameFromTick(tick2) : 0;

    framelen = frame2 - frame1 - 1;
}

bool Element::isPointed(int mx, int my, Grid* grid)
{
    if(patt == grid->getPattern()->ptBase && checkVisible(grid))
    {
        if (mx >= dx1 && mx <= dx2 && my >= dy1 && my <= dy2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

bool Element::checkVisible(Grid* grid)
{
    int x1 = grid->getXfromTick(gettick());
    int x2 = grid->getXfromTick(getendtick());
    int y1 = grid->getYfromLine(getline()) - grid->getLineHeight();
    int y2 = grid->getYfromLine(getline());

    if(CheckPlaneCrossing(x1, y1, x2, y2, grid->getX1(), grid->getY1(), grid->getX2(), grid->getY2()))
    {
        return true;
    }

    return false;
}

void Element::setPos(float tick, int ln)
{
    if(tick != tick1 || ln != line)
    {
        tick1 = tick;

        if(tick1 < 0)
        {
            tick1 = 0;
        }

        tick2 = tick1 + ticklen;

        line = ln;

        if(line < 0)
        {
            line = 0;
        }

/*
        if (patt)
        {
            if (line >= patt->numlines)
            {
                line = patt->numlines - 1;
            }
        }*/
    }
}

void Element::move(float dtick, int dtrack)
{
    setPos(tick1 + dtick, line + dtrack);
}

void Element::handleMouseDown(InputEvent& ev)
{

}

void Element::addTrigger(Trigger* tg)
{
    triggers.push_back(tg);
}

void Element::removeTrigger(Trigger* tg)
{
    triggers.remove(tg);
}

bool Element::isshown()
{
    if(deleted || patt == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Element::save(XmlElement * xmlNode)
{
    xmlNode->setAttribute(T("Type"), int(type));
    xmlNode->setAttribute(T("gettick"), tick1);
    xmlNode->setAttribute(T("getendtick"), tick2);
    xmlNode->setAttribute(T("TickLength"), ticklen);
    xmlNode->setAttribute(T("getline"), line);

    if(type != El_Pattern)
    {
    //    xmlNode->setAttribute(T("PattIndex"), patt->momIndex);
    }
}

void Element::load(XmlElement * xmlNode)
{
    tick1 = (float)xmlNode->getDoubleAttribute(T("gettick"));
    tick2 = (float)xmlNode->getDoubleAttribute(T("getendtick"));
    ticklen = (float)xmlNode->getDoubleAttribute(T("TickLength"));
}

void Element::softdel()
{
    markDeleted(true);

    markSelected(false);
}

void Element::markDeleted(bool del)
{
    deleted = del;
}

bool Element::isdel()
{
    return deleted;
}

void Element::markSelected(bool sel)
{
    selected = sel;
}

bool Element::issel()
{
    return selected;
}

void Element::deactivateAllTriggers()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    for(Trigger* tg : triggers)
    {
        if(tg->tgworking)
        {
            tg->stop();
        }
    }

    ReleaseMutex(AudioMutex);
}

void Element::deleteAllTriggers()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    while(triggers.size() > 0)
    {
        Trigger* tg = triggers.front();

        /*
            if(tg->apatt_instance != NULL)
            {
                // Take care of autopattern playback, if any. Pattern doesn't care about this, when being deleted.

                if(tg->apatt_instance->IsPlaybackActive())
                {
                    Player_Deactivate(tg->apatt_instance->pbk);
                }
            }
        */

        tg->removeFromEvent();
        tg->stop();

        removeTrigger(tg);

        delete tg;
    }

    ReleaseMutex(AudioMutex);
}

// Creates and locate triggers for element per single child pattern
//
// Also, we add an auto-pattern instance here, if applicable
//
// [IN]
//    el - element to create triggers for
//    pt - pattern to create element's triggers at
//
void Element::propagateTriggers(Pattern* sonPatt)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Trigger* tgStart = new Trigger(this, sonPatt, NULL);

    addTrigger(tgStart);

    if(calculated)
    {
        tgStart->locate();
    }

    ReleaseMutex(AudioMutex);
}

void Element::unpropagateTriggers(Pattern* sonPatt)
{
    WaitForSingleObject(AudioMutex, INFINITE);

restart:

    for(Trigger* tg : triggers)
    {
        if(tg->tgPatt == sonPatt)
        {
            tg->stop();

            removeTrigger(tg);

            tg->removeFromEvent();

            delete tg;

            goto restart;
        }
    }

    ReleaseMutex(AudioMutex);
}

void Element::relocateTriggers()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    for(Trigger* tg : triggers)
    {
        if(!tg->previewTrigger)
        {
            tg->relocate();
        }
    }

    patt->queueEvent();

    ReleaseMutex(AudioMutex);
}

Pattern* Element::getbasepatt()
{
    return patt;
}

void Element::calcForGrid(Grid* grid)
{
    x1 = grid->getXfromTick(gettick());
    y1 = grid->getYfromLine(getline());
}

float Element::gettick()
{
    return tick1;
}

float Element::getendtick()
{
    return tick2;
}

int Element::getline()
{
    return line;
}

long Element::getframe()
{
    return frame1;
}

long Element::getendframe()
{
    if(frame2 == 0)
    {
        return 2147483647;  // -1 casts "infinite" frame length
    }

    return frame2;
}

long Element::getframes()
{
    return getendframe() - getframe();
}

void Element::setline(int trkline)
{
    line = trkline;
}

void Element::setendtick(float endtick)
{
    tick2 = endtick;
}

void Element::setTickLength(float tick_length)
{
    ticklen = tick_length;

    tick2 = tick1 + ticklen;
}

void Element::setTickDelta(float tick_delta)
{
    ticklen += tick_delta;

    if(ticklen < 0)
        ticklen = 0;

    tick2 = tick1 + ticklen;
}

void Element::setPattern(Pattern* pt)
{
    patt = pt;
}

float Element::getticklen()
{
    if(ticklen == -1)
    {
        return 2147483647.f;  // -1 casts "infinite" step length
    }

    return ticklen;
}


