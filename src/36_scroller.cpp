


#include "36_scroller.h"
#include "36_draw.h"
#include "36_ctrlpanel.h"




Scroller::Scroller(bool is_vertical)
{
    vertical = is_vertical;

    event = {};

    ratio = 0;
    offset = 0;
    fullspan = 0;
    visiblepart = 0;

    coordref = is_vertical ? &y1 : &x1;
}

void Scroller::handleMouseUp(InputEvent & ev)
{
    currpos = 0;
}

void Scroller::handleMouseDrag(InputEvent& ev)
{
    int mouseRef = vertical ? ev.mouseY : ev.mouseX;

    if(currpos == 0)
    {
        event = ev;

        setOffs(float(mouseRef - barpos - (*coordref + 1))/ratio);
    }
}

void Scroller::handleMouseDown(InputEvent& ev)
{
    currpos = getPos(ev, barpos);

    if(currpos != 0)
    {
        int mouseRef = vertical ? ev.mouseY : ev.mouseX;
        int ref = mouseRef - (*coordref + 1);

        event = ev;
        //setOffset(offset + (visiblepart*0.9f)*currpos);
        setOffs(float(ref)/ratio - float(barpixlen*0.5f)/ratio);

        int barRef = (*coordref + 1 + offsetpix);
        barpos = mouseRef - barRef;
        currpos = 0;
    }
}

int Scroller::getPos(InputEvent& ev, int& offset_on_bar)
{
    int pos = 0;
    int mouseRef = vertical ? ev.mouseY : ev.mouseX;
    int barRef = (*coordref + 1 + offsetpix);

    if (mouseRef >= barRef && mouseRef <= (barRef + barpixlen))
    {
        pos = 0;

        offset_on_bar = mouseRef - barRef; 
    }
    else if (mouseRef < barRef)
    {
        pos = -1;
    }
    else
    {
        pos = 1;
    }

    return pos;
}

void Scroller::adjOffs(float delta)
{
    setOffs(offset + delta);
}

void Scroller::setOffs(float offs)
{
    offset = offs;

    if(offset < 0)
    {
        offset = 0;
    }

    if(offset + visiblepart > fullspan)
    {
        offset = fullspan - visiblepart;
    }

    if(fullspan < visiblepart)
    {
        offset = 0;
        active = false;
    }

    offsetpix = int(fullpixlen*(offset/fullspan));

    redraw();

    parent->handleChildEvent(this, event);
}

void Scroller::handleMouseWheel(InputEvent& ev)
{
    float ofsDelta = ev.wheelDelta*(visiblepart*0.1f);

    event = ev;

    setOffs(offset - ofsDelta);
}

void Scroller::updBounds(float full_span, float visible_span, float offs)
{
    fullspan = full_span;
    visiblepart = visible_span;
    offset = offs;

    if(offset < 0)
    {
        offset = 0;
    }

    if(fullspan > visiblepart || offset > 0)
    {
        active = true;

        if(vertical)
        {
            fullpixlen = height - 2;
        }
        else
        {
            fullpixlen = width - 2;
        }

        ratio = (float)fullpixlen/fullspan;

        barpixlen = int(ratio*visiblepart);

        offsetpix = int(ratio*offset);
    }
    else
    {
        active = false;
    }

    redraw();
}

void Scroller::drawSelf(Graphics & g)
{
    fill(g, .22f);

    setc(g, .12f);

    rectx(g, 0, 0, width, height);

    if(active)
    {
        setc(g, 0.45f);

        if(vertical)
        {
            fillx(g, 3, 1 + offsetpix, width - 4, barpixlen);
        }
        else
        {
            fillx(g, 1 + offsetpix, 1, barpixlen, height - 2);
        }
    }
}

void Scroller::gotoStart()
{
    event = {};
    setOffs(0);
}

void Scroller::gotoEnd()
{
    event = {};
    setOffs(fullspan - visiblepart);
}

Scrolled::Scrolled()
{
    addObject(vscr = new Scroller(true));
    addObject(hscr = new Scroller(false));
}

float Scrolled::getHoffs()
{
    return hscr->getoffs();
}

void Scrolled::setHoffs(float offs)
{
    hscr->setOffs(offs);
}

float Scrolled::getVoffs()
{
    return vscr->getoffs();
}

void Scrolled::setVoffs(float offs)
{
    vscr->setOffs(offs);
}

