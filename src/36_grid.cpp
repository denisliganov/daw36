

#include "36_globals.h"
#include "36_grid.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_keyboard.h"
#include "36_scroller.h"
#include "36_edit.h"
#include "36_ctrlpanel.h"
#include "36_instrpanel.h"
#include "36_vstinstr.h"
#include "36_params.h"
#include "36_history.h"
#include "36_instr.h"
#include "36_lane.h"
#include "36_audio_dev.h"
#include "36_draw.h"
#include "36_playhead.h"
#include "36.h"
#include "36_system.h"
#include "36_button.h"
#include "36_transport.h"
#include "36_timeline.h"
#include "36_snapmenu.h"
#include "36_dragndrop.h"
#include "36_macros.h"
#include "36_events_triggers.h"



class Selection : public Gobj
{
friend  Grid;

protected:

        Grid*       grid;

        bool        isMouseTouching(int mx, int my)  { return false; }

        void        drawself(Graphics& g)
        {
            g.saveState();
            //g.reduceClipRegion(x1, y1, width, height);

            fill(g, 1.f, .1f);

            rect(g, 1.f, .3f);

            g.restoreState();
        }

public:

        Selection(Grid* grd)
        {
            grid = grd;
        }
};

class InstrHighlight : public Gobj
{
friend  Grid;

        Grid*       grid;

        bool        isMouseTouching(int mx, int my)  { return false; }

        void drawself(Graphics& g)
        {
            g.saveState();
            g.reduceClipRegion(grid->getX1(),grid->getY1(), grid->getW(), grid->getH());

            //gSetMonoColor(g, 1, 0.7f);
            //gSetColor2(g, 0xffFFC030, 1, .5f);

            fill(g, 1.f, .09f);

            g.restoreState();
        }

public:

        InstrHighlight(Grid* grd) {grid = grd;}
};


class PlaceHighlight : public Gobj
{
friend  Grid;

        float               tick;
        int                 line;
        Grid*               grid;
        InstrHighlight*     instrHighlight;
        Note*               currNote;

        bool    isMouseTouching(int mx, int my)  { return false; }

public:

        PlaceHighlight(Grid* grd)
        {
            grid = grd;

            currNote = NULL;

            tick = 0;
            line = 0;

            addHighlight(instrHighlight = new InstrHighlight(grid));

            instrHighlight->setRelative(false);
        }

        void setPos(float newTick, int newLine)
        {
            tick = newTick;
            line = newLine;

            if (grid->isMouseHovering())
            {
                int ly = grid->getYfromLine(line);
                int lh = grid->getLineHeight();

                instrHighlight->setCoords1(grid->getX1() - 7, ly - lh + 2, 2, lh - 2);
                //instrHighlight->setCoords1(grid->getX1(), ly - lh + 2, grid->getW(), InstrHeight);

                if (grid->getDisplayMode() == GridDisplayMode_Bars)
                {
                    currNote = grid->activeNote;

                    if (currNote && !currNote->isDeleted())
                    {
                        if (!currNote->isSelected())
                        {
                            int x = currNote->getX1() - grid->getX1();
                            int y = currNote->getY1() - grid->getY1();

                            setCoords1(x, y, currNote->getW(), currNote->getH());

                            setVisible(true);
                        }
                    }
                    else if (grid->mode == GridMode_Default)
                    {
                        int x = grid->getXfromTick(tick) - grid->getX1();
                        int y = grid->getYfromLine(line) - int(grid->getLineHeight()) - grid->getY1() + 1;

                        setCoords2(x, y, x + grid->snap*grid->getPixelsPerTick() - 1, y + int(grid->getLineHeight()) - 1);

                        setVisible(true);
                    }
                }
            }
        }

        void update()
        {
            setVisible(false);

            if((grid->isMouseHovering()) && grid->getDisplayMode() == GridDisplayMode_Bars && grid->mode != GridMode_Selecting)
            {
                if (grid->alignLine >= 0)
                {
                    setPos(grid->alignTick, grid->alignLine);
                }
            }
        }

        void drawself(Graphics& g)
        {
            if (currNote && !currNote->isDeleted())
            {
                if (!currNote->isSelected())
                {
                    //gSetMonoColor(g, 1, .9f);
                    //gLineVertical(g, x1, y1, y2);
                    //gLineHorizontal(g, y2, x1, x2);

                    fill(g, 1.f, .2f);
                    //gTriangle(g, x1, y1 + 1, x2, y2, x1, y2);

                    //gDrawTriangle(g, x1, y1 + 1, x2, y2, x1, y2);
                    rect(g, .9f);

                    //gSetMonoColor(g, 1, .1f);
                    //gTriangle(g, x1, y2 - height, x2, y2, x1, y2);
                }
            }
            else
            {
            //    gSetMonoColor(g, 1, .7f);
            //    gFillRect(g, x1, y1, x1, y2);
            }
        }
};


Grid::Grid(float step_width, int line_height, Pattern* pt, Timeline* tl)
{
    snap = 0;
    pixpertick = step_width;
    lheight = line_height; 
    timeline = tl;
    timeline->grid = this;
    patt = pt;

    activeElem = NULL;
    activeNote = previewNote = NULL;

    mainimg = NULL;
    brushImage = NULL;
    elemImage = NULL;

    brushWidthTicks = 0;
    alignTick = 0;
    alignLine = 0;

    lastElementEndTick = 0;
    fullTickSpan = visibleTickSpan = 0;
    vscr = hscr = NULL;
    mouseIsDown = false;
    stepDefault = true;
    wasSelecting = false;

    displayMode = GridDisplayMode_Bars;

    addHighlight(sel = new Selection(this));
    addHighlight(place = new PlaceHighlight(this));

    selreset();

    updbounds();

    setmode(GridMode_Default);

    lastAction = GridAction_Reset;
}

void Grid::grabTextCursor(float tick, int line)
{
    addHighlight(MTextCursor);

    MTextCursor->setPos(tick, line);

    updbounds();
}

void Grid::drawIntermittentHighlight(Graphics& g, int x, int y, int w, int h, int numBars)
{
    int tickLen = numBars*MTransp->getTicksPerBar();
    int pixLen = int(getPixelsPerTick() * tickLen);
    int xoffs = RoundFloat((getTickOffset() / tickLen - (int)getTickOffset() / tickLen) * getPixelsPerTick() * tickLen);
    int num = int(getTickOffset() / tickLen);
    int flag = num % 2;
    int xCoordinate = -xoffs + pixLen * flag + x;

    xCoordinate += pixLen;

    int end = x + w - 1;

    gSetMonoColor(g, 1, .05f);

    while (xCoordinate < end)
    {
        g.fillRect(xCoordinate, y, pixLen, h);

        gLineVertical(g, xCoordinate, y, y + h - 1);
        gLineVertical(g, xCoordinate + pixLen, y, y + h - 1);

        xCoordinate += pixLen * 2;
    }
}

void Grid::refreshImageBrush()
{
    int imgHeight = lheight; //*20;
    int barWidth = RoundFloat(MTransp->getTicksPerBar()*getPixelsPerTick());
    int imgWidth = barWidth;

/*
    while(imgWidth < 400)
    {
        imgWidth += barWidth;
    }*/

    if(brushImage != NULL)
    {
        delete brushImage;
    }

    brushImage = new Image(Image::RGB, imgWidth, imgHeight, true);

    Graphics imageContext(*(brushImage));

    gSetMonoColor(imageContext, .1f, 1);

    imageContext.fillAll();

    float tick = .25f;
    float beat = .35f;
    float bar = .42f;

    float divClr = .05f;

    // ticks
    //gSetMonoColor(imageContext, .5f);
    gSetMonoColor(imageContext, tick);

    if(pixpertick > 1.5f)
    {
        for (int x = 0; x < imgWidth; x += pixpertick)
        {
            //gFillRectWH(imageContext, x, 0, 1, 5);
            gFillRectWH(imageContext, x, 0, 1, lheight);
        }
    }
    else
    {
        //gFillRectWH(imageContext, 0, 0, imgWidth, 5);
        gFillRectWH(imageContext, 0, 0, imgWidth, lheight);
    }

    // beats
    gSetMonoColor(imageContext, beat);

    int beatStep = MTransp->getTicksPerBeat()*getPixelsPerTick();
    for (int x = 0; x < imgWidth; x += beatStep)
    {
        gFillRectWH(imageContext, x, 0, 1, lheight);
    }

    // bar line
    gSetMonoColor(imageContext, bar);

    gLineVertical(imageContext, 0, 0, lheight);

    // horiz bottom line
    gSetMonoColor(imageContext, divClr);

    gLineHorizontal(imageContext, getLineHeight() - 1, 0, imgWidth);

    if (displayMode != GridDisplayMode_Pans)
    {
     //   gSetMonoColor(imageContext, .12f);
     //   gLineHorizontal(imageContext, float(getLineHeight() - 1)*(1 - DAW_INVERTED_VOL_RANGE), 0, imgWidth);
    }
    else
    {
        gSetMonoColor(imageContext, .12f);
        gLineHorizontal(imageContext, getLineHeight()*.5f - 1, 0, imgWidth);
    }
}

void Grid::refreshImageBuffer()
{
    if (brushImage != NULL && width > 0 && height > 0)
    {
        if (mainimg != NULL) 
        {
            delete mainimg;
        }

        mainimg = new juce::Image(Image::RGB, width, height, true);

        Graphics image(*(mainimg));

        int tickPerBar = MTransp->getTicksPerBar();

        int xoffs = RoundFloat((getTickOffset() / tickPerBar - (int)getTickOffset() / tickPerBar) * getPixelsPerTick() * tickPerBar);

        int yoffs = int(vscr->getoffs()) % lheight;

        ImageBrush* imgBrush = new ImageBrush(brushImage, -xoffs, -yoffs, 1);

        image.setBrush(imgBrush);

        image.fillRect(0, 0, width, height);

        //drawIntermittentHighlight(image, 0, 0, width, height, 4);

        delete imgBrush;
    }
}

void Grid::refreshElementsImage()
{
    if (width > 0 && height > 0)
    {
        if (elemImage != NULL)
        {
            delete elemImage;
        }

        elemImage = new juce::Image(Image::RGB, width, height, true);

        Graphics image(*(elemImage));

        drawelems(image);
    }
}

void Grid::removeElementFromLists(Element * el)
{
    visible.remove(el);
    selected.remove(el);
    clipboard.remove(el);
}

void Grid::setDisplayMode(GridDisplayMode display_mode)
{
    displayMode = display_mode;

    redraw(true, true);
}

void Grid::setmode(GridActionMode md)
{
    mode = md;
}

void Grid::remap()
{
    refreshImageBuffer();

    updbounds();

    mapelems();

    place->update();
}

void Grid::mapelems()
{
    visible.clear();

    std::list<Element*>::iterator it = visible.end();

    float lastStartTick = -1;
    float lastVisibleTick = hscr != NULL? hscr->getoffs() + hscr->getvisible() : 0;

    if(MInstrPanel)
    {
        /*
        for(Instrument* i : MInstrPanel->instrs)
        {
            if(i->isShown())
            {
                visible.splice(visible.end(), i->getNotesFromRange(tickOffset, lastVisibleTick));
            }

            for(Element* el : visible)
            {
                el->calcCoordsForGrid(this);
            }
        }
        */

        for(Element* el : patt->elems)
        {
            if ( el->getEndTick() < hscr->getoffs() || el->getStartTick() > lastVisibleTick)
            {
                // Skip out-of-visible-area elements
            }
            else
            {
                if (el->isShown())
                {
                    if(visible.size() > 0)
                    {
                        // Place to the right position, so the list is sorted according to start tick
    
                        if(el->getStartTick() >= (*it)->getStartTick())
                        {
                            while(it != visible.end() && el->getStartTick() >= (*it)->getStartTick())
                            {
                                it++;
                            }
                        }
                        else
                        {
                            while(it != visible.begin())
                            {
                                --it;
    
                                if(el->getStartTick() >= (*it)->getStartTick())
                                {
                                    it++;
    
                                    break;
                                }
                            }
                        }
                    }

                    visible.insert(it, el);

                    it--;

                    el->calcCoordsForGrid(this);
                }
            }
        }
    }

    if (MTextCursor && MTextCursor->getParent() == this)
    {
        MTextCursor->updPos();
    }
}

void Grid::drawelems(Graphics& g)
{
    if(visible.size() > 0)
    {
        std::list<Element*>::iterator it = visible.begin();

        while(it != visible.end())
        {
            Element* el = *it;

            el->drawOnGrid(g, this);

            it++;
        }
    }

/*
    if(visible.size() > 0)
    {
        std::list<Element*>::iterator it = visible.end();

        it--;

        while(1)
        {
            Element* el = *it;

            el->drawOnGrid(g, this);

            if(el->isNote())
            {
                g.excludeClipRegion(el->getDrawX1(), el->getDrawY1(), el->getDrawWidth(), el->getDrawHeight());
            }

            if(it == visible.begin())
            {
                break;
            }

            it--;
        }
    }
*/
}

void Grid::drawself(Graphics& g)
{
    if(mainimg != NULL)
    {
        g.drawImageAt(mainimg, x1, y1);

        //gFillRect(g, x1, y1, x2, y2);
        //g.drawImage(mainimg, x1 + 40, y1 + 40, width - 80, height - 80, 40, 40, width - 80, height - 80);
    }

    if(lastline > 0 && (vscr->getoffs() + height)/lheight > lastline)
    {
        setc(g, 0.15f, .4f);
        int y = (lastline+1)*lheight - vscr->getoffs();
        fillx(g, 0, y, width, height - y);
    }

    //if(elemImage != NULL)
    //    g.drawImageAt(elemImage, x1, y1);

    drawelems(g);

// Testing text output:
    setc(g, 0xdfFFFFFF);
    gDString(g, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 297, 8, 8);
    gZxString(g, "0123456ABCDEFGHIJKLMNOPQRST", 550, 320);
}

void Grid::redraw(bool remap_objects, bool refresh_image)
{
    if(refresh_image)
    {
        refreshImageBrush();
        refreshImageBuffer();

        timeline->redraw();
    }

    if(remap_objects)
    {
        remap();

        refreshElementsImage();
    }

    Gobj::redraw();
}

void Grid::sethoffs(float offs, bool from_nav_bar)
{
    hscr->setoffs(offs);

    //updateScrollers();

    //MEdit->playHead->updatePosFromFrame();
}

void Grid::updbounds()
{
    lastElementEndTick = 0;
    lastElementStartTick = 0;
    lastline = 0;

    if (patt != NULL)
    {
        for(Element* el : patt->ptBase->elems)
        {
            if (!el->isDeleted())
            {
                if(el->getEndTick() > lastElementEndTick)
                {
                    lastElementEndTick = el->getEndTick();
                }

                if(el->getStartTick() > lastElementStartTick)
                {
                    lastElementStartTick = el->getStartTick();
                }

                if(el->getLine() > lastline)
                {
                    lastline = el->getLine();
                }
            }
        }
    }

    visibleTickSpan = (float)(width)/getPixelsPerTick();

    fullTickSpan = lastElementEndTick + (visibleTickSpan*0.9f);

    if (MTextCursor && MTextCursor->grid == this && MTextCursor->getLine() > lastline)
    {
        lastline = MTextCursor->getLine();
    }

    if(hscr)
    {
        hscr->updlimits(fullTickSpan, visibleTickSpan, getTickOffset());
    }

    if(vscr)
    {
        vscr->updlimits((1 + lastline + 5)*lheight, height, vscr->getoffs());
    }
}

void Grid::updscale()
{
    framesPerPixel = MTransp != NULL ? MTransp->getFramesPerTick()/pixpertick : 0;

    MInstrPanel->updateWaves();

    redraw(true, true);
}

void Grid::setppt(float ppt, int mouseRefX)
{
    pixpertick = ppt;

    updscale();

    MEdit->playHead->updatePosFromFrame();

    float offs = hscr->getoffs();

    if(mouseRefX >= 0)
    {
        offs = currTick - float(mouseRefX - getX1())/(float)pixpertick;
    }
    else
    {
        // offs = patt->getPlayTick() - visibleTickSpan/2;
    }

    float last = 0;

    if (displayMode == GridDisplayMode_Bars)
    {
        last = lastElementEndTick - visibleTickSpan*0.2f;
    }
    else
    {
        last = lastElementStartTick - visibleTickSpan*0.2f;
    }

    if (last < 0)
    {
        last = 0;
    }

    if(offs < 0)
    {
        offs = 0;
    }
    else if(offs > last)
    {
        offs = last;
    }

    sethoffs(offs);
}

void Grid::handleTransportUpdate()
{
    updscale();

    for(Element* el : patt->ptBase->elems)
    {
        if (!el->isDeleted())
        {
            el->recalculate();

            el->calcCoordsForGrid(this);
        }
    }

    patt->updateEvents();

    redraw(true, true);
}

void Grid::changeScale(int delta, int mouseRefX)
{
    float midTick = getTickFromX(getX1() + getW()/2);

    float val = pixpertick;

    float ticksWidth = float(getW())/(float)pixpertick;

    if (delta > 0)
    {
        val += (val < 1 ? 0.25f : val < 2 ? 0.5f : val < 6 ? 1 : val < 12 ? 2 : val < 32 ? 4 : val < 64 ? 8 : 16);

        if(val > 128)
        {
            val = 128;
        }
    }
    else if (delta < 0)
    {
        val -= (val > 64 ? 16 : val > 32 ? 8 : val > 12 ? 4 : val > 6 ? 2 : val > 2 ? 1 : val > 1 ? 0.5f : 0.25f);

        if(val < 0.5f)
        {
            val = 0.5f;
        }
    }

    setppt(val, mouseRefX);
}

void Grid::setLineHeight(int newLH)
{
    if(newLH != lheight)
    {
        lheight = newLH;
    }

    redraw(true, true);
}

float Grid::getPixelsPerTick()
{
    return pixpertick;
}

float Grid::getTickOffset()
{
    return hscr->getoffs();
}

int Grid::getLineHeight()
{
    return lheight;
}

Pattern* Grid::getPattern()
{
    return patt;
}

int Grid::getXfromTick(float tick)
{
    return RoundFloat((tick - hscr->getoffs())*pixpertick) + getX1();
}

float Grid::getTickFromX(int x)
{
    return ((float)x - getX1())/pixpertick + hscr->getoffs();
}

int Grid::getYfromLine(int line)
{
    return getY1() - vscr->getoffs() + line*lheight + lheight - 1;
}

int Grid::getLineFromY(int y)
{
    return (y - getY1() + vscr->getoffs())/lheight;
}

void Grid::checkpos(InputEvent & ev)
{
    Element* newEl = NULL;

    bool checkByStep = false; //(mode == GridMode_Ctrl ? true : false);

    if(visible.size() > 0)
    {
        std::list<Element*>::iterator it = visible.end();

        it--;

        while(1)
        {
            Element* el = *it;

            if(checkByStep)
            {
                if (el->getLine() == alignLine)
                {
                    if (el->isSelected() && el->isPointed(ev.mouseX, ev.mouseY, this))
                    {
                        newEl = el;
                        break;
                    }
                    else if (el->getStartTick() == alignTick)
                    {
                        newEl = el;
                        break;
                    }
               }
            }
            else
            {
                if (el->getLine() == alignLine)
                {
                    if (mode == GridMode_Alt)
                    {
                        if (abs(ev.mouseX - el->getX2()) < 10)
                        {
                            setmode(GridMode_ElemResizing);

                            newEl = el;

                            resizeEdge = Resize_Right;

                            prevTick = alignTick;

                            break;
                        }
                    }
                    else if (el->isPointed(ev.mouseX, ev.mouseY, this))
                    {
                        newEl = el;

                        break;
                    }
                }
            }

            if(it == visible.begin())
            {
                break;
            }

            it--;
        }
    }

    if (newEl != activeElem)
    {
        setactivelem(newEl);
    }
}

void Grid::updcursor(InputEvent & ev)
{
    if (mode == GridMode_ElemResizing)
    {
        if (resizeEdge == Resize_Left || resizeEdge == Resize_Right)
        {
            window->setCursor(Cursor_LeftRight);
        }
        else if (resizeEdge == Resize_Top || resizeEdge == Resize_Bottom)
        {
            window->setCursor(Cursor_UpDown);
        }
        else
        {
            window->setCursor(Cursor_Arrow);
        }
    }
    else if (mode == GridMode_Shift)
    {
        window->setCursor(Cursor_Clone);
    }
    else if (mode == GridMode_Brushing)
    {
        window->setCursor(Cursor_Brush);
    }
    else if (mode == GridMode_Ctrl)
    {
        //window->setCursor(Cursor_Slide);
    }
    else
    {
        window->setCursor(Cursor_Arrow);
    }
}

void Grid::handleMouseLeave(InputEvent & ev)
{
   // if (activeElem != NULL )
    //    activeElem->highlightOff();

    //place->update();
}

void Grid::handleMouseEnter(InputEvent & ev)
{
    //place->update();
}

void Grid::updmode(InputEvent & ev)
{
    if (ev.keyFlags & kbd_alt)
    {
        setmode(GridMode_Alt);
    }
    else if (ev.keyFlags & kbd_shift)
    {
        setmode(GridMode_Shift);
    }
    else if (ev.keyFlags & kbd_ctrl)
    {
        setmode(GridMode_Ctrl);
    }
    else
    {
        //selreset(false);

        setmode(GridMode_Default);
    }
}

void Grid::handleModifierKeys(unsigned flags)
{
    if (mouseIsDown)
    {
        return;
    }

    InputEvent& event = lastEvent;

    event.keyFlags = 0;

    if (flags & kbd_ctrl)
    {
        event.keyFlags |= kbd_ctrl;
    }

    if (flags & kbd_alt)
    {
        event.keyFlags |= kbd_alt;
    }

    if (flags & kbd_shift)
    {
        event.keyFlags |= kbd_shift;
    }

    updmode(event);

    if (mode == GridMode_Ctrl)
    {
        if (activeNote != NULL)
        {
            //activeNote->preview();
            //previewNote = activeNote;
        }
    }

    checkpos(event);

    place->update();

    updcursor(lastEvent);
}

void Grid::updpos(InputEvent & ev, bool textCursor)
{
    getpos(ev.mouseX, ev.mouseY, &alignTick, &alignLine);

    //if (alignLine >= MInstrPanel->getInstrNum() || alignLine < 0)
    //    alignLine = -1;

    updmode(ev);

    currTick = getTickFromX(ev.mouseX);

    checkpos(ev);
}

void Grid::handleMouseMove(InputEvent & ev)
{
    newEvent = ev;

    updpos(ev);

    if (mode == GridMode_Ctrl && activeNote != NULL && activeNote != previewNote)
    {
        //activeNote->preview();
        //previewNote = activeNote;
    }

    if(mode == GridMode_Selecting)
    {
        action(GridAction_SelectRectangle);
    }

    place->update();

    lastEvent = ev;

    updcursor(ev);
}

void Grid::handleMouseDown(InputEvent& ev)
{
    mouseIsDown = true;

    updpos(ev, true);

    place->update();

    actionLine = alignLine;
    currTick = alignTick;

    if (displayMode == GridDisplayMode_Steps || displayMode == GridDisplayMode_Bars)
    {
        if (mode == GridMode_ElemResizing)
        {
            if(!activeElem->isSelected())
            {
                selreset(true);
            }
        }
        else if(ev.leftClick)
        {
            dragTickStart = alignTick;
            dragLineStart = alignLine;

            if (selected.size() > 0 && activeElem != NULL && activeElem->isSelected())
            {
                if (mode == GridMode_Shift)
                {
                    setmode(GridMode_Cloning);
                }
                else
                {
                    setmode(GridMode_Moving);
                }
            }
            else
            {
                if (alignLine >= 0)
                {
                    if(activeNote == NULL)
                    {
                        grabTextCursor(alignTick, alignLine);

                        selreset(true);

                        //activeNote = putnote(alignTick, alignLine, -1);
                        //activeNote->recalculate();
                        //activeNote->preview(-1, true);
                        //setactivelem(activeNote);
                        //place->update();

                        //setmode(GridMode_Moving);
                    }
                    else
                    {
                        if (mode == GridMode_Ctrl)
                        {
                            Note* nt = activeNote;

                            if (!nt->isSelected())
                            {
                                selected.remove(nt);
                            
                                selected.push_back(nt);
                            
                                nt->markSelected(true);
                            
                                redraw(false, false);
                            }
                            else 
                            {
                                selected.remove(nt);
                            
                                nt->markSelected(false);
                            
                                redraw(false, false);
                            }
                        }
                        else
                        {
                            selreset(true);

                            activeNote->preview(-1, true);

                            if (displayMode == GridDisplayMode_Steps)
                            {
                                setmode(GridMode_Brushing);
                            }
                            else
                            {
                                if (mode == GridMode_Shift)
                                {
                                    setmode(GridMode_Cloning);
                                }
                                else
                                {
                                    setmode(GridMode_Moving);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
        /*
            if (mode != GridMode_Selecting)
            {
                setmode(GridMode_Selecting);

                selreset(true);
            }
        */
            if(activeNote != NULL && activeNote->isSelected())
            {

            }
            else
            {
                selreset(true);
            }

/*
            if(place->currNote != NULL)
            {
                DeleteElement(place->currNote);

                setactivelem(NULL);

                redraw(true, false);
            }

            setmode(GridMode_Deleting);
*/
        }
    }
    else if (mode == GridMode_VolPanning)
    {
        action(GridAction_VolPanChange);
    }

    lastEvent = ev;
}

void Grid::handleMouseUp(InputEvent& ev)
{
    mouseIsDown = false;

    prevX = -1;
    prevY = -1;

    if (ev.leftClick)
    {
        if (mode == GridMode_ElemResizing)
        {
            ///
        }
        else
        {
            if (activeElem != NULL)
            {
                activeElem->handleMouseUp(ev);
            }
            else
            {
                clickscroll(ev);
            }
        }

        if (lastAction == GridAction_Reset)
        {
            //selreset(true);
        }
    }
    else
    {
        if (mode == GridMode_Selecting)
        {
            selreset(false);
        }
    }

    action(GridAction_Reset);

    lastEvent = ev;
}

int Grid::getselnum()
{
    return selected.size();
}

void Grid::handleMouseDrag(InputEvent& ev)
{
    newEvent = ev;

    if(lastEvent.mouseX == newEvent.mouseX && lastEvent.mouseY == newEvent.mouseY )
    {
        updpos(ev);

        return;
    }

    getpos(ev.mouseX, ev.mouseY, &alignTick, &alignLine);

    if (alignLine >= MInstrPanel->getInstrNum())
    {
        alignLine = -1;
    }

    if (displayMode == GridDisplayMode_Bars)
    {
        if(ev.leftClick)
        {
            if (mode == GridMode_Brushing)
            {
                action(GridAction_Brush);
            }
            else if (mode == GridMode_Moving)
            {
                action(GridAction_Move, alignTick - dragTickStart, alignLine - dragLineStart);
            }
            else if (mode == GridMode_Cloning)
            {
                action(GridAction_Clone, alignTick - dragTickStart, alignLine - dragLineStart);
            }
            else if (mode == GridMode_ElemResizing)
            {
                //action(GridAction_Resize, alignTick - activeElem->getStartTick(), alignLine);
                action(GridAction_Resize2, alignTick - prevTick, alignLine);

                prevTick = alignTick;
            }
        }
        else
        {
            if(mode != GridMode_Selecting)
            {
                mode = GridMode_Selecting;
            }

            if(mode == GridMode_Selecting)
            {
                action(GridAction_SelectRectangle);
            }
        }
    }
    else
    {
        action(GridAction_VolPanChange);
    }

    updbounds();

    redraw(true, false);

    //place->update();

    dragTickStart = alignTick;
    dragLineStart = alignLine;

    lastEvent = ev;
}

void Grid::handleMouseWheel(InputEvent& ev)
{
    newEvent = ev;

    if (place->currNote != NULL )
    {
        if (selected.size() > 0)
        {
            action(GridAction_WheelVolume);
        }
        else
        {
            Note*  note = place->currNote;

            if (note)
            {
                float fraction = 1.f/(getLineHeight() - 1);

                float newVal = note->vol->getValue();

                newVal += fraction*ev.wheelDelta;

                note->vol->setValue(newVal);

                redraw(true);
            }
        }
    }
    else
    {
        if (ev.keyFlags & kbd_ctrl)
        {
            if(ev.wheelDelta > 0)
            {
                while(ev.wheelDelta > 0)
                {
                    changeScale(1, ev.mouseX);

                    ev.wheelDelta--;
                }
            }
            else
            {
                while(ev.wheelDelta < 0)
                {
                    changeScale(-1, ev.mouseX);

                    ev.wheelDelta++;
                }
            }

            if(mode == GridMode_Selecting)
            {
                action(GridAction_SelectRectangle);
            }
        }
        else
        {
            // Horizontal
            //float ofsDelta = ev.wheelDelta*(visibleTickSpan*0.03f);
            //setTickOffset(getTickOffset() - ofsDelta);
            
            // Vertical
            vscr->setoffs(vscr->getoffs() - ev.wheelDelta * (lheight * .5f));
            //vscr->setOffset(vscr->getOffset() - ev.wheelDelta*(lheight*.5f));

            //MInstrPanel->setOffset((int)(MInstrPanel->getOffset() - ev.wheelDelta*int(InstrHeight*1.1f)));

            //MInstrPanel->setOffset(int(verticalGridScroller->getOffset()));
        }
    }

    lastEvent = ev;
}

void Grid::clickscroll(InputEvent& ev)
{
    float area = 0.1f;
    float click =(alignTick - hscr->getoffs())/hscr->getvisible();

    if(click < area)
    {
        hscr->adjoffs(-(area - click)*hscr->getvisible());
    }
    else if(click > (1 - area))
    {
        hscr->adjoffs((click - (1 - area))*hscr->getvisible());
    }

/*
    click = (alignLine*lheight - vscr->getoffs())/vscr->getvisible();

    if(click < area)
    {
        vscr->adjoffs(-(area - click)*vscr->getvisible());
    }
    else if(click > (1.f - area))
    {
        vscr->adjoffs((click - (1 - area))*vscr->getvisible());
    }*/

    handleMouseMove(ev);
}

void Grid::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if (obj == vscr || obj == hscr)
    {
        redraw(true, true);
    }
}

void Grid::setactivelem(Element* el)
{
    activeElem = el; 

    activeNote = dynamic_cast<Note*>(el);

    if (activeNote != previewNote && previewNote != NULL)
    {
        previewNote->releasePreview();

        previewNote = NULL;
    }
}

float Grid::getticksnap(float val)
{
    return float(int(val/snap))*snap;
}

void Grid::getpos(int mx, int my, float* tick, int* line)
{
    snap = MCtrllPanel->getSnapMenu().getSnapSize();

    if(snap == 0)
    {
        *tick = getTickFromX(mx);
    }
    else
    {
        *tick = getticksnap(getTickFromX(mx));
    }

    *line = getLineFromY(my);
}

bool Grid::handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my)
{
    Instrument* i = dynamic_cast<Instrument*>(obj);

    if(i != NULL)
    {
        float tick;
        int line;

        getpos(mx, my, &tick, &line);

/*
        if((tick - tickOffset)/visibleTickSpan > 0.9f)
        {
            float offs = (tick - tickOffset) - visibleTickSpan*0.9f;

            setTickOffset(tickOffset + offs, false);
        }*/

        Note* n = i->selfNote;

        n->setPos(tick, line);

        n->calcCoordsForGrid(this);

        drag.setCoords1(n->x1, n->y1, n->width, n->height);

        return true;
    }

    return false;
}

bool Grid::handleObjDrop(Gobj * obj,int mx,int my, unsigned flags)
{
    Instrument* i = dynamic_cast<Instrument*> (obj);

    if(i != NULL)
    {
        float tick;
        int line;

        getpos(mx, my, &tick, &line);

        Note* note = putnote(tick, line, BaseNote);
        note->recalculate();

        MAudio->releaseAllPreviews();

        return true;
    }

    return false;
}

bool Grid::drawDraggedObject(Graphics & g,Gobj * obj)
{
    Instrument* i = dynamic_cast<Instrument*> (obj);

    if(i != NULL)
    {
        i->selfNote->drawOnGrid(g, this);

        return true;
    }

    return false;
}

void Grid::selall(bool select)
{
    selected.clear();

    for(Element* el : patt->elems)
    {
        if(!el->isDeleted())
        {
            if(select)
            {
                el->markSelected(true);

                selected.push_back(el);
            }
            else
            {
                el->markSelected(false);
            }
        }
    }

    redraw(true);
}

void Grid::selreset(bool deselect)
{
    selStartX = selStartY = -1;

    if(deselect)
    {
        selall(false);
    }

    sel->setCoords1(0, 0, 1, 1);
    sel->setVisible(false);
}

void Grid::changebars(InputEvent& ev)
{
    int         tx1 = x1;
    int         tx2 = x2;
    int         ty2 = getYfromLine(actionLine) - 1;
    int         ty1 = ty2 - lheight + 2;
    int         tH = lheight - 1;
    int         setY = ev.mouseY - ty1;

    if (prevX == -1 && prevY == -1)
    {
        prevX = ev.mouseX;
        prevY = setY;
    }

    float       newVal;
    bool        setDefault = (ev.keyFlags & kbd_alt);
    bool        processSelectedOnly = false;

    if (selected.size() > 0)
    {
        processSelectedOnly = true;
    }

    Instrument* instr = MInstrPanel->getInstrFromLine(actionLine);

    for(Note* note : instr->notes)
    {
        int noteX = note->getX1();

        if (note->isShown() && (noteX >= tx1 && noteX <= tx2) && (!processSelectedOnly || note->isSelected()))
        {
            bool doChange = false;

            if ((ev.leftClick || prevX == ev.mouseX) && abs(ev.mouseX - noteX) <= 3)
            {
                newVal = (float)(tH - setY) / (float)tH;

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
            else if ((noteX >= prevX && noteX <= ev.mouseX) || (noteX >= ev.mouseX && noteX <= prevX))
            {
                newVal = (1.0f - (Interpolate_Line((float)prevX, (float)prevY, (float)ev.mouseX, (float)setY, (float)noteX) / (float)tH));

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
                Parameter* param = note->getParamByDisplayMode(displayMode);

                if (setDefault)
                {
                    newVal = param->getDefaultVal();
                }
                else
                {
                    newVal = newVal*param->getRange() + param->getOffset();
                }

                newVal = param->adjustForEditor(newVal);

                MHistory->addNewAction(HistAction_ParamChange, (void*)param, param->getValue(), newVal, 0, 0);

                updateList.push_back(note);
            }
        }
    }

    if (ev.leftClick)
    {
        prevX = ev.mouseX;
        prevY = setY;
    }
}

void Grid::delacross(int mx1, int my1, int mx2, int my2)
{
    if(mx1 == -1)
    {
        mx1 = mx2;
    }

    if(my1 == -1)
    {
        my1 = my2;
    }

    std::list<Element*> elemsDelete;

    for(Element* el : visible)
    {
        if(el->checkVisible(this) && CheckPlaneCrossing(el->getDrawX1(), el->getDrawY1(), el->getDrawX2(), el->getDrawY2(), mx1, my1, mx2, my2))
        {
            elemsDelete.push_back(el);
        }
    }

    MHistory->addNewAction(HistAction_DeleteGroup, elemsDelete);
}

void Grid::reassign()
{
    std::list<Element*> elems;

    if (selected.size() == 0)
    {
        elems.push_back(activeElem);
    }
    else
    {
        elems = selected;
    }

    std::list<Element*>  newSelected;

    for(Element* el : elems)
    {
        Note* note = dynamic_cast<Note*>(el);

        if (note)
        {
            Instrument* instrNew = MInstrPanel->getInstrFromLine(note->getLine());
            Instrument* instrOld = note->getInstr();

            if (instrOld != instrNew)
            {
                Note* newNote = _Create_Note(note->getStartTick(), note->getLine(), instrNew, note->getNoteValue(), 
                                                note->getTickLength(), note->vol->getValue(), note->pan->getValue(), patt);

                if (selected.size() > 0)
                {
                    newSelected.push_back(newNote);
                    newNote->markSelected(true);
                }
                else
                {
                    setactivelem(newNote);
                }
            }
        }
    }

    MHistory->addNewAction(HistAction_DeleteGroup, elems);

    if (selected.size() > 0)
    {
        selected.clear();
        selected.splice(selected.begin(), newSelected);
    }

    MGrid->redraw(true);
}

Note* Grid::putnote(float tick, int line, int noteVal)
{
    Instrument* instr = MInstrPanel->getCurrInstr();

    if(instr != NULL)
    {
        MInstrPanel->setCurrInstr(instr);

        Note* newNote = _Create_Note(tick, line, instr, noteVal > 0 ? noteVal : instr->lastNoteVal, 
                                        instr->lastNoteLength, instr->lastNoteVol, instr->lastNotePan, getPattern());

        updbounds();

        redraw(true);

        return newNote;
    }
    else
    {
        return NULL;
    }
}

void Grid::action(GridAction act, float dTick, int dLine)
{
    static bool     hasCloned = false;
    static bool     hasCut = false;
    static bool     hasMoved = false;
    static float    firstElemTick;
    static int      firstElemLine;

    // Need remember first tick and line in clipboard

    firstElemTick = -1;
    firstElemLine = -1;

    if(clipboard.size() > 0)
    {
        for(Element* el : clipboard)
        {
            if(firstElemTick == -1 || el->getStartTick() < firstElemTick)
            {
                firstElemTick = el->getStartTick();
            }

            if(firstElemLine == -1 || el->getLine() < firstElemLine)
            {
                firstElemLine = el->getLine();
            }
        }
    }

    // Now perform action

    if(act == GridAction_PutNote)
    {
        
    }
    else if(act == GridAction_Brush)
    {
        if(alignTick >= 0 && currTick != alignTick)
        {
            float brushStep = snap;
            float xtick = currTick;

            if (mode == GridMode_Ctrl)
            {
                brushStep = activeElem->getTickLength();
                xtick = activeElem->getStartTick();
            }

            if(alignTick < currTick)
            {
                brushStep = -brushStep;     // Going backwards
            }

            while(1)
            {
                if (patt->checkElement(xtick, actionLine) == NULL)
                {
                    Element* el;

                    if(activeElem != NULL)
                    {
                        el = activeElem->clone();

                        el->setPos(xtick, actionLine);
                    }
                    else
                    {
                        el = putnote(xtick, actionLine, BaseNote);  // _MKeyboard->createNote(false, -1);
                    }

                    setactivelem(el);

                    updateList.push_back(el);
                }

                xtick += brushStep;

                if(alignTick > currTick)
                {
                    if(xtick > alignTick)
                    {
                        break;      // Every position filled, exit
                    }
                }
                else
                {
                    if(xtick < alignTick)
                    {
                        break;      // Every position filled, exit
                    }
                }
            }

            currTick = alignTick;

            redraw(true, false);
        }
    }
    else if(act == GridAction_VolPanChange)
    {
        changebars(newEvent);

        redraw(true);
    }
    else if(act == GridAction_WheelVolume)
    {
        for(auto el : selected)
        {
            Note* note = dynamic_cast<Note*>(el);

            if (note)
            {
                //float range = note->vol->getRange();

                float fraction = 1.f/(getLineHeight() - 1);

                float newVal = note->vol->getValue();

                newVal += fraction*newEvent.wheelDelta;

                note->vol->setValue(newVal);
            }
        }

        updateList = selected;

        redraw(true);
    }
    else if(act == GridAction_Cut)
    {
        hasCut = true;

        clipboard.clear();

        if(getselnum() == 0)
        {
            if (activeElem)
            {
                clipboard.push_back(activeElem);
            }
        }
        else
        {
            for(auto el : selected)
            {
                clipboard.push_back(el);
            }
        }

        if (clipboard.size() > 0)
        {
            updateList = clipboard;

            MHistory->addNewAction(HistAction_DeleteGroup, clipboard);

            MInstrPanel->updateInstrNotePositions();

            redraw(true);
        }
    }
    else if(act == GridAction_Delete)
    {
        //if (alignLine == actionLine)
        //    delacross(lastEvent.mouseX, getYfromLine(actionLine) - lheight, newEvent.mouseX, getYfromLine(actionLine) - 1);

        std::list<Element*> elem;

        if(getselnum() == 0)
        {
            if (activeElem)
            {
                elem.push_back(activeElem);
            }
        }
        else
        {
            for(auto el : selected)
            {
                elem.push_back(el);
            }
        }

        if (elem.size() > 0)
        {
            updateList = elem;

            MHistory->addNewAction(HistAction_DeleteGroup, elem);

            MInstrPanel->updateInstrNotePositions();

            redraw(true);
        }
    }
    else if(act == GridAction_Move)
    {
        if(getselnum() == 0)
        {
            // turn off moving across instruments for now
            //dLine = 0;

            if(activeElem->getStartTick() + dTick < 0)
            {
                dTick -= activeElem->getStartTick() + dTick;
            }

            if(activeElem->getLine() + dLine < 0)
            {
                dLine -= activeElem->getLine() + dLine;
            }

            if(dTick != 0 || dLine != 0)
            {
                std::list<Element*>  elem;
                elem.push_back(activeElem);

                //MHistory->addNewAction(HistAction_Move, (void*)activeElem, dTick, 0, dLine, 0);
                MHistory->addNewAction(HistAction_MoveGroup, elem, dTick, dLine);

                updateList = elem;
            }

            if (dLine != 0)
            {
                //reassign();
            }
        }
        else
        {
            float   ct = 0;
            int     cl = 0;

            // turn off moving across instruments for now
            //dLine = 0;

            // Handle moving limits, can't get < 0

            for(Element* el : selected)
            {
                if(el->getStartTick() + dTick < ct)
                {
                    ct = el->getStartTick() + dTick;
                }

                if(el->getLine() + dLine < cl)
                {
                    cl = el->getLine() + dLine;
                }
            }

            dTick -= ct;
            dLine -= cl;

            MHistory->addNewAction(HistAction_MoveGroup, selected, dTick, dLine);

            updateList = selected;

            if (dLine != 0)
            {
                //reassign();
            }
        }

        redraw(true);
    }
    else if(act == GridAction_Clone)
    {
        // turn off moving across instruments for now
        //dLine = 0;

        if(getselnum() == 0)
        {
            if(hasCloned == false)
            {
                hasCloned = true;

                setactivelem(activeElem->clone());
            }

            std::list<Element*> elem;
            elem.push_back(activeElem);

            MHistory->addNewAction(HistAction_MoveGroup, elem, dTick, dLine);

            if (dLine != 0)
            {
                //reassign();
            }

            updateList = elem;

            redraw(true);
        }
        else
        {
            setactivelem(NULL);

            if(hasCloned == false)
            {
                hasCloned = true;

                std::list<Element*>    cloned;

                for(Element* el : selected)
                {
                    el->markSelected(false);

                    Element* cl = el->clone();

                    cl->markSelected(true);

                    cloned.push_back(cl);
                }

                selected.clear();
                selected.splice(selected.begin(), cloned);

                cloned.clear();
            }

            MHistory->addNewAction(HistAction_MoveGroup, selected, dTick, dLine);

            if (dLine != 0)
            {
                //reassign();
            }

            updateList = selected;

            redraw(true);
        }
    }
    else if(act == GridAction_Resize)
    {
        float newLen = dTick;

        if (newLen < getSnapSize())
        {
            newLen = getSnapSize();
        }

        if(newLen != activeElem->getTickLength())
        {
            std::list<Element*> resized;

            float delta = newLen - activeElem->getTickLength();

            MHistory->addNewAction(HistAction_Resize, (void*)activeElem, activeElem->getTickLength(), newLen, 0, 0);

            resized.push_back(activeElem);

            for(Element* el : selected)
            {
                if(el != activeElem)
                {
                    newLen = el->getTickLength() + delta;

                    if (newLen < getSnapSize())
                    {
                        newLen = getSnapSize();
                    }

                    if(newLen != el->getTickLength())
                    {
                        resized.push_back(el);

                        MHistory->addNewAction(HistAction_Resize, (void*)el, el->getTickLength(), newLen, 0, 0);
                    }
                }
            }

            updateList = resized;

            redraw(true);
        }
    }
    else if(act == GridAction_Resize2)
    {
        float deltaTick = dTick;

        if(deltaTick != 0 && 
                activeElem->getTickLength() + deltaTick > 0 && 
                    activeElem->getTickLength() + deltaTick != activeElem->getTickLength())
        {
            std::list<Element*> resized;

            resized.push_back(activeElem);

            for(Element* el : selected)
            {
                if(el != activeElem)
                {
                    if(el->getTickLength() + deltaTick > 0)
                    {
                        resized.push_back(el);
                    }
                }
            }

            MHistory->addNewAction(HistAction_Resize2, resized, deltaTick);

            updateList.splice(updateList.end(), resized);

            redraw(true);
        }
    }
    else if(act == GridAction_Copy)
    {
        hasCut = false;

        clipboard.clear();

        if(getselnum() == 0 && activeElem != NULL)
        {
            clipboard.push_back(activeElem);
        }
        else
        {
            for(auto el : selected)
            {
                clipboard.push_back(el);
            }
        }
    }
    else if(act == GridAction_Paste)
    {
        selected.clear();

        if(hasCut)
        {
            // If cut, then undelete and move to needed position

            MHistory->addNewAction(HistAction_AddGroup, clipboard, 0, 0);
            MHistory->addNewAction(HistAction_MoveGroup, clipboard, alignTick - firstElemTick, 0);

            for(Element* el : clipboard)
            {
                el->markSelected(true);

                selected.push_back(el);
            }

            hasCut = false;

            updateList = selected;
        }
        else
        {
            // If copied, then clone and add to needed position

            for(Element* el : clipboard)
            {
                el->markSelected(false);

                Element* cl = el->clone();

                if (cl)
                {
                    //cl->setPos(cl->getStartTick() + alignTick - firstElemTick, cl->getLine());

                    cl->markSelected(true);

                    selected.push_back(cl);

                    //MHistory->addNewAction(HistAction_Add, (void*)cl, 0, 0, 0, 0);
                }
            }

            MHistory->addNewAction(HistAction_AddGroup, selected, 0, 0);
            MHistory->addNewAction(HistAction_MoveGroup, selected, alignTick - firstElemTick, 0);

            clipboard.clear();
            clipboard = selected;

            updateList = selected;
            //reassign();
        }

        redraw(true);
    }
    else if(act == GridAction_SelectRectangle)
    {
        if(selStartX == -1)
        {
            selStartX = 0;
            selStartY = 0;

            selStartX = newEvent.mouseX - getX1();
            selStartY = newEvent.mouseY - getY1();

            sel->setCoords1(selStartX, selStartY, 0, 0);
        }
        else
        {
            int yNew = newEvent.mouseY - getY1();
            int xNew = newEvent.mouseX - getX1();

            LIMIT(yNew, 0, getH() - 1);
            LIMIT(xNew, 0, getW() - 1);

            int xS = selStartX;
            int yS = selStartY;

            int xN = xNew;
            int yN = yNew;

            if(xNew < selStartX)
            {
                xS = xNew;
                xN = selStartX;
            }

            if(yNew < selStartY)
            {
                yS = yNew;
                yN = selStartY;
            }

            sel->setCoords2(xS, yS, xN, yN);

            selTickStart = getticksnap(getTickFromX(sel->getDrawX1()));
            selTickEnd = getticksnap(getTickFromX(sel->getDrawX2()));

            selLineStart = getLineFromY(sel->getDrawY1());
            selLineEnd = getLineFromY(sel->getDrawY2());

            // Adjust selection to snap setting

            sel->setCoords2(getXfromTick(selTickStart) - getX1(), getYfromLine(selLineStart) - getY1() - lheight, 
                                        getXfromTick(selTickEnd) - getX1(), getYfromLine(selLineEnd) - getY1());

            for(Element* el : visible)
            {
                if (iselselected(el))
                {
                    selected.remove(el);

                    selected.push_back(el);

                    el->markSelected(true);

                    redraw(false, false);
                }
                else if(el->isSelected())
                {
                    selected.remove(el);

                    el->markSelected(false);

                    redraw(false, false);
                }

            }
        }
    }

    else  if (act == GridAction_Reset)
    {
        hasCloned = false;

        updateChangedElements();

        setmode(GridMode_Default);
    }

    updbounds();

    lastAction = act;
}

bool Grid::iselselected(Element* el)
{
    if (displayMode == GridDisplayMode_Bars)
    {
        float ex1 = el->getStartTick();
        float ex2 = el->getEndTick();
        float ey1 = (float)el->getLine();
        float ey2 = ey1;

        if ( CheckPlaneCrossing(ex1, ey1, ex2, ey2, selTickStart, (float)selLineStart, selTickEnd, (float)selLineEnd))
        {
            return true;
        }
    }
    else
    {
        if (el->getStartTick() >= selTickStart && el->getStartTick() < selTickEnd && el->getLine() >= selLineStart && el->getLine() <= selLineEnd )
        {
            return true;
        }
    }

    return false;
}

ContextMenu* Grid::createContextMenu()
{
    if(wasSelecting)
    {
        wasSelecting = false;

        return NULL;
    }

    ContextMenu* menu = new ContextMenu(this);

    if(activeNote != NULL)
    {
        menu->addMenuItem("Copy");
        menu->addMenuItem("Cut");
        menu->addMenuItem("Duplicate");
    }
    else
    {
        if(clipboard.size() > 0)
        {
            menu->addMenuItem("Paste");
        }

        menu->addMenuItem("Create Pianoroll");
    }

    return menu;
}

void Grid::activateContextMenuItem(std::string item)
{
    if(item == "Paste")
    {
        MGrid->action(GridAction_Paste);
    }
    else if(item == "Copy")
    {
        MGrid->action(GridAction_Copy);
    }
    else if(item == "Cut")
    {
        MGrid->action(GridAction_Cut);
    }

    updateChangedElements();
}

void Grid::updateChangedElements()
{
    for(Element* el : updateList)
    {
        el->recalculate();
    }

    updateList.clear();
}


