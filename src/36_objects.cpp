

#include "36_objects.h"
#include "36.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_vu.h"
#include "36_dropbox.h"



Gobj::Gobj()
{
    enabled = false;
    visible = false;
    changed = false;
    mouseHovering = false;
    relativeToParent = true;
    autoMapped = false;

    objGroup = ObjGroup_Default;

    parent = NULL;
    window = NULL;
    gr = NULL;

    monoLevel = -1;
    colorHue = -1;
    colorSat = -1;

    xRel = yRel = 0;
    x1 = y1 = x2 = y2 = width = height = 0;
    dx1 = dy1 = dx2 = dy2 = dwidth = dheight = 0;
    bx1 = by1 = bx2 = by2 = -1;
}

Gobj::~Gobj()
{
    if(window)
    {
        window->unregisterObject(this);
    }

    deleteAllObjects();
}

void Gobj::deleteAllObjects()
{
    while(objs.size() > 0)
    {
        deleteObject(objs.front());
    }
}

void Gobj::setWindow(WinObject* w)
{
    if(window)
    {
        window->unregisterObject(this);
    }

    window = w;

    if(window)
    {
        window->registerObject(this);
    }

    for(Gobj* obj : objs)
    {
        obj->setWindow(window);
    }
}

void Gobj::setParent(Gobj* par)
{
    if(parent != NULL)
    {
        parent->removeObject(this);
    }

    parent = par;

    parent->objs.push_back(this);

    setWindow(parent->getWindow());
}

void Gobj::setRelative(bool rel)
{
    relativeToParent = rel;
}


void Gobj::addObject(Gobj* obj, std::string id, ObjectGroup type)
{
    obj->objGroup = type;
    obj->setParent(this);

    if(id.size() > 0)
    {
        obj->objId = id;
    }

    obj->setEnable(enabled);
}

void Gobj::addObject(Gobj* obj, ObjectGroup type)
{
    addObject(obj, obj->getObjId(), type);
}

void Gobj::addObject(Gobj* obj, int xr, int yr, std::string id, ObjectGroup type)
{
    addObject(obj, id, type);

    obj->objId = id;
    obj->setCoords1(xr, yr);

    obj->autoMapped = true;
}

void Gobj::addObject(Gobj* obj, int xr, int yr, int ww, int hh, std::string id, ObjectGroup type)
{
    addObject(obj, id, type);

    obj->objId = id;
    obj->setCoords1(xr, yr, ww, hh);

    obj->autoMapped = true;
}

void Gobj::addHighlight(Gobj* obj)
{
    addObject(obj, obj->getObjId(), ObjGroup_Highlight);
}

void Gobj::deleteObject(Gobj* obj)
{
    objs.remove(obj);

    delete obj;
}

void Gobj::removeObject(Gobj* obj)
{
    objs.remove(obj);
}

// Recusively look for the top object touched by mouse
//
Gobj* Gobj::getLastTouchedObject(int mx, int my)
{
    for(Gobj* obj : objs)
    {
        if(obj->isMouseTouching(mx, my))
        {
            return obj->getLastTouchedObject(mx, my);
        }
    }

    return this;
}

void Gobj::setEnable(bool en)
{
    enabled = en;

    for(Gobj* obj : objs)
    {
        obj->setEnable(en);
    }
}

void Gobj::setVisible(bool vis)
{
    if(window)
    {
        window->addRepaint(dx1, dy1, dwidth, dheight);
    }

    visible = vis;

    if(visible == false)
    {
        // Force all child objs invisible

        for(Gobj* obj : objs)
        {
            obj->setVisible(false);
        }
    }
}

void Gobj::setDrawAreaDirectly(int sx1, int sy1, int sx2, int sy2)
{
    dx1 = x1;
    dy1 = y1;
    dx2 = x2;
    dy2 = y2;

    dwidth = sx2 - sx1 + 1;
    dheight = sy2 - sy1 + 1;

    if(window)
    {
        window->addRepaint(dx1, dy1, dwidth, dheight); // window->listen->repaint(dx1, dy1, dwidth, dheight);
    }
}

// use x, y, width, height

void Gobj::setCoords1(int xnew, int ynew, int wnew, int hnew)
{
    if(window)
    {
        // Erase me at the old coords
        window->addRepaint(dx1, dy1, dwidth, dheight);
    }

    xRel = xnew;
    yRel = ynew;

    if (wnew >= 0)
        width = wnew;

    if (hnew >= 0)
        height = hnew;

    updCoords();
}

// use x, y, x2, y2

void Gobj::setCoords2(int x, int y, int xx, int yy)
{
    setCoords1(x, y, xx - x + 1, yy - y + 1);
}

void Gobj::updCoords()
{
    x1 = (relativeToParent && parent != NULL) ? parent->x1 : 0;
    y1 = (relativeToParent && parent != NULL) ? parent->y1 : 0;
    x1 += xRel; 
    y1 += yRel;
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;

    updDrawCoords();

    if(visible)
    {
        // Reset draw bounds & draw kids

        confine();

        for(Gobj* obj : objs)
        {
            if(obj->autoMapped)
            {
                obj->updCoords();
            }
        }

        mapObjects();
    }
}

void Gobj::updDrawCoords()
{
    width = getW();
    height = getH();

    int cx1 = xRel, cx2 = xRel + width - 1, cy1 = yRel, cy2 = yRel + height - 1;

    if(relativeToParent && parent != NULL)
    {
        // Override bounds with those of parent's, if they overlap

        if (parent->bx1 > cx1) 
        {
            cx1 = (parent->bx1);
        }

        if (parent->by1 > cy1) 
        {
            cy1 = (parent->by1);
        }

        if (parent->bx2 < cx2) 
        {
            cx2 = (parent->bx2);
        }

        if (parent->by2 < cy2) 
        {
            cy2 = (parent->by2);
        }

        // Make coords absolute

        cx1 += parent->x1;
        cy1 += parent->y1;
        cx2 += parent->x1;
        cy2 += parent->y1;

        // If parent had their draw coords cut, adjust ours as well

        if(parent->dx1 > cx1) 
        {
            cx1 = parent->dx1;
        }

        if(parent->dy1 > cy1)
        {
            cy1 = parent->dy1;
        }

        if(parent->dx2 < cx2)
        {
            cx2 = parent->dx2;
        }

        if(parent->dy2 < cy2) 
        {
            cy2 = parent->dy2;
        }
    }
    else
    {
        // Use absolute coords

        cx1 = x1; cy1 = y1; cx2 = x2; cy2 = y2;
    }

    // now adjust drawing coords 

    if(CheckPlaneCrossing(x1, y1, x2, y2, cx1, cy1, cx2, cy2) == true)
    {
        setVisible(true);

        dx1 =       x1 > cx1 ? x1 : cx1;
        dy1 =       y1 > cy1 ? y1 : cy1;
        dwidth =    x2 < cx2 ? width : cx2 - x1 + 1;
        dheight =   y2 < cy2 ? height : cy2 - y1 + 1;

        dwidth -= (dx1 - x1);
        dheight -= (dy1 - y1);

        dx2 =  dx1 + dwidth - 1;
        dy2 =  dy1 + dheight - 1;

        if(window)
        {
            window->addRepaint(dx1, dy1, dwidth, dheight); // window->listen->repaint(dx1, dy1, dwidth, dheight);
        }
    }
    else
    {
        setVisible(false);

       // dx1 = dy1 = dx2 = dy2 = dwidth = dheight = 0;
    }
}

// Restrict drawing area for child objects to certain rectangle, defined by 4 coords
//
void Gobj::confine(int x1r, int y1r, int x2r, int y2r)
{
    bx1 = (x1r == -1 ? 0 : x1r); 
    by1 = (y1r == -1 ? 0 : y1r); 
    bx2 = (x2r == -1 ? width - 1 : x2r); 
    by2 = (y2r == -1 ? height - 1 : y2r);

    int cx1 = dx1 - x1, cx2 = dx2 - x1, cy1 = dy1 - y1, cy2 = dy2 - y1;

    if(cx1 > bx1) 
        bx1 = cx1;
    
    if(cy1 > by1) 
        by1 = cy1;
    
    if(cx2 < bx2) 
        bx2 = cx2;

    if(cy2 < by2) 
        by2 = cy2;
}

void Gobj::redraw(bool change)
{
    changed = change;

    if(change && window && window != this)
    {
        if(objGroup != ObjGroup_Highlight)
        {
            window->addChangedObject(this);
        }

        window->addRepaint(dx1, dy1, dwidth, dheight);
    }
}

void Gobj::remapAndRedraw()
{
    if (enabled)
    {
        redraw();

        mapObjects();
    }
}

void Gobj::handleDraw(Graphics& g)
{
    gr = &g;

    g.saveState();
    g.reduceClipRegion(dx1, dy1, dwidth, dheight);

    drawSelf(g);

    // Draw child objects

    for(Gobj* obj : objs)
    {
        if(obj->isShown() && obj->objGroup != ObjGroup_Highlight)
        {
            obj->handleDraw(g);
        }
    }

    drawOverChildren(g);

    g.restoreState();

    // Reset flags

    changed = false;
}

bool Gobj::checkMouseHovering(int mx, int my)
{
    return (isShown() && mx >= dx1 && mx <= dx2 && my >= dy1 && my <= dy2);
}

bool Gobj::isMouseTouching(int mx, int my)
{
    if(checkMouseHovering(mx, my))
    {
        if(!mouseHovering)
        {
            InputEvent ev = {};

            ev.mouseX = mx;
            ev.mouseY = my;

            handleMouseEnter(ev);
        }

        mouseHovering = true;
    }
    else
    {
        if(mouseHovering)
        {
            InputEvent ev = {};

            ev.mouseX = mx;
            ev.mouseY = my;

            handleMouseLeave(ev);
        }

        mouseHovering = false;
    }

    return mouseHovering;
}

bool Gobj::handleObjDrag(DragAndDrop& drag, Gobj* obj, int mx, int my)
{
    if(parent != NULL)
    {
        return parent->handleObjDrag(drag, obj, mx, my);
    }
    else
    {
        return false;
    }
}

bool Gobj::handleObjDrop(Gobj* obj, int mx, int my, unsigned flags)
{
    if(parent != NULL) 
    {
        return parent->handleObjDrop(obj, mx, my, flags);
    }
    else
    {
        return false;
    }
}

void Gobj::setc(Graphics& g, float c, float a)
{
    gSetMonoColor(g, c, a);
}

void Gobj::setc(Graphics& g, uint32 c)
{
    gSetColor(g, c);
}

void Gobj::setc(Graphics& g, uint32 c, float b, float a)
{
    gSetColor2(g, c, b, a);
}

void Gobj::fill(Graphics& g, float clr, float alpha) 
{
    gSetMonoColor(g, clr, alpha);
    gFillRect(g, x1, y1, x2, y2);
}

void Gobj::rect(Graphics& g, float clr, float alpha) 
{
    gSetMonoColor(g, clr, alpha);
    gDrawRect(g, x1, y1, x2, y2);
}

void Gobj::fill(Graphics& g, uint32 clr, float b, float a) 
{
    gSetColor2(g, clr, b, a);
    gFillRect(g, x1, y1, x2, y2);
}

void Gobj::rect(Graphics& g, uint32 clr, float b, float a) 
{
    gSetColor2(g, clr, b, a);
    gDrawRect(g, x1, y1, x2, y2);
}

void Gobj::rectx(Graphics& g, int x, int y, int w, int h) 
{
    gDrawRectWH(*gr, x1 + x, y1 + y, w, h);
}

void Gobj::fill(Graphics& g, uint32 clr) 
{
    gSetColor(g, clr);
    gFillRect(g, x1, y1, x2, y2);
}

void Gobj::fillx(Graphics& g,int x, int y, int w, int h)
{
    gFillRectWH(*gr, x1 + x, y1 + y, w, h);
}

void Gobj::rect(Graphics& g, uint32 clr) 
{
    gSetColor(g, clr);
    gDrawRect(g, x1, y1, x2, y2);
}

void Gobj::lineH(Graphics& g, int ly,int lx1,int lx2)
{
    gLineHorizontal(g, y1 + ly, x1 + lx1, x1 + lx2);
}

void Gobj::lineV(Graphics& g, int lx,int ly1,int ly2)
{
    gLineVertical(g, x1 + lx, y1 + ly1, y1 + ly2);
}

void Gobj::line(int lx,int ly,int lx2,int ly2)
{
    gLine(*gr, x1 + lx, y1 + ly, x1 + lx2, y1 + ly2);
}

void Gobj::deactivateDropBoxById(int dropbox_id)
{
    for(Gobj* obj : objs)
    {
        DropBox* db = dynamic_cast<DropBox*>(obj);

        if(db != NULL && db->getMenuId() == dropbox_id)
        {
            db->deactivate();
            break;
        }
    }
}

void Gobj::setMyColor(Graphics & g, float brightness, float saturation, float alpha)
{
/*
    if(colorHue > 0)
    {
        g.setColour(Colour(colorHue, saturation > 0 ? saturation : colorSat, brightness > 0 ? brightness : 1, alpha));
    }
    else */if(brightness > 0)
    {
        gSetMonoColor(g, brightness, alpha);
    }
    else if(monoLevel > 0)
    {
        gSetMonoColor(g, monoLevel, alpha);
    }
}


// Checks if mouse y coordinate is located between two objects going vertical
// Used in drag'n'drop for instruments and effects
//
// Params: [in] object list, object name filter, y coord [out] upper object, lower object
//
// Return: lower object
//
Gobj* CheckNeighborObjectsY(std::list<Gobj*> &lst, std::string oname, int my, Gobj** upper, Gobj** lower)
{
    Gobj* u = NULL;
    Gobj* l = NULL;

    for(Gobj* o : lst)
    {
        if(o->isShown() && o->getObjId().substr(0, oname.size()) == oname)
        {
            if(my > o->getY1() + o->getH()/2)
            {
                if(u == NULL || o->getY1() > u->getY1())
                {
                    u = o;
                }
            }

            if(my < o->getY1() + o->getH()/2)
            {
                if(l == NULL || o->getY1() < l->getY1())
                {
                    l = o;
                }
            }
        }
    }

   *upper = u;
   *lower = l;

    return l;
}

Gobj* CheckNeighborObjectsX(std::list<Gobj*> &lst, std::string oname, int mx, Gobj** lefter, Gobj** righter)
{
    Gobj* l = NULL;
    Gobj* r = NULL;

    for(Gobj* o : lst)
    {
        if(o->isShown() && o->getObjId().substr(0, oname.size()) == oname)
        {
            if(mx > o->getX1() + o->getW()/2)
            {
                if(l == NULL || o->getX1() > l->getX1())
                {
                    l = o;
                }
            }

            if(mx < o->getX1() + o->getW()/2)
            {
                if(r == NULL || o->getX1() < r->getX1())
                {
                    r = o;
                }
            }
        }
    }

   *lefter = l;
   *righter = r;

    return r;
}



