

#include "36.h"
#include "36_button.h"
#include "36_params.h"
#include "36_controls.h"
#include "36_glyphs.h"
#include "36_ctrlpanel.h"
#include "36_grid.h"
#include "36_draw.h"
#include "36_text.h"




Button36* GetButton(Gobj* parent, std::string id)
{
    for (auto obj : parent->objs)
    {
        if (obj->getObjId() == id)
        {
            return (Button36*)obj;
        }
    }

    return NULL;
}



Button36::Button36(bool stick, std::string title)
{
    objTitle = title;
    sticking = stick;

    pressed = false;
    isLedType = false;
}

void Button36::addParam(Parameter* prm)
{
    if (prm->type == Param_Bool)
    {
        param = prm;

        pressed = &((BoolParam*)param)->outval;
    }
}

void Button36::process(bool down)
{
    static bool pressing = false;

    if(!sticking)
    {
        pressed = down;
    }
    else
    {
        if(down)
        {
            if(!pressed)
            {
                pressing = true;
            }

            pressed = down;
        }
        else
        {
            if(pressing)
            {
                pressing = false;
            }
            else
            {
                pressed = false;
            }
        }
    }

    redraw();
}

void Button36::press()
{
    pressed = true;

    redraw();
}

void Button36::toggle()
{
    pressed = !pressed;

    redraw();
}

void Button36::release()
{
    pressed = false;

    redraw();
}

bool Button36::isPressed()
{
    return pressed;
}

void Button36::handleMouseDown(InputEvent& ev)
{
    process(true);

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Button36::handleMouseWheel(InputEvent& ev)
{
    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Button36::handleMouseUp(InputEvent& ev)
{
    process(false);

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Button36::updPosition()
{
    redraw();
}

void Button36::drawButtonBase(Graphics& g)
{
    if (!pressed)
    {
        gSetMonoColor(g, 0.25f);
        gFillRect(g, x1 + 1, y1 + 1, x2 - 1, y2 - 1);

        gSetMonoColor(g, 0.1f);
        gDrawRect(g, x1, y1, x2, y2);
    }
    else
    {
        if(isLedType)
        {
            gSetMonoColor(g, .6f);
            gFillRect(g, x1, y1, x2, y2);

            gSetMonoColor(g, .7f);
            gDrawRect(g, x1, y1, x2, y2);
        }
        else
        {
            gSetColor2(g, 0xffFFFF40, .6f);
            gFillRect(g, x1, y1, x2, y2);

            gSetColor2(g, 0xffFFFF40, .7f);
            gDrawRect(g, x1, y1, x2, y2);
        }
    }
}

void Button36::drawSelf(Graphics& g)
{
    drawButtonBase(g);

    if (pressed)
    {
        if (isLedType)
            gSetMonoColor(g, 1.f);
        else
            gSetColor2(g, 0xffFFFF40, 1.f);
     }
    else
    {
        gSetMonoColor(g, .7f);
    }

    DrawButtonGlyph(g, this);

    if(objTitle.size() > 0)
    {
        gSetMonoColor(g, .9f);

        gText(g, FontInst, objTitle, x1 + width / 2 - gGetTextWidth(FontInst, objTitle) / 2, y1 + height / 2 + gGetTextHeight(FontInst)/2 - 1);
    }
}


GroupButton::GroupButton(int group_id) : Button36(true)
{
    groupId = group_id;

    lastPressed = NULL;
}

void GroupButton::handleMouseDown(InputEvent & ev)
{
    process(true);

    for(Gobj* co : parent->objs)
    {
        if (co != this)
        {
            GroupButton* gb = dynamic_cast<GroupButton*>(co);

            if (gb && gb->getGroupId() == groupId && gb->isPressed())
            {
                gb->release();

                lastPressed = gb;
            }
        }
    }

    parent->handleChildEvent(this, ev);
}

void GroupButton::handleMouseUp(InputEvent & ev) 
{

}

void GroupButton::revertToPrevious()
{
    release();

    if (lastPressed)
    {
        lastPressed->press();
    }
}


