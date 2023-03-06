
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

        Gobj*               targetObj;
        Gobj*               dragObj;
        DropHighlight*      dropHighlightHorizontal;
        DropHighlight*      dropHighlightVertical;
        DropHighlightRect*  dropRect;
        int                 count;

        bool            canDrag();
        void            drawSelf(Graphics & g);
        void            drag(Gobj* targetObj, int mx, int my);
        void            drop(int mx, int my, unsigned int flags);
        bool            isActive();
        void            reset();
        void            start(Gobj * drag_obj, int mx, int my);
};


