
#pragma once

#include "36_controls.h"



class Knob : public Control
{
public:

            Knob(Parameter* par);
            void                drawKnob(Graphics& g);
            void                drawSelf(Graphics& g);
            void                drawSlider(Graphics& g);
            void                drawText(Graphics& g);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseUp(InputEvent & ev);
            void                remap();
            void                setVis(bool vis);
            void                updValue();
            void                updPosition();

protected:

            float               angleRange;       // in radians too
            float               angleOffset;      // in radians too
            float               positionAngle;    // in radians
            int                 ys;
            Instrument*         instr;
};

