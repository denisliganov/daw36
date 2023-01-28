
#pragma once

#include "36_controls.h"



class Knob : public Control
{
public:

            Knob(Parameter* par);
            void            updValue();
            void            updPosition();
            void            TweakByWheel(int delta, int mouse_x, int mouse_y);
            void            handleMouseDrag(InputEvent& ev);
            void            drawSelf(Graphics& g);
            void            handleMouseDown(InputEvent & ev);
            void            handleMouseUp(InputEvent & ev);
            void            remap();

protected:

            float           positionAngle;    // in radians
            float           angleRange;       // in radians too
            float           angleOffset;      // in radians too
            int             ys;
};

