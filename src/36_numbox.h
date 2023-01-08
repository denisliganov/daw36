

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

            void    handleMouseDown(InputEvent& ev);
            void    handleMouseUp(InputEvent& ev);
            void    handleMouseDrag(InputEvent& ev);
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
            int th, tw1;
            int tw3;
            int tw4;
            int tx1;
            int tx2;
            int tx3;
            int tx4;

    virtual void    updValue();
            void    handleMouseWheel(InputEvent& ev);
            void    handleNumDrag(int dragCount);
            void    calcTextCoords();
            void    drawSelf(Graphics& g);

public:

            ParamBox(Parameter* param);
            void getTextCoords(int* txp1, int* txp2, int* txp3, int* txp4);
            void setTextCoords(int txs1, int txs2, int txs3, int txs4);
            void adjustTx2(int txmin2);
            void adjustTx3(int txmin3);
            void adjustTx4(int txmin4);
};


