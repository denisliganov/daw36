
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
            void                adjustScale(int delta, int mouseRefX = -1);
            void                action(GridAction act, float dTick = 0, int dLine = 0);
            void                changeBars(InputEvent& ev);
            void                deleteAcross(int mx1, int my1, int mx2, int my2);
            void                grabTextCursor(float tick, int line);
            Pattern*            getPattern();
            Element*            getActiveElement()  { return activeElem; };
            GridActionMode      getActionMode()   { return mode; };
            float               getFramesPerPixel() { return framesPerPix; }
            float               getSnapSize()   { return snap; }
            int                 getXfromTick(float tick);
            float               getTickFromX(int x);
            int                 getYfromLine(int line);
            int                 getLineFromY(int y);
            float               getPixelsPerTick();
            int                 getLineHeight();
            Note*               getNoteAtPos(float tick, int line);
            GridDisplayMode     getDisplayMode() { return dispmode; }
            int                 getNumSelectedElements();
    virtual void                handleModifierKeys(unsigned flags);
            bool                isElementSelected(Element* el);
            Note*               putNote(float tick, int line, int noteVal);
            void                removeElement(Element* el);
            void                recalcElements();
            void                reassignElements();
            void                redraw(bool remap_objects, bool refresh_image = false);
            void                selectAll(bool select);
            void                setEditMode( GridDisplayMode display_mode );
            void                setActivElement(Element* el);
            void                setActionMode(GridActionMode md);
            void                setLineHeight(int newLH);
            void                setPixelsPerTick(float tick_width, int mouseRefX = -1);
            void                selReset(bool deselect = true);
            void                updBounds();
            void                updTransport();

protected:

            Element*            activeElem;
            Note*               actnote;
            int                 actionLine;
            float               alignTick;
            int                 alignLine;
            int                 bottomLine;
            Image*              brushImage;
            float               brushWidthTicks;
            float               currTick;
            int                 dragLineStart;
            float               dragTickStart;
            GridDisplayMode     dispmode;
            Image*              elemImage;
            float               framesPerPix;
            InputEvent          lastEvent;
            float               lastElementEndTick;
            int                 lheight;
            GridAction          lastAction;
            bool                mouseIsDown;
            Image*              mainimg;
            GridActionMode      mode;
            InputEvent          newEvent;
            Pattern*            patt;
            int                 prevX;
            int                 prevY;
            Note*               previewNote;
            float               prevTick;
            float               pixpertick;
            PlaceHighlight*     place;
            ResizeEdge          resizeEdge;
            Selection*          sel;
            float               snap;
            int                 selStartX;
            int                 selStartY;
            float               selTickStart;
            float               selTickEnd;
            int                 selLineStart;
            int                 selLineEnd;
            Timeline*           timeline;
            bool                wasSelecting;

            std::list<Element*>     clipboard;
            std::list<Element*>     selected;
            std::list<Element*>     updateList;
            std::list<Element*>     visible;

            void                activateMenuItem(std::string item);
            void                checkElementsAtPos(InputEvent & ev);
            void                clickScroll(InputEvent& ev);
            ContextMenu*        createContextMenu();
            void                drawIntermittentHighlight(Graphics& g, int xh, int yh, int wh, int hh, int numBars);
            bool                drawDraggedObject(Graphics& g, Gobj* obj);
            void                drawSelf(Graphics & g);
            void                drawElements(Graphics& g);
            float               getSnap(float val);
            void                getPosFromCoords(int mx, int my, float* tick, int* line);
    virtual void                handleMouseDown(InputEvent& ev);
    virtual void                handleMouseWheel(InputEvent& ev);
    virtual void                handleMouseMove(InputEvent & ev);
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseUp(InputEvent& ev);
            void                handleMouseEnter(InputEvent & ev);
            void                handleMouseLeave(InputEvent & ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool                handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void                updatePosition(InputEvent & ev);
            void                updcursor(InputEvent & ev);
            void                updFillerImage();
            void                updBufferImage();
            void                updElementsImage();
            void                remap();
    virtual void                remapElements();
};


