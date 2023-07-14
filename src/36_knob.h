
#pragma once

#include "36_controls.h"
#include "36_params.h"






class ParamBox : public Control
{
public:
            ParamBox();
            void                setTextParams(bool txt, bool inside=false, bool value_replace=false, float width_divider = 0.45f);

protected:

            void                drawText(Graphics& g);
            int                 getMaxTextWidth();
            void                handleMouseEnter(InputEvent& ev) { redraw(); }
            void                handleMouseLeave(InputEvent& ev) { redraw(); }
            void                remap();

            bool                hasText;
            bool                textInside;
            bool                valueReplace;
            float               widthDivider;
            int                 controlWidth;
            int                 txtAreaX;
            int                 txtAreaWidth;
            int                 txtX;
            int                 txtY;
            int                 hoverOption;

            Instr*              instr;
            Eff*                eff;
};

class Knob : public ParamBox
{
public:

            Knob(Parameter* par, bool knob = false, bool vert = false);
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
            void                drawHorizontalSlider(Graphics& g);
            void                drawVerticalSlider(Graphics& g);
            ContextMenu*        createContextMenu();

protected:

            float               angleRange;       // in radians too
            float               angleOffset;      // in radians too
            float               positionAngle;    // in radians
            int                 ys;
            int                 defaultPos;
            int                 savedHeight;
            bool                knobMode;
            bool                dim;
            bool                sliding;
            bool                dimOnZero;
            bool                vertical;
};



class SelectorBox : public ParamBox
{
public:
            SelectorBox(Parameter* param_sel);

protected:

            void                drawSelf(Graphics& g);
    virtual void                handleMouseDown(InputEvent & ev);
            void                handleMouseDrag(InputEvent& ev);
            void                remap();
            ContextMenu*        createContextMenu();
            void                handleMouseMove(InputEvent& ev);

            int                 itemWidth;
            bool                radioMode;
};

class ToggleBox : public SelectorBox
{
public:
            ToggleBox(Parameter* prm) : SelectorBox(prm) { }
            bool                getBoolValue() { return param->getBoolValue(); }
};


