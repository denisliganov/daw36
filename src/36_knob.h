
#pragma once

#include "36_controls.h"



class Knob : public Control
{
public:

            Knob(Parameter* par, bool knob = false);
            float               getMinStep()    { return 1.f / width; }
            std::string         getClickHint();
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseUp(InputEvent & ev);
            void                handleSliding(InputEvent& ev);
            void                remap();
            void                setDimOnZero(bool dim) { dimOnZero = dim; }
            void                setHasText(bool txt, bool inside=false);
            void                updValue();
            void                updPosition();

protected:

    virtual void                drawKnob(Graphics& g);
            void                drawSelf(Graphics& g);
            void                drawSlider(Graphics& g);
            void                drawText(Graphics& g);

protected:

            float               angleRange;       // in radians too
            float               angleOffset;      // in radians too
            float               positionAngle;    // in radians
            float               widthDiv;
            int                 ys;
            int                 defPos;
            int                 savedHeight;
            Instr*              instr;
            bool                knobMode;
            bool                dim;
            bool                sliding;
            bool                dimOnZero;
            bool                hasText;
            bool                textInside;
};

