

#include "36_dragndrop.h"
#include "36_instr.h"
#include "36_text.h"
#include "36_draw.h"
#include "36_window.h"





void DropHighlight::drawSelf(Graphics & g)
{
    if (rect)
    {
        uint32 color = 0xffFF9930;

        setc(g, color);

        rectx(g, 0, 0, width, height);
    }
    else
    {
        //fill(g, 1.f);

        uint8 a = 128;

        Rect36 drwRect ={(float)(x1), (float)y1, (float)width, (float)height};

        bool vert = drwRect.w < drwRect.h;

        int count = vert ? width/2 : height/2;

        for (int c = 0; c < count; c++)
        {
            float m = (float)c/count;

            m *= m;

            gSetColor(g, 255, 153, 48, uint8(m*255));

            //gDrawRect(g, x1, y1, width, height);
            //gFillRectWH(g, drwRect.x, drwRect.y, drwRect.w, drwRect.h);

            if (vert)
            {
                gFillRectWH(g, drwRect.x, drwRect.y, 1, drwRect.h);
                gFillRectWH(g, drwRect.x + drwRect.w - 1, drwRect.y, 1, drwRect.h);

                drwRect.x++;
                drwRect.w -= 2;
            }
            else
            {
                gFillRectWH(g, drwRect.x, drwRect.y, drwRect.w, 1);
                gFillRectWH(g, drwRect.x, drwRect.y + drwRect.h - 1, drwRect.w, 1);

                drwRect.y++;
                drwRect.h -= 2;
            }

            if (drwRect.h < 1 || drwRect.w < 1)
            {
                break;
            }

            a /= 2;
        }
    }
}

DragAndDrop::DragAndDrop()
{
    setTouchable(false);

    addHighlight(dropHighlight = new DropHighlight());

    reset();
}

void DragAndDrop::reset()
{
    count = 0;

    dragObj = targetObj = NULL;

    setCoords2(0, 0, -1, -1);

    dropHighlight->setVis(false);
};

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

    sw = MWindow->createChildWindowFromWinObject(this, mx, my, false);

    sw->setBounds(mx, my, getW() + 4, getH() + 4);

    sw->setResizable(false, false);

    sw->setOpen(true);

    sw->grabKeyboardFocus();

    //sw->mouseDown(*(window->getLastEvent1()));

    sw->setAlwaysOnTop(true);

    //window->setOpen(true);

    dropHighlight->setCoords2(-1,-1, 1, 1);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    sw->setBounds(mx - getW()/2, my + getH(), getW(), getH());

    return;

    dropHighlight->setCoords1(-1,-1, 1, 1);   // disable by default

    bool result = target_object->handleObjDrag(*this, dragObj, mx, my);

    if(!result)
    {
        int tw = gGetTextWidth(FontSmall, dragObj->getObjName());
        int th = gGetTextHeight(FontSmall);

        //setCoords1(mx - tw/2, my - th/2, tw, th);
        //setCoordsAbs(mx - tw/2, my - th/2, mx - tw/2 + tw, my - th/2 + th);
    }

    targetObj = target_object;
}

void DragAndDrop::drop(int mx,int my,unsigned int flags)
{
    if(targetObj)
    {
        targetObj->handleObjDrop((Gobj*)dragObj, mx, my, flags);
    }

    dragObj->getWindow()->removeObject(this);

    reset();
}

void DragAndDrop::drawSelf(Graphics & g)
{
    fill(g, 0.4f);

    setc(g, 1.f);

    gText(g, FontSmall, dragObj->getObjName(), 2, height - 2);

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
    }
}


void DragAndDrop::setDropCoords(int x,int y,int w,int h,bool rect)
{
    dropHighlight->setCoordsNoCrop(x, y, x+w-1, y+h-1);
    dropHighlight->setRectType(rect);
}

