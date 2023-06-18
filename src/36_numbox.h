

#pragma once


#include "36_globals.h"
#include "36_controls.h"
#include "36_project.h"
#include "36_scroller.h"




class NumBox : public Control
{
public:

            NumBox();

protected:

            int                 count;
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


