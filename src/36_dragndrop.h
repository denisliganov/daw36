
#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_draw.h"




class DropHighlight  : public Gobj
{
public:
        DropHighlight()  { rect = false; }

        void                drawSelf(Graphics& g);
        void                setRectType(bool r) { rect = r; }

protected:
        bool                rect;
};

class DropHighlightRect  : public Gobj
{
public:

        DropHighlightRect() {}

        void    drawSelf(Graphics& g);
};

class DragAndDrop  : public Gobj
{
public:

        DragAndDrop();

        int                 count;
        Gobj*               dragObj;
        DropHighlight*      dropHighlight;
        DropHighlightRect*  dropRect;
        Gobj*               targetObj;

        bool                canDrag();
        void                drawSelf(Graphics & g);
        void                drag(Gobj* targetObj, int mx, int my);
        void                drop(int mx, int my, unsigned int flags);
        bool                isActive();
        void                reset();
        void                start(Gobj * drag_obj, int mx, int my);
        void                setDropCoords(int x, int y, int w, int h, bool rect = false);
};


