
#pragma once

#include "36_globals.h"
#include "36_objects.h"




typedef enum LaneType
{
    Lane_Vol = 1,
    Lane_Pan,
    Lane_Pitch,
    Lane_Keys,
    Lane_Usual
}LaneType;

class Lane : public Gobj
{
friend  LanePanel;

public:

        Lane(Grid* grid, LaneType ltype);

        LaneType    type;
        Grid*       grid;
        int         prevX;
        int         prevY;

        void        drawBar(Graphics& g, Element* el, int cx, bool sel, int divHgt);
        void        drawBars(Graphics& g);
        void        drawSelf(Graphics& g);
        void        handleMouseDrag(InputEvent& ev);
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseUp(InputEvent& ev);
        void        process(bool leftbt, int mx, int my, unsigned flags);
};

class LanePanel : public Gobj
{
public:

        LanePanel(Grid* grid);

        Grid*       workGrid;
        Lane*       lane;
        AuxKeys*    keys;
        LaneType    laneType;

        void        drawSelf(Graphics& g);
        void        remap();
        void        setType(LaneType ltype);
};


