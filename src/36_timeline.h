
#pragma once

#include "36_globals.h"
#include "36_objects.h"



class Timeline : public Gobj
{
friend Grid;

protected:

        Grid*       grid;

        void        drawSelf(Graphics& g);
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseDrag(InputEvent& ev);
        void        handleMouseMove(InputEvent & ev);
        void        handleMouseWheel(InputEvent & ev);
        void        set(int mouse_x);
        void        drawIntermittentHighlight(Graphics& g, int x, int y, int w, int h, int numBars);

public:

        Timeline(Grid* g);
};

