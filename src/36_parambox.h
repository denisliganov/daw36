

#pragma once


#include "36_globals.h"
#include "36_controls.h"
#include "36_scroller.h"




class ParamBox : public Control
{
public:
            ParamBox(Parameter* par);
            std::string         getClickHint();
            float               getMinStep();
            void                setSliderOnly(bool so) { sliderOnly = so; }

protected:
            int                 defPos;
            int                 tw1;
            int                 tw2;
            int                 tw3;
            bool                sliderOnly;

            void                drawSelf(Graphics& g);
            void                drawSlider(Graphics& g);
            void                drawText(Graphics& g);
            void                handleMouseWheel(InputEvent& ev);
            void                handleNumDrag(int dragCount);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseDrag(InputEvent & ev);
            void                handleMouseUp(InputEvent& ev);
            void                remap();
            void                redraw();
};


class ToggleBox : public Control
{
public:
            ToggleBox(Parameter* param_tg);

protected:

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseUp(InputEvent& ev);
};

class RadioBox : public Control
{
public:
            RadioBox(Parameter* param_radio);

protected:

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
};


class SelectorBox : public Control
{
public:
            SelectorBox(Parameter* param_sel);

protected:

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
};



