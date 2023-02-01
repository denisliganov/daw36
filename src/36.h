
#pragma once

//#define _WIN32_WINNT 0x0500

//Find out wich platform we build for

//#if (defined(_WIN32) || defined(_WIN64) || defined(_WIN32_WINNT))

#define       USE_WIN32 1


#include "36_objects.h"
#include "36_window.h"



typedef short   int16; 
typedef long    Tframe; 
typedef float   Tdata;


#define RELEASEBUILD FALSE


extern bool             InitComplete;





class MainWinObject : public WinObject
{
friend  ControlPanel;
friend  Browser;

    enum DragMode
    {
        DragMode_Default,
        DragMode_DragY1,
        DragMode_DragY2,
        DragMode_DragX0,
        DragMode_DragX1,
        DragMode_DragX2,
    };

public:

            MainWinObject();
            ~MainWinObject();

            void                setMainX1(int main_x1);
            void                setMainX2(int main_x2);
            void                setMainY1(int main_y1);
            void                setMainY2(int main_y2);
            void                switchBrowser(bool on);

protected:

            void                drawSelf(Graphics& g);
            DragMode            getMode() { return mode; }
            void                handleClose();
            void                handleChildEvent(Gobj * obj,InputEvent & ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleWindowResize(int wx, int wh);
            void                remap();
            void                setMode(DragMode m) { mode = m; }
            void                updActiveObject(InputEvent& ev);
            void                updCursorImage(unsigned flags);

            int                 mainY1;
            int                 mainY2;
            int                 mainX1;
            int                 mainX2;

            DragMode            mode;
            Gobj*               currAuxPanel;
            ScrollTimer*        scrollTimer;
};


