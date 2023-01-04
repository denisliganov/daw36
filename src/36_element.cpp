


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

    trkLine = 0;
    highlighted = false;
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

void Element::recalculate()
{
    calcFrames();

    calculated = true;

    if (patt)
    {
        relocateTriggers();
    }
}

void Element::calcFrames()
{
    tickLength = endTick - startTick;

    startFrame = MTransp != NULL ? MTransp->getFrameFromTick(startTick) : 0;

    endFrame = MTransp != NULL ? MTransp->getFrameFromTick(endTick) : 0;

    frameLength = endFrame - startFrame - 1;
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
    int x1 = grid->getXfromTick(getStartTick());
    int x2 = grid->getXfromTick(getEndTick());
    int y1 = grid->getYfromLine(getLine()) - grid->getLineHeight();
    int y2 = grid->getYfromLine(getLine());

    if(CheckPlaneCrossing(x1, y1, x2, y2, grid->getX1(), grid->getY1(), grid->getX2(), grid->getY2()))
    {
        return true;
    }

    return false;
}

void Element::setPos(float tick, int line)
{
    if(tick != startTick || line != trkLine)
    {
        startTick = tick;

        if(startTick < 0)
        {
            startTick = 0;
        }

        endTick = startTick + tickLength;

        trkLine = line;

        if(trkLine < 0)
        {
            trkLine = 0;
        }

        if (patt)
        {
            if (trkLine >= patt->numLines)
            {
                trkLine = patt->numLines - 1;
            }
        }
    }
}

void Element::move(float dtick, int dtrack)
{
    setPos(startTick + dtick, trkLine + dtrack);
}

void Element::handleMouseDown(InputEvent& ev)
{
    oldTick = getStartTick();
}

void Element::addTrigger(Trigger* tg)
{
    triggers.push_back(tg);
}

void Element::removeTrigger(Trigger* tg)
{
    triggers.remove(tg);
}

bool Element::isShown()
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
    xmlNode->setAttribute(T("getStartTick"), startTick);
    xmlNode->setAttribute(T("getEndTick"), endTick);
    xmlNode->setAttribute(T("TickLength"), tickLength);
    xmlNode->setAttribute(T("getLine"), trkLine);

    if(type != El_Pattern)
    {
    //    xmlNode->setAttribute(T("PattIndex"), patt->momIndex);
    }
}

void Element::load(XmlElement * xmlNode)
{
    startTick = (float)xmlNode->getDoubleAttribute(T("getStartTick"));
    endTick = (float)xmlNode->getDoubleAttribute(T("getEndTick"));
    tickLength = (float)xmlNode->getDoubleAttribute(T("TickLength"));
}

void Element::softDelete()
{
    markDeleted(true);

    markSelected(false);

    highlightOff();
}

void Element::markDeleted(bool del)
{
    deleted = del;
}

bool Element::isDeleted()
{
    return deleted;
}

void Element::markSelected(bool sel)
{
    selected = sel;
}

bool Element::isSelected()
{
    return selected;
}

void Element::highlightOn()
{
    highlighted = true;
}

void Element::highlightOff()
{
    highlighted = false;
}

bool Element::isHighlighted()
{
    return highlighted;
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

Pattern* Element::getBase()
{
    return patt;
}

void Element::calcCoordsForGrid(Grid* grid)
{
    x1 = grid->getXfromTick(getStartTick());
    y1 = grid->getYfromLine(getLine());
}

float Element::getStartTick()
{
    return startTick;
}

float Element::getEndTick()
{
    return endTick;
}

int Element::getLine()
{
    return trkLine;
}

long Element::getStartFrame()
{
    return startFrame;
}

long Element::getEndFrame()
{
    if(endFrame == 0)
    {
        return 2147483647;  // -1 casts "infinite" frame length
    }

    return endFrame;
}

long Element::getFrameLength()
{
    return getEndFrame() - getStartFrame();
}

void Element::setLine(int trkline)
{
    trkLine = trkline;
}

void Element::setEndTick(float endtick)
{
    endTick = endtick;
}

void Element::setTickLength(float tick_length)
{
    tickLength = tick_length;

    endTick = startTick + tickLength;
}

void Element::setTickDelta(float tick_delta)
{
    tickLength += tick_delta;

    if(tickLength < 0)
        tickLength = 0;

    endTick = startTick + tickLength;
}

float Element::getTickLength()
{
    if(tickLength == -1)
    {
        return 2147483647.f;  // -1 casts "infinite" step length
    }

    return tickLength;
}


