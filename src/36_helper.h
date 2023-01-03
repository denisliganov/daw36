

#pragma once

#include "36_globals.h"
#include "36_draw.h"
#include "36_objects.h"



class HelperPanel : public Gobj
{
protected:

        Scroller*   vScroller;

        void        drawSelf(Graphics& g);

public:

        HelperPanel();
};



