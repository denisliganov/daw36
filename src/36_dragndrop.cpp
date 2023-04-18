

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

    sw->setAlwaysOnTop(true);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    targetObj = target_object;

    sw->setBounds(mx - getW()/2, my + getH(), getW(), getH());

    bool result = target_object->handleObjDrag(*this, dragObj, mx, my);
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
}

