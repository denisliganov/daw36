

#include "36_dragndrop.h"
#include "36_instr.h"
#include "36_text.h"
#include "36_draw.h"






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

        uint8 a = 255;

        Rect36 drwRect ={(float)(x1), (float)y1, (float)width, (float)height};

        for (int c = 0; c < 4; c++)
        {
            gSetColor(g, 255, 153, 48, 255 - a);

            //gDrawRect(g, x1, y1, width, height);
            gDrawRectWH(g, drwRect.x, drwRect.y, drwRect.w, drwRect.h);

            drwRect.x++;
            drwRect.y++;
            drwRect.w -= 2;
            drwRect.h -= 2;

            if (drwRect.h < 1 || drwRect.w < 1)
            {
                break;
            }

            a /= 2;
        }
    }
}

void DropHighlightRect::drawSelf(Graphics & g)
{
    uint32 color = 0xffFF9930;

    setc(g, color);

    rectx(g, 0, 0, width, height);
}

DragAndDrop::DragAndDrop()
{
    setTouchable(false);

    addHighlight(dropHighlight = new DropHighlight());
    addHighlight(dropRect = new DropHighlightRect());

    reset();

    dragObj = targetObj = NULL;
}

void DragAndDrop::reset()
{
    count = 0;

    dragObj = targetObj = NULL;

    setCoords2(0, 0, -1, -1);

    dropHighlight->setVis(false);
    dropRect->setVis(false);
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

    dropHighlight->setCoords2(-1,-1, 1, 1);
    dropRect->setCoords2(-1,-1, 1, 1);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    dropHighlight->setCoords1(-1,-1, 1, 1);   // disable by default
    dropRect->setCoords1(-1,-1, 1, 1);

    bool result = target_object->handleObjDrag(*this, dragObj, mx, my);

    if(!result)
    {
        int tw = gGetTextWidth(FontSmall, dragObj->getObjName());
        int th = gGetTextHeight(FontSmall);

        //setCoords1(mx - tw/2, my - th/2, tw, th);
        setCoordsAbs(mx - tw/2, my - th/2, mx - tw/2 + tw, my - th/2 + th);
    }

    targetObj = target_object;
}

void DragAndDrop::drop(int mx,int my,unsigned int flags)
{
    if(targetObj)
    {
        targetObj->handleObjDrop((Gobj*)dragObj, mx, my, flags);
    }

    reset();
}

void DragAndDrop::drawSelf(Graphics & g)
{
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

