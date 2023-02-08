

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

public:

        Selection(Grid* grd)
        {
            settouchable(false);

            grid = grd;
        }

protected:

        Grid*       grid;

        void        drawSelf(Graphics& g)
        {
            g.saveState();
            //g.reduceClipRegion(x1, y1, width, height);

            fill(g, 1.f, .1f);

            rect(g, 1.f, .3f);

            g.restoreState();
        }
};


class PlaceHighlight : public Gobj
{
friend  Grid;

        float               tick;
        int                 line;
        Grid*               grid;
        Note*               currNote;

public:

        PlaceHighlight(Grid* grd)
        {
            settouchable(false);

            grid = grd;

            currNote = NULL;

            tick = 0;
            line = 0;
        }

        void setpos(float newTick, int newLine)
        {
            tick = newTick;
            line = newLine;

            if (grid->isundermouse())
            {
                int ly = grid->getYfromLine(line);
                int lh = grid->getLineHeight();

                if (grid->getDisplayMode() == GridDisplayMode_Bars)
                {
                    currNote = grid->actnote;

                    if (currNote && !currNote->isdel())
                    {
                        if (!currNote->issel())
                        {
                            int x = currNote->getX1() - grid->getX1();
                            int y = currNote->getY1() - grid->getY1();

                            setCoords1(x, y, currNote->getW(), currNote->getH());

                            setVis(true);
                        }
                    }
                    else if (grid->mode == GridMode_Default)
                    {
                        int x = grid->getXfromTick(tick) - grid->getX1();
                        int y = grid->getYfromLine(line) - int(grid->getLineHeight()) - grid->getY1() + 1;

                        setCoords2(x, y, x + grid->snap*grid->getPixelsPerTick() - 1, y + int(grid->getLineHeight()) - 1);

                        setVis(true);
                    }
                }
            }
        }

        void update()
        {
            setVis(false);

            if((grid->isundermouse()) && grid->getDisplayMode() == GridDisplayMode_Bars && grid->mode != GridMode_Selecting)
            {
                if (grid->alignLine >= 0)
                {
                    setpos(grid->alignTick, grid->alignLine);
                }
            }
        }

        void drawSelf(Graphics& g)
        {
            if (currNote && !currNote->isdel())
            {
                if (!currNote->issel())
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
    actnote = previewNote = NULL;

    mainimg = NULL;
    brushImage = NULL;
    elemImage = NULL;

    brushWidthTicks = 0;
    alignTick = 0;
    alignLine = 0;

    lastElementEndTick = 0;
    mouseIsDown = false;
    wasSelecting = false;

    dispmode = GridDisplayMode_Bars;

    addHighlight(sel = new Selection(this));
    addHighlight(place = new PlaceHighlight(this));

    selReset();

    setActionMode(GridMode_Default);

    lastAction = GridAction_Release;
}

void Grid::activateMenuItem(std::string item)
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

    recalcElements();
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
            if(firstElemTick == -1 || el->gettick() < firstElemTick)
            {
                firstElemTick = el->gettick();
            }

            if(firstElemLine == -1 || el->getline() < firstElemLine)
            {
                firstElemLine = el->getline();
            }
        }
    }

    // Now perform action

    if(act == GridAction_PutNote)
    {
        Note* nt = putNote(dTick, dLine, BaseNote);

        updateList.push_back(nt);
    }
    else if(act == GridAction_Brush)
    {
        if(alignTick >= 0 && currTick != alignTick)
        {
            float brushStep = snap;
            float xtick = currTick;

            if (mode == GridMode_Ctrl)
            {
                brushStep = activeElem->getticklen();
                xtick = activeElem->gettick();
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
                        el = putNote(xtick, actionLine, BaseNote);  // _MKeyboard->createNote(false, -1);
                    }

                    setActivElement(el);

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
        changeBars(newEvent);

        redraw(true);
    }
    else if(act == GridAction_WheelVolume)
    {
        for(auto el : selected)
        {
            Note* note = dynamic_cast<Note*>(el);

            if (note)
            {
                note->getVol()->adjustFromControl(NULL, newEvent.wheelDelta, 0, 1.f/(getLineHeight() - 1));
            }
        }

        updateList = selected;

        redraw(true);
    }
    else if(act == GridAction_Cut)
    {
        hasCut = true;

        clipboard.clear();

        if(getNumSelectedElements() == 0)
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

            redraw(true);
        }
    }
    else if(act == GridAction_Delete)
    {
        //if (alignLine == actionLine)
        //    deleteAcross(lastEvent.mouseX, getYfromLine(actionLine) - lheight, newEvent.mouseX, getYfromLine(actionLine) - 1);

        std::list<Element*> elem;

        if(getNumSelectedElements() == 0)
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

            redraw(true);
        }
    }
    else if(act == GridAction_Move)
    {
        if(getNumSelectedElements() == 0)
        {
            // turn off moving across instruments for now
            //dLine = 0;

            if(activeElem->gettick() + dTick < 0)
            {
                dTick -= activeElem->gettick() + dTick;
            }

            if(activeElem->getline() + dLine < 0)
            {
                dLine -= activeElem->getline() + dLine;
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
                //reassignElements();
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
                if(el->gettick() + dTick < ct)
                {
                    ct = el->gettick() + dTick;
                }

                if(el->getline() + dLine < cl)
                {
                    cl = el->getline() + dLine;
                }
            }

            dTick -= ct;
            dLine -= cl;

            MHistory->addNewAction(HistAction_MoveGroup, selected, dTick, dLine);

            updateList = selected;

            if (dLine != 0)
            {
                //reassignElements();
            }
        }

        redraw(true);
    }
    else if(act == GridAction_Clone)
    {
        // turn off moving across instruments for now
        //dLine = 0;

        if(getNumSelectedElements() == 0)
        {
            if(hasCloned == false)
            {
                hasCloned = true;

                setActivElement(activeElem->clone());
            }

            std::list<Element*> elem;
            elem.push_back(activeElem);

            MHistory->addNewAction(HistAction_MoveGroup, elem, dTick, dLine);

            if (dLine != 0)
            {
                //reassignElements();
            }

            updateList = elem;

            redraw(true);
        }
        else
        {
            setActivElement(NULL);

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
                //reassignElements();
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

        if(newLen != activeElem->getticklen())
        {
            std::list<Element*> resized;

            float delta = newLen - activeElem->getticklen();

            MHistory->addNewAction(HistAction_Resize, (void*)activeElem, activeElem->getticklen(), newLen, 0, 0);

            resized.push_back(activeElem);

            for(Element* el : selected)
            {
                if(el != activeElem)
                {
                    newLen = el->getticklen() + delta;

                    if (newLen < getSnapSize())
                    {
                        newLen = getSnapSize();
                    }

                    if(newLen != el->getticklen())
                    {
                        resized.push_back(el);

                        MHistory->addNewAction(HistAction_Resize, (void*)el, el->getticklen(), newLen, 0, 0);
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
                activeElem->getticklen() + deltaTick > 0 && 
                    activeElem->getticklen() + deltaTick != activeElem->getticklen())
        {
            std::list<Element*> resized;

            resized.push_back(activeElem);

            for(Element* el : selected)
            {
                if(el != activeElem)
                {
                    if(el->getticklen() + deltaTick > 0)
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

        if(getNumSelectedElements() == 0 && activeElem != NULL)
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
            //reassignElements();
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

            selTickStart = getSnap(getTickFromX(sel->getDrawX1()));
            selTickEnd = getSnap(getTickFromX(sel->getDrawX2()));

            selLineStart = getLineFromY(sel->getDrawY1());
            selLineEnd = getLineFromY(sel->getDrawY2());

            // Adjust selection to snap setting

            sel->setCoords2(getXfromTick(selTickStart) - getX1(), getYfromLine(selLineStart) - getY1() - lheight, 
                                        getXfromTick(selTickEnd) - getX1(), getYfromLine(selLineEnd) - getY1());

            for(Element* el : visible)
            {
                if (isElementSelected(el))
                {
                    selected.remove(el);

                    selected.push_back(el);

                    el->markSelected(true);

                    redraw(false, false);
                }
                else if(el->issel())
                {
                    selected.remove(el);

                    el->markSelected(false);

                    redraw(false, false);
                }

            }
        }
    }

    else  if (act == GridAction_Release)
    {
        hasCloned = false;

        recalcElements();

        setActionMode(GridMode_Default);
    }

    updBounds();

    lastAction = act;
}

void Grid::adjustScale(int delta, int mouseRefX)
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

    setPixelsPerTick(val, mouseRefX);
}

ContextMenu* Grid::createContextMenu()
{
    if(wasSelecting)
    {
        wasSelecting = false;

        return NULL;
    }

    ContextMenu* menu = new ContextMenu(this);

    if(actnote != NULL)
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

void Grid::clickScroll(InputEvent& ev)
{
    float area = 0.1f;
    float click =(alignTick - hscr->getOffset())/hscr->getVisiblePart();

    if(click < area)
    {
        hscr->adjustOffset(-(area - click)*hscr->getVisiblePart());
    }
    else if(click > (1 - area))
    {
        hscr->adjustOffset((click - (1 - area))*hscr->getVisiblePart());
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

void Grid::checkElementsAtPos(InputEvent & ev)
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
                if (el->getline() == alignLine)
                {
                    if (el->issel() && el->isPointed(ev.mouseX, ev.mouseY, this))
                    {
                        newEl = el;
                        break;
                    }
                    else if (el->gettick() == alignTick)
                    {
                        newEl = el;
                        break;
                    }
               }
            }
            else
            {
                if (el->getline() == alignLine)
                {
                    if (mode == GridMode_Alt)
                    {
                        if (abs(ev.mouseX - el->getX2()) < 10)
                        {
                            setActionMode(GridMode_ElemResizing);

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
        setActivElement(newEl);
    }
}

void Grid::changeBars(InputEvent& ev)
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

        if (note->isshown() && (noteX >= tx1 && noteX <= tx2) && (!processSelectedOnly || note->issel()))
        {
            bool change = false;

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

                change = true;
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

                change = true;
            }

            if (change)
            {
                Parameter* param = note->getParamByDisplayMode(dispmode);

                if (setDefault)
                {
                    newVal = param->getDefaultValue();
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

void Grid::deleteAcross(int mx1, int my1, int mx2, int my2)
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

void Grid::drawElements(Graphics& g)
{
    /*
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
*/

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

void Grid::drawSelf(Graphics& g)
{
    if(mainimg != NULL)
    {
        g.drawImageAt(mainimg, x1, y1);

        //gFillRect(g, x1, y1, x2, y2);
        //g.drawImage(mainimg, x1 + 40, y1 + 40, width - 80, height - 80, 40, 40, width - 80, height - 80);
    }

    if(bottomLine > 0 && (vscr->getOffset() + height)/lheight > bottomLine)
    {
        setc(g, 0.15f, .4f);
        int y = (bottomLine+1)*lheight - vscr->getOffset();
        fillx(g, 0, y, width, height - y);
    }

    //if(elemImage != NULL)
    //    g.drawImageAt(elemImage, x1, y1);

    drawElements(g);

// Testing text output:
    setc(g, 0xdfFFFFFF);
    gDString(g, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 297, 8, 8);
    gZxString(g, "0123456ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 320);

    gText(g, FontFix, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 340);
    gText(g, FontRox, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 360);
    gText(g, FontArial, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 380);
    gText(g, FontProject, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 400);
    gText(g, FontDix, "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", 550, 440);
}

void Grid::drawIntermittentHighlight(Graphics& g, int x, int y, int w, int h, int numBars)
{
    int tickLen = numBars*MTransp->getTicksPerBar();
    int pixLen = int(getPixelsPerTick() * tickLen);
    int xoffs = RoundFloat((getHoffs() / tickLen - (int)getHoffs() / tickLen) * getPixelsPerTick() * tickLen);
    int num = int(getHoffs() / tickLen);
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

void Grid::grabTextCursor(float tick, int line)
{
    addHighlight(MCursor);

    MCursor->setPos(tick, line);
}

float Grid::getSnap(float val)
{
    return float(int(val/snap))*snap;
}

void Grid::getPosFromCoords(int mx, int my, float* tick, int* line)
{
    snap = MCtrllPanel->getSnapMenu().getSnapSize();

    if(snap == 0)
    {
        *tick = getTickFromX(mx);
    }
    else
    {
        *tick = getSnap(getTickFromX(mx));
    }

    *line = getLineFromY(my);
}

int Grid::getNumSelectedElements()
{
    return selected.size();
}

Note* Grid::getNoteAtPos(float tick, int line)
{
    Note* note = NULL;

    for (Element* el : visible)
    {
        if(el->gettick() == tick && el->getline() == line)
        {
            note = dynamic_cast<Note*>(el);

            if (note)
            {
                break;
            }
        }
    }

    return note;
}

float Grid::getPixelsPerTick()
{
    return pixpertick;
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
    return RoundFloat((tick - hscr->getOffset())*pixpertick) + getX1();
}

float Grid::getTickFromX(int x)
{
    return ((float)x - getX1())/pixpertick + hscr->getOffset();
}

int Grid::getYfromLine(int line)
{
    return getY1() - vscr->getOffset() + line*lheight + lheight - 1;
}

int Grid::getLineFromY(int y)
{
    return (y - getY1() + vscr->getOffset())/lheight;
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

    //updmode(event);

    if (mode == GridMode_Ctrl)
    {
        if (actnote != NULL)
        {
            //actnote->preview();
            //previewNote = actnote;
        }
    }

    //checkElementsAtPos(event);

    updatePosition(event);

    place->update();

    updcursor(lastEvent);
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

void Grid::handleMouseMove(InputEvent & ev)
{
    newEvent = ev;

    updatePosition(ev);

    if (mode == GridMode_Ctrl && actnote != NULL && actnote != previewNote)
    {
        //actnote->preview();
        //previewNote = actnote;
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

    //updatePosition(ev);

    place->update();

    actionLine = alignLine;
    currTick = alignTick;

    if (dispmode == GridDisplayMode_Steps || dispmode == GridDisplayMode_Bars)
    {
        if (mode == GridMode_ElemResizing)
        {
            if(!activeElem->issel())
            {
                selReset(true);
            }
        }
        else if(ev.leftClick)
        {
            dragTickStart = alignTick;
            dragLineStart = alignLine;

            if (selected.size() > 0 && activeElem != NULL && activeElem->issel())
            {
                if (mode == GridMode_Shift)
                {
                    setActionMode(GridMode_Cloning);
                }
                else
                {
                    setActionMode(GridMode_Moving);
                }
            }
            else
            {
                if (alignLine >= 0)
                {
                    if(actnote == NULL)
                    {
                        grabTextCursor(alignTick, alignLine);

                        selReset(true);

                        //actnote = putNote(alignTick, alignLine, -1);
                        //actnote->recalculate();
                        //actnote->preview(-1, true);
                        //setActivElement(actnote);
                        //place->update();

                        //setActionMode(GridMode_Moving);
                    }
                    else
                    {
                        if (mode == GridMode_Ctrl)
                        {
                            Note* nt = actnote;

                            if (!nt->issel())
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
                            selReset(true);

                            actnote->preview(-1, true);

                            MInstrPanel->setCurrInstr(actnote->getInstr());

                            if (dispmode == GridDisplayMode_Steps)
                            {
                                setActionMode(GridMode_Brushing);
                            }
                            else
                            {
                                if (mode == GridMode_Shift)
                                {
                                    setActionMode(GridMode_Cloning);
                                }
                                else
                                {
                                    setActionMode(GridMode_Moving);
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
                setActionMode(GridMode_Selecting);

                selReset(true);
            }
        */
            if(actnote != NULL && actnote->issel())
            {

            }
            else
            {
                selReset(true);
            }

/*
            if(place->currNote != NULL)
            {
                DeleteElement(place->currNote);

                setActivElement(NULL);

                redraw(true, false);
            }

            setActionMode(GridMode_Deleting);
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
                clickScroll(ev);
            }
        }

        if (lastAction == GridAction_Release)
        {
            //selReset(true);
        }
    }
    else
    {
        if (mode == GridMode_Selecting)
        {
            selReset(false);
        }
    }

    action(GridAction_Release);

    lastEvent = ev;
}

void Grid::handleMouseDrag(InputEvent& ev)
{
    newEvent = ev;

    if(lastEvent.mouseX == newEvent.mouseX && lastEvent.mouseY == newEvent.mouseY )
    {
        updatePosition(ev);

        return;
    }

    getPosFromCoords(ev.mouseX, ev.mouseY, &alignTick, &alignLine);

    if (alignLine >= MInstrPanel->getNumInstrs())
    {
        alignLine = -1;
    }

    if (dispmode == GridDisplayMode_Bars)
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

    redraw(true, false);

    //place->update();

    dragTickStart = alignTick;
    dragLineStart = alignLine;

    lastEvent = ev;
}

void Grid::handleMouseWheel(InputEvent& ev)
{
    newEvent = ev;

    if (place->currNote != NULL && ev.keyFlags & kbd_alt)
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
                note->getVol()->adjustFromControl(NULL, ev.wheelDelta, 0, 1.f/(getLineHeight() - 1));

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
                    adjustScale(1, ev.mouseX);

                    ev.wheelDelta--;
                }
            }
            else
            {
                while(ev.wheelDelta < 0)
                {
                    adjustScale(-1, ev.mouseX);

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
            //setTickOffset(getHoffs() - ofsDelta);
            
            // Vertical
            vscr->setOffset(vscr->getOffset() - ev.wheelDelta * (lheight*1.5f));
            //vscr->setOffset(vscr->getOffset() - ev.wheelDelta*(lheight*.5f));

            //MInstrPanel->setOffset((int)(MInstrPanel->getOffset() - ev.wheelDelta*int(InstrHeight*1.1f)));

            //MInstrPanel->setOffset(int(verticalGridScroller->getOffset()));
        }
    }

    lastEvent = ev;
}

void Grid::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if (obj == vscr || obj == hscr)
    {
        redraw(true, true);
    }
}

bool Grid::handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my)
{
    Instrument* i = dynamic_cast<Instrument*>(obj);

    if(i != NULL)
    {
        float tick;
        int line;

        getPosFromCoords(mx, my, &tick, &line);

/*
        if((tick - tickOffset)/visibleTickSpan > 0.9f)
        {
            float offs = (tick - tickOffset) - visibleTickSpan*0.9f;

            setTickOffset(tickOffset + offs, false);
        }*/

        Note* n = i->selfNote;

        n->setPos(tick, line);

        n->calcForGrid(this);

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

        getPosFromCoords(mx, my, &tick, &line);

        action(GridAction_PutNote, tick, line);
        action(GridAction_Release);

        MAudio->releaseAllPreviews();

        return true;
    }

    return false;
}

bool Grid::isElementSelected(Element* el)
{
    if (dispmode == GridDisplayMode_Bars)
    {
        float ex1 = el->gettick();
        float ex2 = el->getendtick();
        float ey1 = (float)el->getline();
        float ey2 = ey1;

        if ( CheckPlaneCrossing(ex1, ey1, ex2, ey2, selTickStart, (float)selLineStart, selTickEnd, (float)selLineEnd))
        {
            return true;
        }
    }
    else
    {
        if (el->gettick() >= selTickStart && el->gettick() < selTickEnd && el->getline() >= selLineStart && el->getline() <= selLineEnd )
        {
            return true;
        }
    }

    return false;
}

Note* Grid::putNote(float tick, int line, int noteVal)
{
    Instrument* instr = MInstrPanel->getCurrInstr();

    if(instr != NULL)
    {
        MInstrPanel->setCurrInstr(instr);

        Note* newNote = AddNote(tick, line, instr, noteVal > 0 ? noteVal : instr->lastNoteVal, 
                                        instr->lastNoteLength, instr->lastNoteVol, instr->lastNotePan, getPattern());

        redraw(true);

        return newNote;
    }
    else
    {
        return NULL;
    }
}

void Grid::recalcElements()
{
    for(Element* el : updateList)
    {
        el->recalc();
    }

    updateList.clear();
}

void Grid::reassignElements()
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
            Instrument* instrNew = MInstrPanel->getInstrFromLine(note->getline());
            Instrument* instrOld = note->getInstr();

            if (instrOld != instrNew)
            {
                Note* newNote = AddNote(note->gettick(), note->getline(), instrNew, note->getNoteValue(), 
                                                note->getticklen(), note->vol->getValue(), note->pan->getValue(), patt);

                if (selected.size() > 0)
                {
                    newSelected.push_back(newNote);
                    newNote->markSelected(true);
                }
                else
                {
                    setActivElement(newNote);
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

void Grid::redraw(bool remap_objects, bool refresh_image)
{
    if(refresh_image)
    {
        updFillerImage();
        updBufferImage();

        timeline->redraw();
    }

    if(remap_objects)
    {
        remap();

        //updElementsImage();
    }

    Gobj::redraw();
}

void Grid::remap()
{
    updBufferImage();

    updBounds();

    remapElements();

    place->update();

    if (MCursor && MCursor->getParent() == this)
    {
        MCursor->updPos();
    }
}

void Grid::remapElements()
{
    visible.clear();

    std::list<Element*>::iterator it = visible.end();

    float lastStartTick = -1;
    float lastVisibleTick = hscr != NULL? hscr->getOffset() + hscr->getVisiblePart() : 0;

    if(MInstrPanel)
    {
        /*
        for(Instrument* i : MInstrPanel->instrs)
        {
            if(i->isshown())
            {
                visible.splice(visible.end(), i->getNotesFromRange(tickOffset, lastVisibleTick));
            }

            for(Element* el : visible)
            {
                el->calcCoordsForGrid(this);
            }
        }
        */

        for(Element* el : patt->getElems())
        {
            if ( el->getendtick() < hscr->getOffset() || el->gettick() > lastVisibleTick)
            {
                // Skip out-of-visible-area elements
            }
            else
            {
                if (el->isshown())
                {
                    if(visible.size() > 0)
                    {
                        // Place to the right position, so the list is sorted according to start tick
    
                        if(el->gettick() >= (*it)->gettick())
                        {
                            while(it != visible.end() && el->gettick() >= (*it)->gettick())
                            {
                                it++;
                            }
                        }
                        else
                        {
                            while(it != visible.begin())
                            {
                                --it;
    
                                if(el->gettick() >= (*it)->gettick())
                                {
                                    it++;
    
                                    break;
                                }
                            }
                        }
                    }

                    visible.insert(it, el);

                    it--;

                    el->calcForGrid(this);
                }
            }
        }
    }
}

void Grid::removeElement(Element * el)
{
    visible.remove(el);
    selected.remove(el);
    clipboard.remove(el);
}

void Grid::setEditMode(GridDisplayMode display_mode)
{
    dispmode = display_mode;

    redraw(true, true);
}

void Grid::setActivElement(Element* el)
{
    activeElem = el; 

    actnote = dynamic_cast<Note*>(el);

    if (actnote != previewNote && previewNote != NULL)
    {
        previewNote->releasePreview();

        previewNote = NULL;
    }
}

void Grid::setActionMode(GridActionMode md)
{
    mode = md;
}

void Grid::setPixelsPerTick(float ppt, int mouseRefX)
{
    pixpertick = ppt;

    framesPerPix = MTransp != NULL ? MTransp->getFramesPerTick()/pixpertick : 0;

    MInstrPanel->updateWaves();

    MEdit->playHead->updatePosFromFrame();

    float offs = hscr->getOffset();

    if(mouseRefX >= 0)
    {
        setHoffs(currTick - float(mouseRefX - getX1())/(float)pixpertick);
    }
    else
    {
        //setHoffs(patt->getPlayTick() - visibleTickSpan/2);
        setHoffs(getHoffs());
    }
}

void Grid::setLineHeight(int newLH)
{
    if(newLH != lheight)
    {
        lheight = newLH;
    }

    redraw(true, true);
}

void Grid::selectAll(bool select)
{
    selected.clear();

    for(Element* el : patt->getElems())
    {
        if(!el->isdel())
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

void Grid::selReset(bool deselect)
{
    selStartX = selStartY = -1;

    if(deselect)
    {
        selectAll(false);
    }

    sel->setCoords1(0, 0, 1, 1);
    sel->setVis(false);
}


void Grid::updBounds()
{
    lastElementEndTick = 0;
    bottomLine = 0;

    if (patt != NULL)
    {
        for(Element* el : patt->getBasePattern()->getElems())
        {
            if (!el->isdel())
            {
                if(el->getendtick() > lastElementEndTick)
                {
                    lastElementEndTick = el->getendtick();
                }

                if(el->getline() > bottomLine)
                {
                    bottomLine = el->getline();
                }
            }
        }
    }


    if(hscr)
    {
        float visiblepart = (float)(width)/getPixelsPerTick();

        float full = lastElementEndTick + (visiblepart*0.9f);

        if (full < (float)width/getPixelsPerTick())
        {
            full = (1.1f*width)/getPixelsPerTick();
        }

        hscr->updBounds(full, visiblepart, getHoffs());
    }

    if(vscr)
    {
        float full = (1 + bottomLine + 5)*lheight;

        if (full < 1.1f*height)
        {
            full = 1.1f*height;
        }

        vscr->updBounds(full, height, getVoffs());
    }
}

void Grid::updElementsImage()
{
    if (width > 0 && height > 0)
    {
        if (elemImage != NULL)
        {
            delete elemImage;
        }

        elemImage = new juce::Image(Image::RGB, width, height, true);

        Graphics image(*(elemImage));

        drawElements(image);
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

void Grid::updatePosition(InputEvent & ev)
{
    getPosFromCoords(ev.mouseX, ev.mouseY, &alignTick, &alignLine);

    currTick = getTickFromX(ev.mouseX);

    if (ev.keyFlags & kbd_alt)
    {
        setActionMode(GridMode_Alt);
    }
    else if (ev.keyFlags & kbd_shift)
    {
        setActionMode(GridMode_Shift);
    }
    else if (ev.keyFlags & kbd_ctrl)
    {
        setActionMode(GridMode_Ctrl);
    }
    else
    {
        //selReset(false);

        setActionMode(GridMode_Default);
    }

    checkElementsAtPos(ev);
}

void Grid::updTransport()
{
    framesPerPix = MTransp != NULL ? MTransp->getFramesPerTick()/pixpertick : 0;

    MInstrPanel->updateWaves();

    for(Element* el : patt->getBasePattern()->getElems())
    {
        if (!el->isdel())
        {
            el->recalc();

            el->calcForGrid(this);
        }
    }

    patt->updateEvents();

    redraw(true, true);
}

void Grid::updFillerImage()
{
    int imgHeight = lheight; //*20;
    int barWidth = RoundFloat(MTransp->getTicksPerBar()*getPixelsPerTick());
    int imgWidth = barWidth;

    if(brushImage != NULL)
    {
        delete brushImage;
    }

    brushImage = new Image(Image::RGB, imgWidth, imgHeight, true);

    Graphics imageContext(*(brushImage));

    gSetMonoColor(imageContext, .15f, 1);

    imageContext.fillAll();

    float tick = .2f;
    float beat = .28f;
    float bar =  .36f;

    float divClr = .05f;

    // ticks
    gSetMonoColor(imageContext, tick);

    if(pixpertick > 1.5f)
    {
        for (int x = 0; x < imgWidth; x += pixpertick)
        {
            gFillRectWH(imageContext, x, 0, 1, lheight);
        }
    }
    else
    {
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

    if (dispmode != GridDisplayMode_Pans)
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

void Grid::updBufferImage()
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

        int xoffs = RoundFloat((getHoffs() / tickPerBar - (int)getHoffs() / tickPerBar) * getPixelsPerTick() * tickPerBar);

        int yoffs = int(vscr->getOffset()) % lheight;

        ImageBrush* imgBrush = new ImageBrush(brushImage, -xoffs, -yoffs, 1);

        image.setBrush(imgBrush);

        image.fillRect(0, 0, width, height);

        //drawIntermittentHighlight(image, 0, 0, width, height, 4);

        delete imgBrush;
    }
}

