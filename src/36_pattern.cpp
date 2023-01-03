


#include "36_pattern.h"
#include "36_params.h"
#include "36_textinput.h"
#include "36_instrpanel.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_edit.h"
#include "36_instr.h"
#include "36_utils.h"
#include "36_project.h"
#include "36_draw.h"
#include "36_transport.h"
#include "36_events_triggers.h"
#include "36_audio_dev.h"




Pattern::Pattern(char* nm, float tk1, float tk2, int tr1, int tr2, bool tracks)
{
    startTick = tk1;
    endTick = tk2;

    init(nm, tracks);
}

Pattern::~Pattern()
{
    ///
}

void Pattern::init(char* nm, bool tracks)
{
    type = El_Pattern;

    numLines = 0;
    currFrame = 0;
    playTick = 0;
    startFrame = endFrame = 0;
    frameCount = -1;

    ranged = false;
    muted = false;
    looped = false;
    playing = false;

    numLines = NUM_PATTERN_LINES;

    pendingEvent = events.end();

    ptBase = NULL;

    adjustBounds();

    queueEvent();
}

Pattern* Pattern::clone(float new_tick, int new_trackline)
{
    //Pattern* clone = Create_Pattern_Instance(ptBase, new_tick, new_tick + tickLength, new_trackline, new_trackline + numLines, patt);

    return NULL;
}

Pattern* Pattern::clone()
{
    Pattern* cl = clone(startTick, trkLine);
    
    return cl;
}

void Pattern::copyParams(Pattern* newPatt)
{
    newPatt->vol->setValue(vol->value);
    newPatt->pan->setValue(pan->value);
    newPatt->patt = patt;
    newPatt->muted = muted;
}

void Pattern::move(float dtick, int dtrack)
{
    startTick += dtick;
    endTick = startTick + tickLength;
    trkLine += dtrack;
}

bool Pattern::checkVisible(Grid* grid)
{
    int x1 = grid->getXfromTick(getStartTick());
    int x2 = grid->getXfromTick(getEndTick());

    int y1 = grid->getYfromLine(getLine()) - grid->getLineHeight()*2;
    int y2 = grid->getYfromLine(getLine()) - grid->getLineHeight();

    if(CheckPlaneCrossing(x1, y1, x2, y2, grid->getX1(), grid->getY1(), grid->getX2(), grid->getY2()))
    {
        return true;
    }

    return false;
}

void Pattern::deleteAllElements(bool flush, bool preventundo)
{
restart:

    for(Element* el : elems)
    {
        Delete_Element(el);

        goto restart;
    }
}

Element* Pattern::checkElement(float tick, int trknum)
{
    for(Element* el : elems)
    {
        if(!el->isDeleted() && el->startTick == tick && el->trkLine == trknum)
        {
            return el;
        }
    }

    return NULL;
}

void Pattern::recalculate()
{
    calcFrames();

    for(Element* el : elems)
    {
        if(!el->isDeleted())
        {
            el->recalculate();
        }
    }

    calcNoteFreq();

    relocateTriggers();
}

void Pattern::addElement(Element* el)
{
    if(ptBase != NULL && ptBase != this)
    {
        // This is a son, add to actual mom

        ptBase->addElement(el);
    }
    else
    {
        // This is mom, add element and propagate to all sons

        elems.push_back(el);

        el->patt = this;

        for (Pattern* ptinst : instances)
        {
            el->propagateTriggers(ptinst);
        }
    }
}

void Pattern::removeElement(Element* el)
{
    elems.remove(el);

    for (Pattern* pt : instances)
    {
        el->unpropagateTriggers(pt);
    }
}

void Pattern::addInstance(Pattern * pt)
{
    instances.push_back(pt);

    pt->ptBase = this;

    for(Element* el : elems)
    {
        el->propagateTriggers(pt);
    }
}

void Pattern::removeInstance(Pattern* pt)
{
    if(pt->ptBase == this)
    {
        instances.remove(pt);

        for(Element* el : elems)
        {
            el->unpropagateTriggers(pt);
        }
    }
}

void Pattern::save(XmlElement * xmlNode)
{
    Element::save(xmlNode);

    xmlNode->setAttribute(T("Volume"), vol->value);
    xmlNode->setAttribute(T("Panning"), pan->value);
    xmlNode->setAttribute(T("Muted"), muted ? 1 : 0);
}

void Pattern::load(XmlElement * xmlNode)
{
    Element::load(xmlNode);

    vol->setValue((float)xmlNode->getDoubleAttribute(T("Volume")));
    pan->setValue((float)xmlNode->getDoubleAttribute(T("Panning")));
    muted = xmlNode->getBoolAttribute(T("Muted"));
}

long Pattern::getLastElementFrame()
{
    long endFrame = 0;

    for(Element* el : elems)
    {
        if (!el->isDeleted() && el->endFrame > endFrame)
        {
            endFrame = el->endFrame;
        }
    }

    return endFrame;
}

void Pattern::handleMouseUp(InputEvent& ev)
{
    Element::handleMouseUp(ev);
}

void Pattern::drawOnGrid(Graphics& g, Grid * grid)
{
    calcCoords(grid);

    int gx1 = x1;
    int gy1 = y1;

    int gx2 = grid->getXfromTick(getEndTick());
    int gy2 = grid->getYfromLine(trkLine + numLines - 1);

    int pl = (int)(tickLength*grid->getPixelsPerTick());
    int tH, ptheight;


    tH = grid->getLineHeight()*1;
    ptheight = grid->getLineHeight()*1 - 2;

    //g.setColour(Colour(0xff000000));
    //gDrawImageRegion(g, ptBase->smallimg, gx1, gy1 - tH + 1, pl, tH + 5);

    if(muted)
    {
        g.setColour(Colour(0x5a000000));

        gFillRect(g, gx1, gy1 - tH + 1, gx1 + pl, gy1 - tH + ptheight + 1);
    }

    if(muted == true)
    {
        g.setColour(Colour(0x9fFFFFFF)); // Darken muted pattern names
    }
    else
    {
        g.setColour(Colour(0xffFFFFFF));
    }

    setDispArea(gx1, gy2 - grid->getLineHeight() + 1, grid->getXfromTick(endTick) - 1, gy2 - 1);
}


// Below former Player methods


void Pattern::setLoop()
{
    looped = true;
}

void Pattern::resetLoop()
{
    looped = false;
}

void Pattern::activate()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    playing = true;

    queueEvent();

    MAudio->players.push_back(this);

    ReleaseMutex(AudioMutex);
}

void Pattern::deactivate()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    playing = false;

    MAudio->players.remove(this);

    frameCount = endFrame;

    ReleaseMutex(AudioMutex);
}

double Pattern::getPlayTick()
{
    return playTick;
}

void Pattern::setPlayTick(double tick)
{
    playTick = tick;

    currFrame = MTransp->getFrameFromTick((float)playTick);

    queueEvent();
}

void Pattern::resetPosition()
{
    currFrame = startFrame;

    queueEvent();
}

void Pattern::setFrame(long frame)
{
    currFrame = frame;

    playTick = currFrame*MTransp->getInvertedFPT();

    queueEvent();
}

long Pattern::getFrame()
{
    return currFrame;
}

long Pattern::getGlobalFrame()
{
    return getStartFrame() + getFrame();
}

void Pattern::setGlobalFrame(long frame)
{
    setFrame(frame - getStartFrame());
}

long Pattern::getGlobalStartFrame()
{
    return getStartFrame();
}

long Pattern::getGlobalEndFrame()
{
    return getStartFrame() + getFrameLength();
}

void Pattern::getSmallestCountDown(long* count)
{
    long newcount;

    if(frameCount != 0)
    {
        newcount = frameCount;
    }
    else
    {
        newcount = requeueEvent(false);
    }

    if(newcount > 0 && newcount < *count)
    {
        *count = newcount;
    }
}

// Ticks dcount number of frames for this pattern. Calling function should give frames number
// less or equal than frames in currently queued event. When countdown becomes zero, 
// process currently queued event, requeue event and process internal envelopes. 
// If the player is looped, then also check for wrapping.
//
// [IN]
//          nc - start frame in buffer
//          frames - number of frames to tick
//          fpb - frames per buffer
//
// RET:     N/A
//
void Pattern::tickFrame(long offset, long frames, long totalFrames)
{
    if(offset == 0 || frameCount == 0)
    {
        if(frameCount == 0 && pendingEvent != events.end())
        {
            // Process currently queued event's triggers and requeue event

           (*pendingEvent)->activateTriggers();

            requeueEvent(true);
        }


        // Need to figure out the stuff below

        long num_frames;

        if(frameCount > (totalFrames - offset) || frameCount == -1)
        {
            num_frames = totalFrames - offset;
        }
        else
        {
            num_frames = (long)frameCount;
        }
    
        if(num_frames > 0)
        {
            processEnvelopes(offset, num_frames, currFrame);
        }
    }

    if(frameCount > 0)
    {
        frameCount -= frames;
    }

    currFrame += frames;

    playTick = currFrame*MTransp->getInvertedFPT();

    // Wrap if needed

    if(endFrame > 0 && currFrame >= endFrame)
    {
        if(isLooped())
        {
            setFrame(startFrame);
        }
        else
        {
            // Deactivate pattern playback
        }
    }
}

void Pattern::processEnvelopes(long buffframe, long num_frames, long curr_frame)
{
    for(Trigger* tg : MAudio->activeCommands)
    {
        if(!tg->el->isDeleted() && (tg->ev->evpatt == this || tg->tgPatt == this))
        {
            if(curr_frame < tg->ev->ev_frame)
            {
                tg->stop();

                continue;
            }
            else 
            {
                /*
                env = (Envelope*)tg->el;
                tg->SetFrame(curr_frame - tg->ev->ev_frame);
                if(tg->GetFrame() > env->frame_length)
                {
                    tg->Stop();
                    continue;
                }
                else
                {
                    env->ProcessBuffer1(curr_frame, buffframe, num_frames, 1, tg->ev->ev_frame, tg, NULL);
                }
                */
            }
        }
    }
}


void Pattern::setBounds(long start, long end)
{
    startFrame = start;
    endFrame = end;
}

//
// Initialise queued event and frame countdown according to current frame
// 
void Pattern::queueEvent()
{
    if(events.size() > 0)
    {
        pendingEvent = events.begin();

        while(pendingEvent != events.end() && currFrame > (*pendingEvent)->ev_frame)
        {
            pendingEvent++;
        }

        if(pendingEvent != events.end())
        {
            frameCount = long((*pendingEvent)->ev_frame - currFrame);
        }
        else
        {
            frameCount = endFrame - currFrame;
        }
    }
    else
    {
        pendingEvent = events.end();
    }

    if(playing)
    {
        preInitTriggers(currFrame, true);
    }
}
    
long Pattern::requeueEvent(bool apply)
{
    long countTmp;
    long remainingFrames = endFrame - currFrame;

    auto tmpEv = pendingEvent;

    while(tmpEv != events.end() && currFrame >= (*tmpEv)->ev_frame)
    {
        tmpEv++;
    }

    countTmp = (tmpEv != events.end() ? long((*tmpEv)->ev_frame - currFrame) : remainingFrames);

    if(isLooped() && remainingFrames < countTmp)
    {
        countTmp = remainingFrames;
    }

    if(apply)
    {
        frameCount = countTmp;

        pendingEvent = tmpEv;
    }

    return countTmp;
}

void Pattern::updateEvents()
{
    for(Event* ev : events)
    {
        ev->setTick(ev->tick);
    }
}

void Pattern::adjustBounds()
{
    if(getFrameLength() > 0)
    {
        setBounds(0, getFrameLength());

        if(currFrame < startFrame || currFrame > endFrame)
        {
            setFrame(startFrame);
        }
    }
}

void Pattern::placeTrigger(Trigger* tg)
{
    float tick = tg->starter ? tg->el->getStartTick() : tg->el->getEndTick();

    if(events.size() == 0)
    {
        Event* ev = new Event(tick, this);

        ev->addTrigger(tg);

        events.push_back(ev);
    }
    else
    {
        for(auto itr = events.begin(); itr != events.end(); )
        {
            if(tick < (*itr)->tick)
            {
                Event* n_ev = new Event(tick, this);

                n_ev->addTrigger(tg);

                events.insert(itr, n_ev);

                break;
            }
            else if(tick == (*itr)->tick)
            {
                //jassert(ev->vick == tick);

                (*itr)->addTrigger(tg);

                break;
            }
            else if((*itr) == events.back())
            {
                Event* n_ev = new Event(tick, this);

                n_ev->addTrigger(tg);

                events.push_back(n_ev);

                break;
            }
            else
            {
                itr++;
            }
        }
    }
}

void Pattern::removeTrigger(Trigger* tg)
{
    Event* ev = tg->ev;

    if(ev && ev->removeTrigger(tg) == true) 
    {
        // if last trigger removed, delete event

        if(pendingEvent != events.end() && *pendingEvent == ev)
        {
            pendingEvent++;
        }

        events.remove(ev);

        delete ev;

        tg->ev = NULL;
    }
}

bool Pattern::isPlaying()
{
    if(frameCount != -1)
    {
        return true;
    }

    return false;
}

bool Pattern::isLooped()
{
    return looped;
}

void Pattern::preInitTriggers(long frame, bool activate_env, bool paraminit)
{
    if(paraminit)
    {
        for(Parameter* param : params)
        {
            param->lastsetframe = 0;

            if(param->envtweaked)
            {
                param->resetToInitial();
            }
        }
    }

    for(Event* ev : events)
    {
        for(Trigger* tg : ev->triggers)
        {
            if(!tg->el->isDeleted() && tg->starter )
            {
                if(tg->el->type == El_Envelope)
                {
                    // if envelope
                    {
                        /*
                        env->param->UnblockEnvAffect(); // to allow env. value set
                        env = (Envelope*)tg->el;
                        long envend = long(tg->ev->ev_frame + env->len*_MClock->getFramesPerTick());
                        if(frame >= tg->ev->GetFrame() && frame < envend)
                        {
                            env->param->SetValueFromEnvelope(env->GetValue(frame - tg->ev->GetFrame()), env);
                            if(activate_env)
                            {
                                if(tg->isActive())  tg->Stop();

                                tg->Start(frame - tg->ev->GetFrame());
                            }
                            
                            env->param->lastsetframe = frame;
                        }
                        else if(envend < frame && envend > cmd->param->lastsetframe)
                        {
                            env->param->SetValueFromEnvelope(env->last_value, env);
                            env->param->lastsetframe = envend;
                        }
                        */
                    }
                }
                else if(tg->el->isNote())
                {
                    Note* note = (Note*)tg->el;

                    if(frame > note->getStartFrame() && frame < note->getEndFrame())
                    {
                        if(tg->isActive())  tg->stop();

                        tg->start(frame - tg->ev->getFrame());

                        tg->wt_pos = tg->freq_incr_base*tg->framePhase;

                        tg->env_phase1 = MAudio->getInvertedSampleRate()*tg->framePhase;
                    }
                    else
                    {
                        tg->stop();
                    }
                }
                else if(tg->el->type == El_Pattern)
                {
                    Pattern* pt = (Pattern*)tg->el;

                    if(frame >= pt->getStartFrame() && frame < pt->getEndFrame())
                    {
                        if(tg->isActive())  tg->stop();

                        tg->start(frame - pt->getStartFrame());
                    }
                    else
                    {
                        tg->stop();
                    }
                }
            }
        }
    }
}


