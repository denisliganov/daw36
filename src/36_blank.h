
#pragma once

#include "36_globals.h"
#include "36_objects.h"


class Blank : public Gobj
{
protected:

        void    drawself(Graphics& g);
        void    handleMouseDown(InputEvent& ev);
        void    handleMouseUp(InputEvent& ev);
        void    handleMouseDrag(InputEvent& ev);
        void    handleMouseWheel(InputEvent& ev);
        void    handleChildEvent(Gobj * obj,InputEvent & ev);

public:

        Blank();
};

