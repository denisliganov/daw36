
#include "36_lane.h"
#include "36_grid.h"
#include "36_draw.h"
#include "36.h"
#include "36_params.h"
#include "36_history.h"
#include "36_playhead.h"
#include "36_element.h"
#include "36_note.h"
#include "36_utils.h"
#include "36_pattern.h"
#include "36_transport.h"
#include "36_device.h"
#include "36_keyboard.h"
#include "36_instr.h"
#include "36_edit.h"
#include "36_auxkeys.h"





LanePanel::LanePanel(Grid* grid)
{
    workGrid = grid;

    //grid->setLane(this);

    laneType = Lane_Vol;

    addObject(lane = new Lane(workGrid, Lane_Vol));

    addObject(keys = new AuxKeys(workGrid, 30, false));
}

void LanePanel::drawself(Graphics& g)
{
    fill(g, 0.32f);

    //gFillRect(g, x2 - LeftGap + 1, y1 + 2, x2, y2);

    //gSetMonoColor(g, 0.1f);
    //gLineVertical(g, x1 + LeftGap - 1, y1 + 2, y2);

    //gLineHorizontal(g, y2 - BottomPadHeight + 1, x1, x2);
}

void LanePanel::remap()
{
    if(laneType == Lane_Vol || laneType == Lane_Pan)
    {
        lane->setCoords2(LeftGap, 2, width - GridScrollWidth, height - BottomPadHeight);
        keys->setEnable(false);
    }
    else if ( laneType == Lane_Keys )
    {
        keys->setCoords2(LeftGap, 2, width - GridScrollWidth, height - BottomPadHeight);
        lane->setEnable(false);
    }
}

void LanePanel::setType(LaneType ltype)
{
    laneType = ltype; 

    lane->type = ltype;

    redraw();
}



Lane::Lane(Grid* g, LaneType lt)
{
    grid = g;
    type = lt;

    prevX = -1;
    prevY = -1;
}

void Lane::drawBar(Graphics& g, Element* el, int cx, bool sel, int divHgt)
{
    float val;
    int barHeight;

    Note* note = (Note*)el;

    if (type == Lane_Vol)
    {
        val = note->vol->getValue();

        barHeight = RoundFloat((float(height-1)/DAW_VOL_RANGE) * val);
    }
    else
    {
        val = 1.f - ((note->pan->getValue() + 1.f)/2.f);

        barHeight = RoundFloat((float(height - 1)) * val);
    }

    if (sel == 1)
    {
        g.setColour(Colour(0xffFFA030));    // selected
    }
    else
    {
        Instrument* instr = (Instrument*)el->dev;

        instr->setMyColor(g, 1, 1, .3f);
    }

    int barY1;
    int barY2;
    int yCap;

    if (type == Lane_Vol)
    {
        barY1 = y2 - barHeight;
        barY2 = y2;

        yCap = barY1;
    }
    else // pan
    {
        if (divHgt >= barHeight)
        {
            barY1 = y2 - divHgt;
            barY2 = y2 - barHeight;
            yCap = barY2;
        }
        else
        {
            barY1 = y2 - barHeight;
            barY2 = y2 - divHgt;
            yCap = barY1;
        }
    }

    gLineVertical(g, cx, barY1, barY2 + 1);

    // draw cap
    //g.setColour(Colour(0xffFFFFFF));

    gLineHorizontal(g, yCap, cx, cx + 3);
}


void Lane::drawBars(Graphics& g)
{
    int divHgt;

    if (type == Lane_Vol)
        divHgt = (int)((float)(height) / DAW_VOL_RANGE);
    else
        divHgt = (int)((float)height / 2);

    g.drawHorizontalLine(y2 - divHgt, float(x1), float(x2));

    // First draw non-selected elements, then selected ones over them

    for(Element* el : grid->getpatt()->ptBase->elems)
    {
        if (!el->isdel() && el->isNote())
        {
            drawBar(g, el, grid->getXfromTick(el->gettick()), false, divHgt);
        }
    }

    for (Element* el : grid->selected)
    {
        if (!el->isdel() && el->patt == grid->getpatt() && el->isNote())
        {
            drawBar(g, el, grid->getXfromTick(el->gettick()), true, divHgt);
        }
    }

    //grid->drawIntermittentHighlight(g, x1, y1, height);
}

void Lane::drawself(Graphics& g)
{
    // Draw backdrop with vertical bars

    fill(g, 0.25f);

    // Horizontal mark
    gSetMonoColor(g, 0.15f);

    // Vertical lines

    float xOffset;
    int xTick;

    xOffset = (grid->getHoffs() - (int)grid->getHoffs()) * grid->getppt();
    xTick = (int)grid->getHoffs();

    float xtt = 0;
    int xc;

    while (1)
    {
        if (xTick % (MTransp->getTicksPerBar()) == 0)
        {
            xc = RoundFloat(xtt - xOffset + x1);

            if (xc > x2 - 14)
            {
                break;
            }

            if (xc >= x1 && xc <= x2)
            {
                gSetMonoColor(g, 0.15f);

                gLineVertical(g, xc, y1, y2 + 1);
            }
        }
        else if (xTick % MTransp->getTicksPerBeat() == 0)
        {
            xc = RoundFloat(xtt - xOffset + x1);

            if (xc > x2)
            {
                break;
            }
            

            if (xc >= x1 && xc <= x2)
            {
                gSetMonoColor(g, 0.2f);

                gLineVertical(g, xc, (y1), y2 + 1);
            }
        }

        xTick++;

        xtt += grid->getppt();
    }

    g.saveState();
    g.reduceClipRegion(x1, y1, width, height);

    drawBars(g);

    g.restoreState();
}

void Lane::process(bool leftbt, int mx, int my, unsigned flags)
{
    int         vh;
    float       newVal;
    bool        doChange;
    bool        setDefault = (flags & kbd_alt);

    int ay = my - y1;

    if ((prevX == -1) && (prevY == -1))
    {
        prevX = mx;
        prevY = ay;
    }

    vh = y2 - y1;

    bool processSelectedOnly = false;

    if (grid->selected.size() > 0)
    {
        processSelectedOnly = true;
    }

    for(Element* el : grid->getpatt()->ptBase->elems)
    {
        int cx = grid->getXfromTick(el->gettick());

        if (!el->isdel() && el->isNote() && (!processSelectedOnly || el->issel()) && (cx >= x1 && cx <= x2))
        {
            doChange = false;

            if ((leftbt || prevX == mx) && abs(mx - cx) <= 3)
            {
                newVal = (float)(vh - ay) / (float)vh;

                if (newVal > 1)
                {
                    newVal = 1;
                }

                if (newVal <= 0)
                {
                    newVal = 0;
                }

                doChange = true;
            }
            else if ((cx >= prevX && cx <= mx) || (cx >= mx && cx <= prevX))
            {
                newVal = (1.0f - (Interpolate_Line((float)prevX, (float)prevY, (float)mx, (float)ay, (float)cx) / (float)vh));

                if (newVal > 1.0f)
                {
                    newVal = 1.0f;
                }

                if (newVal < 0)
                {
                    newVal = 0;
                }

                doChange = true;
            }

            if (doChange)
            {
                if (type == Lane_Vol)
                {
                    Note* elNote = (Note*)el;

                    Parameter* locvol = elNote->vol;

                    if (setDefault)
                    {
                        newVal = locvol->getDefaultVal();
                    }

                    //locvol->SetNormalValue(newVal*locvol->getRange() + locvol->getOffset());

                    MHistory->addNewAction(HistAction_ParamChange, (void*)locvol, locvol->getValue(), newVal*locvol->getRange() + locvol->getOffset(), 0, 0);
                }
                else if (type == Lane_Pan)
                {
                    Note* elNote = (Note*)el;

                    Parameter* locpan = elNote->pan;

                    if (setDefault)
                    {
                        newVal = locpan->getDefaultVal();
                    }

                    //locpan->SetNormalValue(1 - newVal*2);

                    MHistory->addNewAction(HistAction_ParamChange, (void*)locpan, locpan->getValue(), newVal*locpan->getRange() + locpan->getOffset(), 0, 0);
                }
            }
        }
    }

    if (leftbt)
    {
        prevX = mx;
        prevY = ay;
    }

    redraw();
}

void Lane::handleMouseDrag(InputEvent& ev)
{
    process(ev.leftClick, ev.mouseX, ev.mouseY, ev.keyFlags);
}

void Lane::handleMouseDown(InputEvent& ev)
{
    process(ev.leftClick, ev.mouseX, ev.mouseY, ev.keyFlags);
}

void Lane::handleMouseUp(InputEvent& ev)
{
    prevX = prevY = -1;
}



