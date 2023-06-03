
#pragma once

#include "36_controls.h"
#include "36_params.h"






class ParamBox : public Control
{
public:
            ParamBox();
            void                setHasText(bool txt, bool inside=false);

protected:

            void                drawText(Graphics& g);

            bool                hasText;
            bool                textInside;
            float               widthDiv;
};

class Knob : public ParamBox
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
            void                updValue();
            void                updPosition();

protected:

    virtual void                drawKnob(Graphics& g);
            void                drawSelf(Graphics& g);
            void                drawSlider(Graphics& g);
            ContextMenu*        createContextMenu();

protected:

            float               angleRange;       // in radians too
            float               angleOffset;      // in radians too
            float               positionAngle;    // in radians
            int                 ys;
            int                 defaultPos;
            int                 savedHeight;
            Instr*              instr;
            bool                knobMode;
            bool                dim;
            bool                sliding;
            bool                dimOnZero;
};



class SelectorBox : public ParamBox
{
public:
            SelectorBox(Parameter* param_sel, bool radio = false);

protected:

            void                drawSelf(Graphics& g);
    virtual void                handleMouseDown(InputEvent & ev);
            void                remap();
            ContextMenu*        createContextMenu();

            int                 itemWidth;
            bool                radioMode;
};

class ToggleBox : public SelectorBox
{
public:
            ToggleBox(Parameter* prm) : SelectorBox(prm) { }
            bool  getBoolValue() { return param->getBoolValue(); }
};


