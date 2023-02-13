

#include "36_timeline.h"
#include "36_draw.h"
#include "36_transport.h"
#include "36_grid.h"
#include "36_pattern.h"
#include "36_events_triggers.h"
#include "36_utils.h"
#include "36_audio_dev.h"
#include "36_text.h"
#include "36_edit.h"
#include "36_playhead.h"
#include "36_macros.h"




Timeline::Timeline(Grid* grd)
{
    grid = grd;
}

void Timeline::handleMouseDown(InputEvent& ev)
{
    if(ev.leftClick)
    {
        set(ev.mouseX);
    }
}

void Timeline::handleMouseDrag(InputEvent& ev)
{
    set(ev.mouseX);
}

void Timeline::set(int mouse_x)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    int newX = mouse_x + 1 - getX1();

    LIMIT(newX, 0, getW());

    float currPosPix = (float(newX) + (float)(grid->getHoffs()*grid->getPixelsPerTick()));

    grid->getPattern()->setFrame((long)(currPosPix*grid->getFramesPerPixel()));

    MTransp->updTimeFromFrame();

    MEdit->playHead->updatePosFromFrame();

    ReleaseMutex(AudioMutex);
}

void Timeline::handleMouseMove(InputEvent & ev)
{
///
}

void Timeline::handleMouseWheel(InputEvent & ev)
{
    float ofsDelta = ev.wheelDelta*((float)grid->getW()/grid->getPixelsPerTick()*0.03f);

    MGrid->setHoffs(MGrid->getHoffs() - ofsDelta);
}


void Timeline::drawIntermittentHighlight(Graphics& g, int x, int y, int w, int h, int numBars)
{
    int tickLen = numBars*MTransp->getTicksPerBar();
    int pixLen = int(grid->getPixelsPerTick() * tickLen);

    int xoffs = RoundFloat((grid->getHoffs() / tickLen - (int)grid->getHoffs() / tickLen) * grid->getPixelsPerTick() * tickLen);

    int num = int(grid->getHoffs() / tickLen);
    int flag = num % 2;
    int xCoordinate = -xoffs + pixLen * flag + x;

    gSetMonoColor(g, 0.0f);
    //gSetMonoColor(g, 0, 1);

    int end = x + w - 1;

    xCoordinate += pixLen;

    while (xCoordinate < end)
    {
        //g.fillRect(xCoordinate, y, pixLen, h);

        gLineVertical(g, xCoordinate, y, y + h - 1);
        gLineVertical(g, xCoordinate + pixLen, y, y + h - 1);

        xCoordinate += pixLen * 2;
    }
}

void Timeline::drawSelf(Graphics & g)
{
    fill(g, .25f);

    setc(g, .2f);

    lineH(g, 0, 0, width - 1);
    lineH(g, height - 1, 0, width - 1);

    int barLenTicks = MTransp->getTicksPerBar();
    float barLenPixels = grid->getPixelsPerTick()*barLenTicks;

    int step = 1;

    while(barLenPixels*step < 32)
    {
        step++;   // 40 pixels min. spacing
    }

    int currentBar = int(grid->getHoffs()/barLenTicks);

    int stepcnt = (step - currentBar%step)%step;

    int tpBar = MTransp->getTicksPerBar();
    int tpBeat = MTransp->getTicksPerBeat();

    //drawIntermittentHighlight(g, x1, y1, width, height, 4);

    gSetMonoColor(g, .8f);

    float xCoord = x1 - ((grid->getHoffs() / barLenTicks - int(grid->getHoffs()) / barLenTicks) * barLenPixels);

    while(xCoord <= x2)
    {
        if(stepcnt == 0)
        {
            int sec = int(currentBar * barLenTicks * MTransp->getSecondsPerTick());
            int min = sec/60;

            sec = sec%60;

            if(currentBar >= 0)
            {
                //String str;
                //if(min == 0)
                //    str = String::formatted(T("%d"), sec);
                //else
                //    str = String::formatted(T("%d:%d"), min, sec);
                //int w = gGetTextWidth(FontSmall, (const char*)str);
                //gText(g, FontSmall, (std::string)str, RoundFloat(xCoord) - w/2, y1 + 12);

                String str = String::formatted(T("%d"), currentBar);
                int w = gGetTextWidth(FontSmall, (const char*)str);
                gText(g, FontSmall, (std::string)str, RoundFloat(xCoord) - w/2, y1 + 17);
            }

            stepcnt = step;
        }

        if(currentBar >= 0)
        {
            for(int tc = 0; tc < tpBar; tc++)
            {
                int h = tc == 0 ? 2 : tc % tpBeat == 0 ? 2 : 0;

                if(h > 0)
                {
                   // lineV(g, RoundFloat(xCoord - x1 + grid->getPixelsPerTick()*tc), height - 1 - h, height - 1);
                }
            }
        }

        stepcnt--;

        currentBar++;

        xCoord += barLenPixels;
    }

/*
    // small gradient

    float alpha = 0.1f;

    for(int c = 0; c < height - 1; c++)
    {
        //g.setColour(Colour(color));

        gSetMonoColor(g, 1.f, alpha);

        gLineHorizontal(g, y1 + c, (float)x1, (float)x2);

        alpha -= 0.003f;

        if (alpha <= 0)
        {
            break;
        }
    }
*/
}


