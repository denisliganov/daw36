
#pragma once


#include "36_globals.h"
#include "36_objects.h"





class DropHighlight  : public Gobj
{
public:
        bool    vertical;

        DropHighlight(bool vert);

        void    drawSelf(Graphics& g);
};

class DragAndDrop  : public Gobj
{
public:

        DragAndDrop();

        Gobj*           targetObj;
        Gobj*           dragObj;
        DropHighlight*  dropHighlightHorizontal;
        DropHighlight*  dropHighlightVertical;
        int             count;

        void            start(Gobj * drag_obj, int mx, int my);
        void            reset();
        void            drawSelf(Graphics & g);
        bool            canDrag();
        bool            isActive();
        void            drag(Gobj* targetObj, int mx, int my);
        void            drop(int mx, int my, unsigned int flags);
        bool            isMouseTouching(int mx, int my) { return false; }     // can't touch this
};


