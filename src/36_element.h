

#pragma once

#include "36_globals.h"
#include "36_objects.h"

#include <list>


typedef enum ElemType
{
    El_Pattern,
    El_SampleNote,
    El_GenNote,
    El_Envelope,
    EL_UNKNOWN = 0xFFFF
}ElemType;


class Element : public Gobj
{
public:

            int             trkLine;
            int             numLines;
            bool            deleted;
            bool            selected;
            long            startFrame;
            long            endFrame;
            long            frameLength;
            bool            highlighted;
            bool            calculated;
            float           oldTick;
            float           endTick;
            float           moveTick;
            float           startTick;
            float           tickLength;
            Pattern*        patt;
            Device36*       dev;
            ElemType        type;

            std::list<Trigger*> triggers;


            Element();
            //const Element& operator= (const Element&);
    virtual ~Element();
    virtual Element*        clone();
    virtual Element*        clone(Pattern* newPtBase);
    virtual void            move(float dtick, int dtrack);
    virtual void            propagateTriggers(Pattern* pt);
    virtual void            unpropagateTriggers(Pattern* pt);
    virtual void            handleMouseDown(InputEvent& ev);
    virtual void            recalculate();
    virtual bool            checkVisible(Grid* grid);
            bool            isPointed(int mx, int my, Grid* grid);
    virtual void            calcCoordsForGrid(Grid* grid);
    virtual void            save(XmlElement* xmlNode);
    virtual void            load(XmlElement* xmlNode);
    virtual void            preview(int key) {}
    virtual bool            isNote() {return false;}
            void            calcFrames();
            void            addTrigger(Trigger* tg);
            void            removeTrigger(Trigger* tg);
            void            deleteAllTriggers();
            void            deactivateAllTriggers();
            void            relocateTriggers();

            void            markSelected(bool sel);
    virtual void            markDeleted(bool del);
            bool            isDeleted();
            bool            isSelected();
            void            highlightOn();
            void            highlightOff();
            bool            isHighlighted();
    virtual bool            isShown();

            int             getLine();
    virtual void            setLine(int tkl);
    virtual void            setPos(float tk, int l);
    virtual void            setEndTick(float et);
    virtual void            setTickLength(float tl);
    virtual void            setTickDelta(float tick_delta);
    virtual long            getStartFrame();
    virtual long            getEndFrame();
    virtual long            getFrameLength();
    virtual float           getStartTick();
    virtual float           getEndTick();
    virtual float           getTickLength();
            int             getNumLines() { return numLines; }
            Pattern*        getBase();
    virtual void            drawOnGrid(Graphics& g, Grid* grid) {}
            void            softDelete();
};




