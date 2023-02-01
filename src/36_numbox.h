

#pragma once


#include "36_globals.h"
#include "36_controls.h"
#include "36_config.h"
#include "36_scroller.h"




class NumBox : public Control
{
public:

            NumBox();

protected:

            int                 count;
            FontId              fontId;
            int                 xOld;
            int                 xDrag;
            int                 yOld;
            int                 ySet;
            float               value;

            void                drawSelf(Graphics& g);
    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleNumDrag(int count) {}
    virtual void                updAfterDrag() {}
};


class BpmBox : public NumBox
{
public:

            BpmBox(float val);

protected:

            void                drawSelf(Graphics& g);
            void                handleNumDrag(int count);
            void                handleMouseWheel(InputEvent& ev);
            void                updAfterDrag();
};

class MeterBox : public NumBox
{
public:

        MeterBox(int tpb, int bpb);

protected:

            int                 bpbVal;
            int                 tpbVal;

            void                drawSelf(Graphics& g);
            void                handleNumDrag(int count);
            void                handleMouseWheel(InputEvent& ev);
            void                updAfterDrag();
};


class OctaveBox : public NumBox
{
public:

            OctaveBox(int val);

            int                 getValue() { return value; }

protected:

            bool                dragging;
            int                 xOld;
            int                 ySet;
            int                 yOld;
            int                 value;

            void                drawSelf(Graphics& g);
            void                handleMouseWheel(InputEvent & ev);
            void                handleNumDrag(int count);
};



class TimeScreen : public NumBox
{
friend ControlPanel;
friend Audio36;

public:

        TimeScreen();

protected:

            ChanVU* vu;

            void                drawSelf(Graphics& g);
            void                handleNumDrag(int count);
            void                handleMouseWheel(InputEvent& ev);
};


class ParamBox : public NumBox
{
public:
            ParamBox(Parameter* param);
            std::string         getClickHint();
            float               getMinStep();
            void                setSliderOnly(bool so) { sliderOnly = so; }

protected:
            int                 defPos;
            int                 th;
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


class ToggleBox : public NumBox
{
public:
            ToggleBox(ParamToggle* param_tg);

protected:

            ParamToggle*        prmToggle;

            void                drawSelf(Graphics& g);
};

class RadioBox : public NumBox
{
public:
            RadioBox(ParamRadio* param_radio);

protected:

            ParamRadio*         prmRad;

            void                drawSelf(Graphics& g);
};


class SelectorBox : public NumBox
{
public:
            SelectorBox(ParamSelector* param_sel);

protected:

            ParamSelector*      prmSelector;

            void                drawSelf(Graphics& g);
};


class ListBoxx : public Scrolled
{
public:
            ListBoxx(std::string name);
            void SetList(std::list<std::string>  entries_list);

protected:

            std::list<std::string>  entriesList;

            void                drawSelf(Graphics& g);
};

