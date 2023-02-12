
#include "36_playhead.h"
#include "36_events_triggers.h"
#include "36_audio_dev.h"
#include "36.h"
#include "36_params.h"
#include "36_env.h"
#include "36_grid.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_transport.h"
#include "36_edit.h"
#include "36_pattern.h"




Playhead::Playhead(Grid* grd)
{
    setTouchable(false);

    grid = grd;
    posX = absX = relX = 0;

    patt = grid->getPattern();
}

void Playhead::timerCallback()
{
    if(GPlaying)
    {
        updatePosFromFrame();

        /*
        handleAutomationRecording(MainClock->GetTickFromFrame(GridPlayer->currFrame_tsync));
        */
    }
}

void Playhead::restartTimer(int interval)
{
    add = int((double)interval/1000*MAudio->getSampleRate());

    startTimer(interval);
}

void Playhead::reset()
{
    stopTimer();

    updatePosFromFrame();
}

void Playhead::updatePosFromFrame()
{
    if (patt == NULL)
    {
        return;
    }

    absX = int(patt->getFrame()/grid->getFramesPerPixel());

    relX = absX - RoundFloat(grid->getHoffs() * grid->getPixelsPerTick());

    posX = grid->getX() + relX;

    MainEdit* me = (MainEdit*)parent;

    setCoords1(posX - 3, 0, 8, me->getH() + 1);
}

void Playhead::updateFrameFromPos()
{
    if (patt == NULL)
    {
        return;
    }

    patt->setFrame((long)(absX*grid->getFramesPerPixel()));
}

bool Playhead::handleAutomationRecording(float currStep)
{
    // Recording envelopes

    bool refreshcontent = false;

    for(Parameter* recparam : recParams)
    {
        if(currStep >= recparam->env->gettick()) // only when playback position is not before the envelope
        {
            float x = currStep - recparam->env->gettick();

            EnvPoint* newpnt;

            if(recparam->getType() == Param_Vol)
            {
                newpnt = recparam->autoenv->addPoint(x, recparam->getValue() *DAW_INVERTED_VOL_RANGE, false);
            }
            else
            {
                newpnt = recparam->autoenv->addPoint(x, (recparam->getValue() - recparam->getOffset())/recparam->getRange(), false);
            }

/*
            if(newpnt->tick < recparam->lastrecpoint->tick && newpnt->next != NULL)
            {
                // This is the case when newly recorded point arrives before the last one. Happens most likely
                // because of position change by user. Avoid any action here.
            }
            else
            {
                // Handle rewriting of already recorded stuff

                EnvPoint* pntnext;
                EnvPoint* pnt = recparam->lastrecpoint->next;

                while(pnt != NULL && pnt != newpnt && pnt->tick < x)
                {
                    pntnext = pnt->next;

                    recparam->autoenv->removePoint(pnt);

                    pnt = pntnext;
                }

                // And increase length if needed

                if(x > recparam->autoenv->tickLength)
                {
                    recparam->autoenv->setTickLength(x);
                }
            }
*/

            recparam->lastrecpoint = newpnt;

            grid->redraw(true);

            refreshcontent = true;
        }
    }

    return refreshcontent;
};

void Playhead::drawSelf(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(dx1, dy1, dwidth, dheight);

    setc(g, 0xffFFB040);

    //g.setColour(Colour(0xffB08050));
    //g.setColour(Colour(0xffC09050));

    //gSetMonoColor(g, .7f);

    int x = x1 + 3;

    gLineHorizontal(g, y1 + 3, x - 3, x + 4);
    gLineHorizontal(g, y1 + 4, x - 2, x + 3);
    gLineHorizontal(g, y1 + 5, x - 1, x + 2);
    gLineHorizontal(g, y1 + 2, x, x + 1);

    fillx(g, 0, 0, 7, 4);
    fillx(g, 1, 1, 6, 4);

    gLineHorizontal(g, y1 + 4, x - 1, x + 2);
    gLineVertical(g, x, y1 + 4, y1 + 6);

    setc(g, 0xffFFB040, 1, 0.6f);

    gLineVertical(g, x, y1 + 6, y2);

    g.restoreState();
}



