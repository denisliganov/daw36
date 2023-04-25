
#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_window.h"






class DragAndDrop  : public Gobj
{
public:

        DragAndDrop();
        ~DragAndDrop();

        int                 count;
        Gobj*               dragObj;
        Gobj*               targetObj;
        SubWindow*          sw;

        bool                canDrag();
        void                drawSelf(Graphics & g);
        void                drag(Gobj* targetObj, int mx, int my);
        void                drop(int mx, int my, unsigned int flags);
        bool                isActive();
        void                start(Gobj * drag_obj, int mx, int my);
};


