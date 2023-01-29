
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

class Grid : public Scrolled
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

public:

            Grid(float step_width, int line_height, Pattern* pt, Timeline* tl);
            ~Grid() {}
            void                adjustscale(int delta, int mouseRefX = -1);
            void                action(GridAction act, float dTick = 0, int dLine = 0);
            void                delacross(int mx1, int my1, int mx2, int my2);
            void                editbars(InputEvent& ev);
            void                grabcursor(float tick, int line);
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
            int                 getLineHeight();
            Note*               getNoteAt(float tick, int line);
            GridDisplayMode     getDisplayMode() { return dispmode; }
            int                 getselnum();
    virtual void                handleModifierKeys(unsigned flags);
            bool                isselected(Element* el);
            Note*               putnote(float tick, int line, int noteVal);
            void                removeelem(Element* el);
            void                recalcElems();
            void                reassign();
            void                redraw(bool remap_objects, bool refresh_image = false);
            void                selall(bool select);
            void                setdispmode( GridDisplayMode display_mode );
            void                setactivelem(Element* el);
            void                setmode(GridActionMode md);
            void                setlineheight(int newLH);
            void                setppt(float tick_width, int mouseRefX = -1);
            void                selReset(bool deselect = true);
            void                updBounds();
            void                updtransport();

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
            void                drawSelf(Graphics & g);
            void                drawelems(Graphics& g);
            void                updcursor(InputEvent & ev);
            ContextMenu*        createContextMenu();
            void                activateMenuItem(std::string item);
            void                remap();
            void                mapElems();
            float               getticksnap(float val);
            void                getPos(int mx, int my, float* tick, int* line);
            void                checkElementsAtPos(InputEvent & ev);
            void                clickscroll(InputEvent& ev);
 };


