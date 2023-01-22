
#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_env.h"
#include "36_scroller.h"
#include "36_timeline.h"





typedef enum ResizeEdge
{
    Resize_Right,
    Resize_Left,
    Resize_Top,
    Resize_Bottom
}ResizeEdge;

typedef enum GridDisplayMode
{
    GridDisplayMode_Steps,
    GridDisplayMode_Bars,
    GridDisplayMode_Volumes,
    GridDisplayMode_Pans,
}GridDisplayMode;

typedef enum GridActionMode
{
    GridMode_Default,
    GridMode_Selecting,
    GridMode_Brushing,
    GridMode_Moving,
    GridMode_Cloning,
    GridMode_Shift,
    GridMode_Ctrl,
    GridMode_Alt,
    GridMode_Deleting,
    GridMode_ElemResizing,
    GridMode_VolPanning,
}GridActionMode;

typedef enum GridAction
{
    GridAction_None,
    GridAction_PutNote,
    GridAction_Copy,
    GridAction_Cut,
    GridAction_Paste,
    GridAction_Move,
    GridAction_Clone,
    GridAction_Brush,
    GridAction_Delete,
    GridAction_Resize,
    GridAction_Resize2,
    GridAction_SelectRectangle,
    GridAction_VolPanChange,
    GridAction_WheelVolume,
    GridAction_Release,
}GridAction;

class Selection;
class PlaceHighlight;

class Grid : public Gobj
{
friend  Selection;
friend  Lane;
friend  Timeline;
friend  MainEdit;
friend  AuxKeys;
friend  EditHistory;
friend  Envelope;
friend  ScrollTimer;
friend  KeyHandler;
friend  MainWinObject;
friend  PlaceHighlight;
friend  TextCursor;
friend  ControlPanel;


protected:

            GridDisplayMode     dispmode;
            GridActionMode      mode;
            GridAction          lastAction;
            bool                wasSelecting;
            Selection*          sel;
            PlaceHighlight*     place;
            ResizeEdge          resizeEdge;
            Image*              mainimg;
            Image*              elemImage;
            Image*              brushImage;
            Timeline*           timeline;
            Scroller*           vscr;
            Scroller*           hscr;
            Pattern*            patt;
            Element*            activeElem;
            Note*               actnote;
            Note*               previewNote;
            InputEvent          lastEvent;
            InputEvent          newEvent;
            bool                mouseIsDown;
            int                 lheight;
            int                 actionLine;
            int                 alignLine;
            int                 dragLineStart;
            float               prevTick;
            float               alignTick;
            float               currTick;
            float               dragTickStart;
            float               snap;
            float               pixpertick;
            float               framesperpix;
            float               brushWidthTicks;

            float               lastElementEndTick;
            int                 bottomLine;

            int                 selStartX;
            int                 selStartY;
            float               selTickStart;
            float               selTickEnd;
            int                 selLineStart;
            int                 selLineEnd;
            int                 prevX;
            int                 prevY;

            std::list<Element*>     visible;
            std::list<Element*>     selected;
            std::list<Element*>     clipboard;
            std::list<Element*>     updateList;

    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseWheel(InputEvent& ev);
    virtual void                handleMouseMove(InputEvent & ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
            void                handleMouseEnter(InputEvent & ev);
            void                handleMouseLeave(InputEvent & ev);
            void                updpos(InputEvent & ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool                handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void                updfillerimage();
            void                updbuffimage();
            void                updelimage();
            void                drawintermittent(Graphics& g, int xh, int yh, int wh, int hh, int numBars);
            bool                drawDraggedObject(Graphics& g, Gobj* obj);
            void                drawself(Graphics & g);
            void                drawelems(Graphics& g);
            void                updcursor(InputEvent & ev);
            ContextMenu*        createmenu();
            void                activatemenuitem(std::string item);
            void                remap();
            void                mapElems();
            float               getticksnap(float val);
            void                getpos(int mx, int my, float* tick, int* line);
            void                checkElementsAtPos(InputEvent & ev);
            void                clickscroll(InputEvent& ev);
 
public:

            Grid(float step_width, int line_height, Pattern* pt, Timeline* tl);
            ~Grid() {}
            void                grabcursor(float tick, int line);
            void                updBounds();
            void                removeelem(Element* el);
            void                setactivelem(Element* el);
            void                setmode(GridActionMode md);
            void                setvscr(Scroller& scr)   { vscr = &scr; }
            void                sethscr(Scroller& scr)   { hscr = &scr; }
            void                sethoffs(float tick_offset, bool from_nav_bar = false);
            void                setlineheight(int newLH);
            void                adjustscale(int delta, int mouseRefX = -1);
            void                setppt(float tick_width, int mouseRefX = -1);
            void                redraw(bool remap_objects, bool refresh_image = false);
            Note*               putnote(float tick, int line, int noteVal);
            void                selall(bool select);
            void                editbars(InputEvent& ev);
            void                reassign();
            void                action(GridAction act, float dTick = 0, int dLine = 0);
            Pattern*            getpatt();
            Element*            getActiveElement()  { return activeElem; };
            GridActionMode      getActionMode()   { return mode; };
            float               getFramesPerPixel() { return framesperpix; }
            float               getSnapSize()   { return snap; }
            int                 getXfromTick(float tick);
            float               getTickFromX(int x);
            int                 getYfromLine(int line);
            int                 getLineFromY(int y);
            float               getppt();
            float               getTickOffset();
            int                 getlh();
            void                delacross(int mx1, int my1, int mx2, int my2);
            void                setdispmode( GridDisplayMode display_mode );
            GridDisplayMode     getdispmode() { return dispmode; }
            void                updtransport();
            void                selReset(bool deselect = true);
    virtual void                handleModifierKeys(unsigned flags);
            int                 getselnum();
            bool                isselected(Element* el);
            void                recalcElems();
};


