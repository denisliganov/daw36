

#include "36_dragndrop.h"
#include "36_instr.h"
#include "36_text.h"
#include "36_draw.h"
#include "36_window.h"







DragAndDrop::DragAndDrop()
{
    setTouchable(false);

    //addHighlight(dropHighlight = new DropHighlight());

    count = 0;

    dragObj = targetObj = NULL;

    sw = NULL;
}

bool DragAndDrop::canDrag()
{
    if(count < 2)
    {
        count++;

        return false;
    }
    else
    {
        count = 0;

        return true;
    }
}

bool DragAndDrop::isActive()
{
    return (dragObj != NULL);
}

void DragAndDrop::start(Gobj * drag_obj,int mx,int my)
{
    dragObj = drag_obj;

    //dragObj->getWindow()->addHighlight(this);

    //window->setTitleBarHeight(0);
    //window->setBounds(200, 200, getW(), getH());

    setWidthHeight(gGetTextWidth(FontSmall, dragObj->getObjName()), gGetTextHeight(FontSmall));

    if (sw == NULL)
    {
        sw = MWindow->createChildWindowFromWinObject(this, mx, my, false);
        sw->setResizable(false, false);
    }

    sw->setBounds(mx, my, getW() + 4, getH() + 4);

    sw->setOpen(true);

    //sw->mouseDown(*(window->getLastEvent1()));

    sw->setAlwaysOnTop(true);

    //window->setOpen(true);

    //dropHighlight->setCoords2(-1,-1, 1, 1);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    targetObj = target_object;

    sw->setBounds(mx - getW()/2, my + getH(), getW(), getH());

    //dropHighlight->setCoords1(-1,-1, 1, 1);   // disable by default

    bool result = target_object->handleObjDrag(*this, dragObj, mx, my);

    return;

    if(!result)
    {
        //int tw = gGetTextWidth(FontSmall, dragObj->getObjName());
        //int th = gGetTextHeight(FontSmall);
        //setCoords1(mx - tw/2, my - th/2, tw, th);
        //setCoordsAbs(mx - tw/2, my - th/2, mx - tw/2 + tw, my - th/2 + th);
    }
}

void DragAndDrop::drop(int mx,int my,unsigned int flags)
{
    if(targetObj)
    {
        targetObj->handleObjDrop((Gobj*)dragObj, mx, my, flags);
    }

    dragObj = NULL;

    if (sw)
        sw->setOpen(false);
}

void DragAndDrop::drawSelf(Graphics & g)
{
    fill(g, 0.4f);

    setc(g, 1.f);

    gText(g, FontSmall, dragObj->getObjName(), 2, height - 2);

/*
    return;

    if(targetObj)
    {
        bool drawn = targetObj->drawDraggedObject(g,(Gobj*)dragObj);

        if(!drawn)
        {
            setc(g, 0.5f);

            fillx(g, 0, 0, width, height);

            setc(g, 0.99f);

            gText(g, FontSmall, dragObj->getObjName(), dx1, dy2 - 2);
        }
    }*/
}


void DragAndDrop::setDropCoords(int x,int y,int w,int h,bool rect)
{
    //dropHighlight->setCoordsNoCrop(x, y, x+w-1, y+h-1);
    //dropHighlight->setRectType(rect);
}

