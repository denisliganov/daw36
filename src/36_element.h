

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

            Element();
    virtual ~Element();
            void                addTrigger(Trigger* tg);
    virtual Element*            clone();
    virtual Element*            clone(Pattern* newPtBase);
    virtual void                calcForGrid(Grid* grid);
            void                calcFrames();
    virtual bool                checkVisible(Grid* grid);
            void                deleteAllTriggers();
            void                deactivateAllTriggers();
    virtual void                drawOnGrid(Graphics& g, Grid* grid) {}
            Device36*           getDevice()     { return dev; }
    virtual long                getFrame();
    virtual long                getEndFrame();
    virtual long                getFrames();
    virtual float               getTick();
    virtual float               getEndTick();
    virtual float               getticklen();
            Pattern*            getbasepatt();
            int                 getLine();
            ElemType            getType()       { return type; }
            long                getFrameLength()    {return framelen;}
        std::list<Trigger*>&    getTriggers()       { return triggers; }
            Pattern*            getPattern()        { return patt; }
    virtual void                handleMouseDown(InputEvent& ev);
    virtual bool                isNote() {return false;}
            bool                isPointed(int mx, int my, Grid* grid);
            bool                isdel();
            bool                issel();
    virtual bool                isShown();
    virtual void                load(XmlElement* xmlNode);
    virtual void                move(float dtick, int dtrack);
            void                markSelected(bool sel);
    virtual void                markDeleted(bool del);
    virtual void                preview(int key) {}
    virtual void                propagateTriggers(Pattern* pt);
            void                removeTrigger(Trigger* tg);
            void                relocateTriggers();
    virtual void                recalc();
    virtual void                save(XmlElement* xmlNode);
    virtual void                setLine(int tkl);
    virtual void                setPos(float tk, int ln);
    virtual void                setEndTick(float et);
    virtual void                setTickLength(float tl);
    virtual void                setTickDelta(float tick_delta);
            void                softdel();
    virtual void                setPattern(Pattern* pt);
    virtual void                unpropagateTriggers(Pattern* pt);

protected:

            bool                calculated;
            bool                deleted;
            Device36*           dev;
            long                frame1;
            long                frame2;
            long                framelen;
            int                 line;
            int                 numlines;
            Pattern*            patt;
            bool                selected;
            float               tick1;
            float               tick2;
            float               ticklen;
            ElemType            type;

            std::list<Trigger*> triggers;
};




