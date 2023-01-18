
#include "36.h"
#include "36_edit.h"
#include "36_pattern.h"
#include "36_draw.h"
#include "36_ctrlpanel.h"
#include "36_grid.h"
#include "36_timeline.h"
#include "36_playhead.h"
#include "36_auxkeys.h"
#include "36_project.h"
#include "36_text.h"
#include "36_instrpanel.h"
#include "36_sampleinstr.h"
#include "36_history.h"
#include "36_note.h"
#include "36_transport.h"
#include "36_events_triggers.h"
#include "36_utils.h"
#include "36_params.h"
#include "36_snapmenu.h"
#include "36_keyboard.h"



MainEdit::MainEdit()
{
    MPattern = new Pattern(NULL, 0.f, -1.f, 0, 119, true);
    MPattern->ptBase = MPattern;
    MPattern->calcFrames();
    MPattern->addInstance(MPattern);

    addObject(timeline = new Timeline(grid));
    addObject(grid = new Grid(2, InstrHeight + 1, MPattern, timeline));

    addHighlight(playHead = new Playhead(grid));

    MGrid = grid;

    MProject.patternList.push_front(MPattern);
    MPattern->ptBase = MPattern;
    MPattern->setBounds(0, 2147483647);

    addObject(verticalGridScroller = new Scroller(true));

    grid->setVertScroller(*verticalGridScroller);
    grid->setHorizScroller(MCtrllPanel->getScroller());

    MTextCursor = new TextCursor();
}

void MainEdit::drawSelf(Graphics& g)
{
    //setc(g, 0.2f);
    //fillx(g, 0, 0, LineNumWidth, height);

    // Gap on the right
    setc(g, 0.3f);
    fillx(g, width - GridScrollWidth, 0, width, MainLineHeight - 1);
    setc(g, 0.4f);
    rectx(g, width - GridScrollWidth, 0, width, MainLineHeight - 1);


/*
    int lines = grid->visibleLineSpan;
    int offs = grid->vertOffset;
    int lH = grid->lineHeight;
    int max = grid->getPattern()->getNumLines() - 1;
    for(int line = 0; line < lines + 1; line++)
    {
        //String str = String::formatted(T("%03d"), line + offs);
        //gText(g, FontSmall, (std::string)str, x1 + 1, y + 13);

        setc(g, 0.0f);
        lineH(g, grid->getY() + lH*line - offs - 1, 0, LineNumWidth - 2);
    }*/

    setc(g, 0.35f);
    fillx(g, 0, height - BottomPadHeight, width, height);
    setc(g, 0.4f);
    lineH(g, height - BottomPadHeight, 0, width);
}

void MainEdit::mapObjects()
{
    confine();

    int yGrid = MainLineHeight - 1;

    timeline->setCoords2(0, 0, width - GridScrollWidth - 1, yGrid - 2);

    //keys->setCoords1(LeftGap, MainLineHeight, 100, height - MainLineHeight - 1);
    //keys1->setCoords2(LeftGap+100, height - 1 - 100, width - GridScrollWidth - 1, height - 1);

    verticalGridScroller->setCoords2(width - GridScrollWidth, MainLineHeight, width - 1, height - 1 - BottomPadHeight - 1);

    grid->setCoords2(0, yGrid, width - GridScrollWidth - 1, height - 1 - BottomPadHeight - 1);

    confine(0, 0, width - GridScrollWidth - 1, height - 1 - BottomPadHeight - 1);

    playHead->updatePosFromFrame();
}

void MainEdit::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if(obj == verticalGridScroller)
    {
        grid->handleChildEvent(verticalGridScroller, ev);

        //MInstrPanel->setOffset(int(verticalGridScroller->getOffset()));
    }
}

Pattern* Create_Pattern_Base(float tk1, float tk2, int tr1, int tr2, char* name, Pattern* ptBase)
{
    Pattern* ptMom = new Pattern(name, tk1, tk2, tr1, tr2, true);

    MProject.patternList.push_front(ptMom);

    //GetPatternNameImage(ptMom);

    ptMom->ptBase = ptMom;

    Pattern* ptSon = Create_Pattern_Instance(ptMom, tk1, tk2, tr1, tr2, ptBase);

    return ptSon;
}

Pattern* Create_Pattern_Instance(Pattern* ptMom, float tk1, float tk2, int tr1, int tr2, Pattern* ptBase)
{
    Pattern* ptSon = new Pattern(NULL, tk1, tk2, tr1, tr2, false);

    ptMom->addInstance(ptSon);
    ptBase->addElement(ptSon);

    return ptSon;
}

Note* _Create_Note(float tick, int line, Instrument* instr, int note_val, float length, float vol, float pan, Pattern* ptBase)
{
    Note* note = Create_Note(tick, line, instr, note_val, length, vol, pan, ptBase);

    std::list<Element*> elem;
    elem.push_back(note);

    MHistory->addNewAction(HistAction_AddGroup, elem);

    return note;
}

Note* Create_Note(float tick, int line, Instrument* instr, int note_val, float length, float vol, float pan, Pattern* ptBase)
{
    Note* newNote = NULL;
    Sample* sample = dynamic_cast<Sample*>(instr);

    if (sample != NULL)
    {
        newNote = new SampleNote((Sample*)instr, note_val);
    }
    else
    {
        newNote = new Note(instr, note_val);
    }

    newNote->setPos(tick, line);

    newNote->vol->setValue(vol);
    newNote->pan->setValue(pan);

    newNote->setTickLength(length);

    if(ptBase)
    {
        ptBase->addElement(newNote);
    }

    return newNote;
}

void Delete_Element(Element* el)
{
    delete el;
}


ScrollTimer::ScrollTimer()
{
    timerPeriodMs = 200;

    startTimer(timerPeriodMs);
}

void ScrollTimer::timerCallback()
{
    Grid* grid = MGrid;

    //grid->setTickOffset(grid->getTickOffset() + 1.f/grid->getPixelsPerTick());

    if(grid->getActionMode() == GridMode_Moving || 
        grid->getActionMode() == GridAction_Move || 
         grid->getActionMode() == GridMode_Selecting)
    {
        int range = 30;

        // Scroll deltas for x and y axes

        float xDelta = grid->visibleTickSpan*.05f;
        float yDelta = grid->getH()*.08f;

        bool moved = false;

        //if(abs(_MainObject->lastEvent.mouseX - grid->getX1()) <= range)
        if(MObject->lastEvent.mouseX <= grid->getX1())
        {
            grid->setTickOffset(grid->tickOffset - xDelta);
            moved = true;
        }

        //if(abs(_MainObject->lastEvent.mouseX - grid->getX2()) <= range)
        if(MObject->lastEvent.mouseX >= grid->getX2())
        {
            grid->setTickOffset(grid->tickOffset + xDelta);
            moved = true;
        }

        // if(abs(_MainObject->lastEvent.mouseY - grid->getY1()) <= range)
        if(MObject->lastEvent.mouseY <= grid->getY1())
        {
            grid->vscr->setOffset(grid->vscr->getOffset() - yDelta);
            moved = true;
        }

        //if(abs(_MainObject->lastEvent.mouseY - grid->getY2()) <= range)
        if(MObject->lastEvent.mouseY >= grid->getY2())
        {
           // grid->setVertOffset(grid->vertOffset + yDelta);
           // moved = true;
        }

    /*
        int line = alignLine - lineOffset;
        int varea = visibleLineSpan*fraction;
        int vdiff = visibleLineSpan - line;

        if(vdiff < varea)
            setLineOffset(lineOffset + varea);

        if(line < varea)
            setLineOffset(lineOffset - varea);
        */

        if(moved)
        {
            grid->handleMouseDrag(MObject->lastEvent);
        }
    }

    startTimer(timerPeriodMs);
}



