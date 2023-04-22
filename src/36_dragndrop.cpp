

#include "36_dragndrop.h"
#include "36_instr.h"
#include "36_text.h"
#include "36_draw.h"
#include "36_window.h"







DragAndDrop::DragAndDrop()
{
    setTouchable(false);

    count = 0;

    dragObj = targetObj = NULL;

    sw = NULL;
}

DragAndDrop::~DragAndDrop()
{
    sw = NULL;
}

bool DragAndDrop::canDrag()
{
    if(count < 3)
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

    setWidthHeight(gGetTextWidth(FontSmall, dragObj->getObjName()) + 8, gGetTextHeight(FontSmall) + 8);

    if (sw == NULL)
    {
        sw = MWindow->createChildWindowFromWinObject(this, mx, my, false);
        sw->setResizable(false, false);
    }

    //sw->setBounds(mx, my, getW() + 4, getH() + 4);
    //sw->setBounds(mx - getW() / 2 - 2, my + getH() - 2, getW() + 4, getH() + 4);
    sw->setOpen(true);
    sw->setAlwaysOnTop(true);

    drag(NULL, mx, my);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    if (targetObj != NULL && targetObj != target_object)
    {
        targetObj->handleObjDrag(true, dragObj, mx, my);
    }

    targetObj = target_object;

    //sw->setBounds(mx - getW()/2, my + getH(), getW(), getH());
    sw->setBounds(mx - getW(), my + getH()/2, getW(), getH());

    if (targetObj)
    {
        if (targetObj == (Gobj*)MGrid)
        {
            sw->setVisible(false);
        }
        else
        {
            sw->setVisible(true);
        }

        target_object->handleObjDrag(false, dragObj, mx, my);
    }

    redraw();
}

void DragAndDrop::drop(int mx,int my,unsigned int flags)
{
    if(targetObj)
    {
        Gobj* obj = (Gobj*)dragObj;
        dragObj = NULL;
        targetObj->handleObjDrop(obj, mx, my, flags);
    }

    if (sw)
        sw->setOpen(false);
}

void DragAndDrop::drawSelf(Graphics & g)
{
    fill(g, 0.4f);
    setc(g, 1.f);
    gText(g, FontSmall, dragObj->getObjName(), 4, gGetTextHeight(FontSmall));
}

