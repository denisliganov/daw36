
#pragma once

#include "36_controls.h"



class Knob : public Control
{
public:

            Knob(Parameter* par, bool knob = false);
            void                drawKnob(Graphics& g);
            void                drawSelf(Graphics& g);
            void                drawSlider(Graphics& g);
            void                drawText(Graphics& g);
            float               getMinStep()    { return 1.f / width; }
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseUp(InputEvent & ev);
            void                handleSliding(InputEvent& ev);
            void                remap();
            void                setVis(bool vis);
            void                updValue();
            void                updPosition();

protected:

            float               angleRange;       // in radians too
            float               angleOffset;      // in radians too
            float               positionAngle;    // in radians
            int                 ys;
            int                 defPos;
            int                 savedHeight;
            Instrument*         instr;
            bool                knobMode;
};

