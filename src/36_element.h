

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

            int             line;
            int             numlines;
            bool            deleted;
            bool            selected;
            long            frame1;
            long            frame2;
            long            framelen;
            bool            calculated;
            float           tick1;
            float           tick2;
            float           ticklen;
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
    virtual void            recalc();
    virtual bool            checkVisible(Grid* grid);
            bool            isPointed(int mx, int my, Grid* grid);
    virtual void            calcForGrid(Grid* grid);
    virtual void            save(XmlElement* xmlNode);
    virtual void            load(XmlElement* xmlNode);
    virtual void            preview(int key) {}
    virtual bool            isNote() {return false;}
            void            calcframes();
            void            addTrigger(Trigger* tg);
            void            removeTrigger(Trigger* tg);
            void            deleteAllTriggers();
            void            deactivateAllTriggers();
            void            relocateTriggers();

            void            marksel(bool sel);
    virtual void            markdel(bool del);
            bool            isdel();
            bool            issel();
    virtual bool            isshown();

            int             getline();
    virtual void            setline(int tkl);
    virtual void            setPos(float tk, int ln);
    virtual void            setendtick(float et);
    virtual void            setticklen(float tl);
    virtual void            settickdelta(float tick_delta);
    virtual long            getframe();
    virtual long            getendframe();
    virtual long            getframes();
    virtual float           gettick();
    virtual float           getendtick();
    virtual float           getticklen();
            Pattern*        getbasepatt();
    virtual void            drawOnGrid(Graphics& g, Grid* grid) {}
            void            softdel();
};




