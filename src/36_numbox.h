

#pragma once


#include "36_globals.h"
#include "36_controls.h"
#include "36_config.h"




class NumBox : public Control
{
protected:

            int         ySet;
            int         xOld;
            int         yOld;
            int         xDrag;
            int         count;

            float       value;

    virtual void    handleMouseDown(InputEvent& ev);
    virtual void    handleMouseUp(InputEvent& ev);
    virtual void    handleMouseDrag(InputEvent& ev);
            void    drawSelf(Graphics& g);

    virtual void    handleNumDrag(int count) {}
    virtual void    updAfterDrag() {}

public:

        NumBox() {}
};


class BpmBox : public NumBox
{
protected:

            void    drawSelf(Graphics& g);
            void    handleNumDrag(int count);
            void    updAfterDrag();
            void    handleMouseWheel(InputEvent& ev);

public:

            BpmBox(float val);
};

class MeterBox : public NumBox
{
protected:

        int             tpbVal;
        int             bpbVal;


        void    drawSelf(Graphics& g);
        void    handleNumDrag(int count);
        void    updAfterDrag();
        void    handleMouseWheel(InputEvent& ev);

public:

        MeterBox(int tpb, int bpb);
};


class OctaveBox : public NumBox
{
protected:

        int         value;
        int         ySet;
        int         xOld;
        int         yOld;
        bool        dragging;

        void    handleMouseWheel(InputEvent & ev);
        void    handleNumDrag(int count);
        void    drawSelf(Graphics& g);

public:

        OctaveBox(int val);

        int     getValue() { return value; }
};



class TimeScreen : public NumBox
{
friend ControlPanel;
friend Audio36;

protected:

        ChanVU*             vu;

        void    drawSelf(Graphics& g);
        void    handleNumDrag(int count);
        void    handleMouseWheel(InputEvent& ev);

public:

        TimeScreen();
};


class ParamBox : public NumBox
{
friend Parameter;

protected:
            int th;
            int tx1;
            int tx2;
            int tx3;
            int tw1;
            int tw2;
            int tw3;

    virtual void    updValue();
            void    handleMouseWheel(InputEvent& ev);
            void    handleNumDrag(int dragCount);
            void    handleMouseDown(InputEvent & ev);
            void    handleMouseDrag(InputEvent & ev);
            void    handleMouseUp(InputEvent& ev);
            void    drawSelf(Graphics& g);
            void    redraw();

public:

            ParamBox(Parameter* param);
            float   getMinStep();
};


