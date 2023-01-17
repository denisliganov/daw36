
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
    GridAction_Reset,
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


protected:

            GridDisplayMode     displayMode;
            GridActionMode      mode;
            GridAction          lastAction;
            bool                stepDefault;
            bool                wasSelecting;
            Selection*          sel;
            PlaceHighlight*     place;
            ResizeEdge          resizeEdge;
            Image*              mainImage;
            Image*              elemImage;
            Image*              brushImage;
            Timeline*           timeline;
            Scroller*           verticalScroller;
            Scroller*           gridScroller;
            Pattern*            patt;
            Element*            activeElem;
            Note*               activeNote;
            Note*               previewNote;
            InputEvent          lastEvent;
            InputEvent          newEvent;
            bool                mouseIsDown;
            int                 lineHeight;
            int                 actionLine;
            int                 alignLine;
            int                 dragLineStart;
            int                 vertOffset;
            int                 fullTracksHeight;
            int                 cursorLine;
            float               prevTick;
            float               alignTick;
            float               currTick;
            float               dragTickStart;
            float               tickOffset;
            float               cursorTick;
            float               snapSize;
            float               pixelsPerTick;
            float               framesPerPixel;
            float               brushWidthTicks;
            float               lastElementEndTick;
            float               lastElementStartTick;
            float               fullTickSpan;
            float               visibleTickSpan;
            float               visibleLineSpan;

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
            void                updatePosition(InputEvent & ev, bool textCursor = false);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool                handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void                refreshImageBrush();
            void                refreshImageBuffer();
            void                refreshElementsImage();
            void                drawIntermittentHighlight(Graphics& g, int xh, int yh, int wh, int hh, int numBars);
            bool                drawDraggedObject(Graphics& g, Gobj* obj);
            void                drawSelf(Graphics & g);
            void                drawElements(Graphics& g);
            void                updateMode(InputEvent & ev);
            void                updateCursorImage(InputEvent & ev);
            ContextMenu*        createContextMenu();
            void                activateContextMenuItem(std::string item);
            void                mapObjects();
            void                mapElements();
            float               getSnappedTick(float val);
            void                getAlignedPosFromCoords(int mx, int my, float* tick, int* line);
            void                checkActivePosition(InputEvent & ev);
            void                updateScrollers();
            void                adjustVisibleArea(InputEvent& ev);
 
public:

            Grid(float step_width, int line_height, Pattern* pt, Timeline* tl);
            ~Grid() {}
            void                grabTextCursor();
            void                syncToInstruments();
            void                updateBounds();
            void                removeElementFromLists(Element* el);
            void                setActiveElement(Element* el);
            void                setMode(GridActionMode md);
            void                setVertScroller(Scroller& scr)   { verticalScroller = &scr; }
            void                setHorizScroller(Scroller& scr)   { gridScroller = &scr; }
            void                setTickOffset(float tick_offset, bool from_nav_bar = false);
            void                setVerticalOffset(int vert_offset);
            void                setLineHeight(int newLH);
            void                changeScale(int delta, int mouseRefX = -1);
            void                setPixelsPerTick(float tick_width, int mouseRefX = -1);
            void                redraw(bool remap_objects, bool refresh_image = false);
            Note*               putNote(float tick, int line, int noteVal);
            void                selectDeselectAll(bool select);
            void                processVolsPans(InputEvent& ev);
            void                reassignAfterMove();
            void                doAction(GridAction act, float dTick = 0, int dLine = 0);
            Pattern*            getPattern();
            Element*            getActiveElement()  { return activeElem; };
            GridActionMode      getActionMode()   { return mode; };
            float               getFramesPerPixel() { return framesPerPixel; }
            float               getSnapSize()   { return snapSize; }
            int                 getXfromTick(float tick);
            float               getTickFromX(int x);
            int                 getYfromLine(int line);
            int                 getLineFromY(int y);
            float               getPixelsPerTick();
            float               getTickOffset();
            int                 getVertOffset();
            int                 getLineHeight();
            void                deleteElementsAcross(int mx1, int my1, int mx2, int my2);
            void                setDisplayMode( GridDisplayMode display_mode );
            GridDisplayMode     getDisplayMode() { return displayMode; }
            void                updateScale();
            void                handleTransportUpdate();
            void                resetSelection(bool deselect = true);
    virtual void                handleModifierKeys(unsigned flags);
            int                 getNumSelected();
            bool                isElementSelected(Element* el);
            void                updateChangedElements();
};


