
#pragma once


#include <array>

#include "36_globals.h"
#include "36_objects.h"


class AuxKeys : public Gobj
{
protected:

            bool        isVertical;
            Grid*       grid;
            int         keyWidth;
            int         playNote;
            int         prevPlayedNote;

            std::array<bool, 120>   keyState;

            void        handleChildEvent(Gobj * obj, InputEvent& ev);
            void        handleMouseDown(InputEvent & ev);
            void        handleMouseUp(InputEvent & ev);
            void        handleMouseDrag(InputEvent & ev);
            void        drawVerticalKey(Graphics& g, bool vert, int coord1, int coord2, int length, int key, bool pressed);
            void        drawVerticalKeys(Graphics& g, int x, int y, int w, int h);
            void        drawHorizontalKeys(Graphics& g, int x, int y, int w, int h);
            void        drawself(Graphics& g);
            int         getNote(int mx, int my);

public:

            AuxKeys(Grid* gr, int key_width, bool vert);
};

