

#include "36_dragndrop.h"
#include "36_instr.h"
#include "36_draw.h"
#include "36_text.h"



DropHighlight::DropHighlight(bool vert)
{
    vertical = vert;

    setrelative(false);
}

void DropHighlight::drawself(Graphics & g)
{
    uint32 color = 0xffFF9930;
    uint32 clrDecr = 0x80000000;

    int y = y1 + height/2;
    int x = x1 + width/2;

    for (int c = 0; c < 4; c++)
    {
        g.setColour(Colour(color));

        if(vertical)
        {
            g.drawVerticalLine(x + c, (float)y1, (float)y2);
            g.drawVerticalLine(x - c, (float)y1, (float)y2);
        }
        else
        {
            g.drawHorizontalLine(y + c, (float)x1, (float)x2);
            g.drawHorizontalLine(y - c, (float)x1, (float)x2);
        }

        color -= clrDecr;
        clrDecr /=2;
    }
}

DragAndDrop::DragAndDrop()
{
    settouchable(false);

    addHighlight(dropHighlightHorizontal = new DropHighlight(false));
    addHighlight(dropHighlightVertical = new DropHighlight(true));

    reset();

    dragObj = targetObj = NULL;
}

void DragAndDrop::reset()
{
    count = 0;

    dragObj = targetObj = NULL;

    setCoords2(0, 0, -1, -1);

    dropHighlightHorizontal->setVisible(false);
    dropHighlightVertical->setVisible(false);
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

    dropHighlightHorizontal->setCoords2(-1,-1, 1, 1);
    dropHighlightVertical->setCoords2(-1,-1, 1, 1);
}

void DragAndDrop::drag(Gobj* target_object, int mx, int my)
{
    dropHighlightHorizontal->setCoords1(-1,-1, 1, 1);   // disable by default
    dropHighlightVertical->setCoords1(-1,-1, 1, 1);   // disable by default

    bool result = target_object->handleObjDrag(*this, dragObj, mx, my);

    if(!result)
    {
        int tw = gGetTextWidth(FontSmall, dragObj->getObjName());
        int th = gGetTextHeight(FontSmall);

        setCoords1(mx - tw/2, my - th/2, tw, th);
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

void DragAndDrop::drawself(Graphics & g)
{
    if(targetObj)
    {
        bool drawn = targetObj->drawDraggedObject(g,(Gobj*)dragObj);

        if(!drawn)
        {
            Instrument* i = dynamic_cast<Instrument*>(dragObj);
            
            if(i)
            {
                int tw = gGetTextWidth(FontSmall, i->getObjName());

                int th = gGetTextHeight(FontSmall);

                setc(g, .6f);

                fillx(g, 0, 0, width, height);

                setc(g, 1.f);

                gText(g, FontSmall, i->getObjName(), dx1, dy2 - 2);
            }
            else
            {
                setc(g, 0.5f);

                fillx(g, 0, 0, width, height);

                setc(g, 0.99f);

                gText(g, FontSmall, dragObj->getObjName(), dx1, dy2 - 2);
            }
        }
    }
}



