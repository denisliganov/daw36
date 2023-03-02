
#pragma once


#include "36_globals.h"
#include "36_text.h"

#include "juce_amalgamated.h"

#include <list>
#include <string>


//namespace M {

typedef enum ObjectGroup
{
    ObjGroup_Default,
    ObjGroup_Highlight,
    ObjGroup_VU,
}ObjectGroup;


typedef struct InputEvent
{
        int             mouseX;
        int             mouseY;
        int             distance;
        bool            leftClick;
        bool            rightClick;
        bool            doubleClick;
        bool            clickDown;
        unsigned int    keyFlags;
        int             wheelDelta;
}InputEvent;


class Rect
{
public:

    int x;
    int y;
    int w;
    int h;

    Rect(int xx, int yy, int ww, int hh) {x = xx; y = yy; w = ww; h = hh;}

    bool    isValid() { return (w != 0 && h != 0); };
};


class Gobj
{
public:
            // List of child objects
            std::list<Gobj*>    objs;

            Gobj();
    virtual ~Gobj();

            void                addObject(Gobj* o, ObjectGroup type);
            void                addObject(Gobj* o, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addObject(Gobj* o, int xr, int yr, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addObject(Gobj* o, int xr, int yr, int ww, int hh, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addHighlight(Gobj* obj);
    virtual void                activateMenuItem(std::string item) {};
    virtual ContextMenu*        createContextMenu() {return NULL;};
    virtual void                confine(int bxNew = -1, int byNew = -1, int bx1New = -1, int by1New = -1);
    virtual bool                checkMouseTouching(int mx, int my);
            void                deactivateDropBoxById(int dropbox_id);
            void                deleteObject(Gobj* o);
            void                deleteAllObjects();
    virtual void                drawloop(Graphics& g);
    virtual void                drawSelf(Graphics& g) {};
    virtual void                drawOverChildren(Graphics& g) {};
    virtual bool                drawDraggedObject(Graphics& g, Gobj* obj) { return false; };
            void                defineHueColor(float hue, float sat) { colorHue = hue; colorSat = sat; }
            void                defineMonoColor(float mono_level) { monoLevel = mono_level; }
      ObjectGroup               getObjGroup()                   { return objGroup; };
    std::string                 getObjId()                      { return objId; }
    std::string                 getObjName()                       { return objName; }
    virtual std::string         getHint()           { return hint; };
    virtual std::string         getClickHint()       { return ""; };
            Gobj*               getParent()         { return parent; }
            WinObject*          getWindow()         { return window; };
            int                 getX()              { return xRel; };
            int                 getY()              { return yRel; };
    virtual int                 getW()              { return width; };
    virtual int                 getH()              { return height; };
            int                 getX1()             { return x1; };
            int                 getY1()             { return y1; };
            int                 getX2()             { return x2; };
            int                 getY2()             { return y2; };
            int                 getDrawX1()         { return dx1; };
            int                 getDrawY1()         { return dy1; };
            int                 getDrawX2()         { return dx2; };
            int                 getDrawY2()         { return dy2; };
            int                 getDrawWidth()      { return dwidth; };
            int                 getDrawHeight()     { return dheight; };
            Gobj*               getLastTouchedObject(int mx, int my);

    virtual void                handleMouseMove(InputEvent& ev) {}
    virtual void                handleMouseWheel(InputEvent& ev) {};
    virtual void                handleMouseDrag(InputEvent& ev) {};
    virtual void                handleMouseDown(InputEvent& ev) {};
    virtual void                handleMouseUp(InputEvent& ev) {};
    virtual void                handleChildEvent(Gobj* obj, InputEvent& ev) {};
    virtual void                handleMouseEnter(InputEvent& ev) {};
    virtual void                handleMouseLeave(InputEvent& ev) {};
    virtual bool                handleObjDrag(DragAndDrop& drag, Gobj* obj, int mx, int my);
    virtual bool                handleObjDrop(Gobj* obj, int mx, int my, unsigned int flags);

            bool                isChanged()         { return changed; }
            bool                isON()              {  return enabled; }
    virtual bool                isShown()           { return (enabled && visible); }
            bool                isUnderMouse()      { return undermouse; }

    virtual void                remap() {}
            void                remapAndRedraw();
            void                removeObject(Gobj* obj);
    virtual void                redraw(bool change = true);

            void                setParent(Gobj* parent);
            void                setWindow(WinObject* w);
            void                setVis(bool vis);
            void                setTouchable(bool tch);
            void                setObjName(std::string title)      { objName = title; }
    virtual void                setEnable(bool en);
            void                setHint(std::string h)              { hint = h; };
    virtual void                setCoords1(int xNew, int yNew, int wNew = -1, int hNew = -1);
    virtual void                setCoords2(int xNew, int yNew, int x1New, int y1New);
            void                setCoordsAbs(int ax1, int ay1, int ax2, int ay2);
            void                setCoordsUn(int ax1, int ay1, int ax2, int ay2);
    virtual void                setDrawAreaDirectly(int xabs1, int xabs2, int xabs3, int xabs4);
            void                setWH(int w, int h);
    virtual void                setMyColor(Graphics& g, float brightness=-1.f, float saturation = -1.f, float alpha=1.f);
            void                setUnderMouse(bool hover);


            void                fill(Graphics& g, float clr, float alpha=1);
            void                fill(Graphics& g, uint32 clr, float b, float a=1);
            void                fill(Graphics& g, uint32 clr);
            void                fillx(Graphics& g,int x, int y, int w, int h);
            void                rect(Graphics& g, float clr, float alpha=1);
            void                rect(Graphics& g, uint32 clr, float b, float a=1);
            void                rect(Graphics& g, uint32 clr);
            void                rectx(Graphics& g, int x, int y, int w, int h);
            void                lineH(Graphics& g, int ly, int lx1, int lx2);
            void                lineV(Graphics& g, int lx, int ly1, int ly2);
            void                setc(Graphics& g, float c, float a=1.f);
            void                setc(Graphics& g, uint32 clr, float b, float);
            void                setc(Graphics& g, uint32 clr);
            int                 txt(Graphics& g, FontId fontId, std::string str, int x, int y);
            void                txtfit(Graphics& g, FontId fontId, std::string str, int x, int y, int maxwidth);

    // context menu suppott

    virtual void                updCoords();

protected:

            ObjectGroup         objGroup;

            std::string         objId;
            std::string         objName;
            std::string         hint;

            float               monoLevel;
            float               colorHue;
            float               colorSat;

            Gobj*               parent;
            WinObject*          window;
            bool                visible;
            bool                enabled;
            bool                changed;
            bool                relativeToParent;
            bool                autoMapped;
            bool                undermouse;
            bool                touchable;

            // Relative coords, width, height
            int                 xRel;
            int                 yRel;
            int                 width;
            int                 height;

            // Absolute coords
            int                 x1;
            int                 y1;
            int                 x2;
            int                 y2;

            // Absolute drawing coords, width, height
            int                 dx1;
            int                 dy1;
            int                 dx2;
            int                 dy2;
            int                 dwidth;
            int                 dheight;

            // Confinement rectangle coords within object
            int                 bx1;
            int                 by1;
            int                 bx2;
            int                 by2;

            // Graphic context
            Graphics*           gr;
};

// Util function used for drag'n'dropping objects

Gobj* CheckNeighborObjectsY(std::list<Gobj*> &lst, std::string oname, int my, Gobj** upper, Gobj** lower);
Gobj* CheckNeighborObjectsX(std::list<Gobj*> &lst, std::string oname, int mx, Gobj** lefter, Gobj** righter);

//}
