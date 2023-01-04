
#pragma once


#include "36_globals.h"

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
protected:

            ObjectGroup         objGroup;

            std::string         objId;
            std::string         objTitle;
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
            bool                mouseHovering;

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

public:
            // List of child objects
            std::list<Gobj*>    objs;

            Gobj();
    virtual ~Gobj();

      ObjectGroup               getObjGroup()       { return objGroup; };
    std::string                 getObjId()          { return objId; }
    std::string                 getObjTitle()       { return objTitle; }
    virtual std::string         getHint()           { return hint; };

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

            bool                isChanged()         { return changed; }
            bool                isEnabled()         {  return enabled; }
    virtual bool                isShown()           { return (enabled && visible); }
            bool                isMouseHovering()   { return mouseHovering; }
            Gobj*               getParent()         { return parent; }
            WinObject*          getWindow()         { return window; };
            bool                getRelative()       { return relativeToParent; };
            Gobj*               getLastTouchedObject(int mx, int my);
            void                remapAndRedraw();

            void                addObject(Gobj* o, ObjectGroup type);
            void                addObject(Gobj* o, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addObject(Gobj* o, int xr, int yr, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addObject(Gobj* o, int xr, int yr, int ww, int hh, std::string id = "", ObjectGroup type = ObjGroup_Default);
            void                addHighlight(Gobj* obj);
            void                deleteObject(Gobj* o);
            void                setParent(Gobj* parent);
            void                setHint(std::string h)              { hint = h; };
            void                setObjTitle(std::string title)      { objTitle = title; }
            void                setWindow(WinObject* w);
            void                setRelative(bool rel);
            void                setVisible(bool vis);
            void                setMouseHovering(bool hover)    { mouseHovering = hover; }
            bool                checkMouseHovering(int mx, int my);
            void                removeObject(Gobj* obj);

    virtual void                setEnable(bool en);

    virtual void                setCoords1(int xNew, int yNew, int wNew = -1, int hNew = -1);
    virtual void                setCoords2(int xNew, int yNew, int x1New, int y1New);
    virtual void                setDrawAreaDirectly(int xabs1, int xabs2, int xabs3, int xabs4);

    virtual void                updCoords();
    virtual void                mapObjects() {}
    virtual void                confine(int bxNew = -1, int byNew = -1, int bx1New = -1, int by1New = -1);
    virtual void                updDrawCoords();
    virtual bool                isMouseTouching(int mx, int my);
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
    virtual bool                drawDraggedObject(Graphics& g, Gobj* obj) { return false; };

    virtual void                redraw(bool change = true);
    virtual void                handleDraw(Graphics& g);
    virtual void                drawSelf(Graphics& g) {};
    virtual void                drawOverChildren(Graphics& g) {};

            // Drawing methods using relatice coords
            void                fillWithMonoColor(float mono_color);
            void                fillMe();
            void                setMonoColor(float mono_color);
            void                drawRect(int x, int y, int w, int h);
            void                fillRect(int x, int y, int w, int h);
            void                line(int lx, int ly, int lx2, int ly2);
            void                lineH(int ly, int lx1, int lx2);
            void                lineV(int lx, int ly1, int ly2);
    virtual ContextMenu*        createContextMenu() {return NULL;};
    virtual void                activateContextMenuItem(std::string item) {};
            void                deactivateDropBoxById(int dropbox_id);
    virtual void                setMyColor(Graphics& g, float brightness=-1.f, float saturation = -1.f, float alpha=1.f);
            void                deleteAllObjects();
            void                defineHueColor(float hue, float sat) { colorHue = hue; colorSat = sat; }
            void                defineMonoColor(float mono_level) { monoLevel = mono_level; }

};

// Util function used for drag'n'dropping objects

Gobj* CheckNeighborObjectsY(std::list<Gobj*> &lst, std::string oname, int my, Gobj** upper, Gobj** lower);
Gobj* CheckNeighborObjectsX(std::list<Gobj*> &lst, std::string oname, int mx, Gobj** lefter, Gobj** righter);

//}
