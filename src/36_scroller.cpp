


#include "36_scroller.h"
#include "36_draw.h"
#include "36_ctrlpanel.h"




Scroller::Scroller(bool is_vertical)
{
    vertical = is_vertical;

    event = {};

    wheelMult = 1;
    ratio = 0;

    coordRef = is_vertical ? &y1 : &x1;
}

void Scroller::handleMouseUp(InputEvent & ev)
{
    currPos = 0;
}

void Scroller::handleMouseDrag(InputEvent& ev)
{
    int mouseRef = vertical ? ev.mouseY : ev.mouseX;

    if(currPos == 0)
    {
        event = ev;
        setoffs(float(mouseRef - barPos - (*coordRef + 1))/ratio);
    }
}

void Scroller::handleMouseDown(InputEvent& ev)
{
    currPos = getPosition(ev, barPos);

    if(currPos != 0)
    {
        int mouseRef = vertical ? ev.mouseY : ev.mouseX;
        int ref = mouseRef - (*coordRef + 1);

        event = ev;
        //setOffset(offset + (visibleSpan*0.9f)*currPos);
        setoffs(float(ref)/ratio - float(barPixLen*0.5f)/ratio);

        int barRef = (*coordRef + 1 + offsetPix);
        barPos = mouseRef - barRef;
        currPos = 0;
    }
}

int Scroller::getPosition(InputEvent& ev, int& offset_on_bar)
{
    int pos = 0;
    int mouseRef = vertical ? ev.mouseY : ev.mouseX;
    int barRef = (*coordRef + 1 + offsetPix);

    if (mouseRef >= barRef && mouseRef <= (barRef + barPixLen))
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

void Scroller::setoffs(float offs)
{
    offset = offs;

    if(offset < 0) 
        offset = 0;

    if(offset + visibleSpan > fullSpan) 
        offset = fullSpan - visibleSpan;

    if(fullSpan < visibleSpan)
    {
        offset = 0;
        active = false;
    }

    offsetPix = int(fullPixLen*(offset/fullSpan));

    redraw();

    parent->handleChildEvent(this, event);
}

void Scroller::handleMouseWheel(InputEvent& ev)
{
    float ofsDelta = ev.wheelDelta*(visibleSpan*0.1f)*wheelMult;

    event = ev;

    setoffs(offset - ofsDelta);
}

void Scroller::updateLimits(float full_span, float visible_span, float offs)
{
    fullSpan = full_span;
    visibleSpan = visible_span;
    offset = offs;

    if(offset < 0)
    {
        offset = 0;
    }

    if(fullSpan > visibleSpan || offset > 0)
    {
        active = true;

        if(vertical)
        {
            fullPixLen = height - 2;
        }
        else
        {
            fullPixLen = width - 2;
        }

        ratio = (float)fullPixLen/fullSpan;

        barPixLen = int(ratio*visibleSpan);

        offsetPix = int(ratio*offset);
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
            fillx(g, 3, 1 + offsetPix, width - 4, barPixLen);
        }
        else
        {
            fillx(g, 1 + offsetPix, y1 + 1, barPixLen, height - 2);
        }
    }
}

void Scroller::goToStart()
{
    event = {};
    setoffs(0);
}

void Scroller::goToEnd()
{
    event = {};
    setoffs(fullSpan - visibleSpan);
}


