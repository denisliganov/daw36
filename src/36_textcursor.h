
#pragma once

#include "36_globals.h"
#include "36_objects.h"



class TextCursor : public Gobj
{
friend  Grid;

protected:

        float       tick;
        int         line;
        Grid*       grid;

public:

        TextCursor();
        void    setPos(float newTick, int newLine);
        void    updPos();
        void    drawSelf(Graphics& g);
        float   getTick();
        int     getLine();
};


