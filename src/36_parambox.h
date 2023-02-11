

#pragma once


#include "36_globals.h"
#include "36_controls.h"
#include "36_scroller.h"




class ParamBox : public Control
{
public:
            ParamBox(Parameter* param);
            std::string         getClickHint();
            float               getMinStep();
            void                setSliderOnly(bool so) { sliderOnly = so; }

protected:
            int                 defPos;
            int                 tw1;
            int                 tw2;
            int                 tw3;
            bool                sliderOnly;
            Parameter*          prm;

            void                drawSelf(Graphics& g);
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
            ToggleBox(ParamToggle* param_tg);

protected:

            ParamToggle*        prmToggle;

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
            void                handleMouseUp(InputEvent& ev);
};

class RadioBox : public Control
{
public:
            RadioBox(ParamRadio* param_radio);

protected:

            ParamRadio*         prmRad;

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
};


class SelectorBox : public Control
{
public:
            SelectorBox(ParamSelector* param_sel);

protected:

            ParamSelector*      prmSelector;

            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent & ev);
};



