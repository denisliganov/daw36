

#include "36_slider.h"
#include "36_params.h"
#include "36_utils.h"
#include "36.h"
#include "36_draw.h"
#include "36_device.h"
#include "36_instr.h"
#include "36_mixer.h"



Slider36::Slider36(bool vert)
{
    vertical = vert;
    prm = NULL;

    slLen = 0;
    length = 0;
}

void Slider36::updValue()
{
    if (prm != NULL && length != 0)
    {
        prm->adjustFromControl(this, 0, (float)slLen/length);
    }
}

void Slider36::updPosition()
{
    if (prm != NULL)
    {
        slLen = RoundFloat((float)length * prm->getValueNormalized());

        slMul = (float)length * prm->getValue();
    }

    redraw();
}

void Slider36::handleMouseWheel(InputEvent& ev)
{
    slLen += ev.wheelDelta;

    if (slLen < 0)
    {
        slLen = 0;
    }

    if (slLen > length)
    {
        slLen = length;
    }

    updValue();

    parent->handleChildEvent(this, ev);

    redraw();
}

void Slider36::handleMouseDrag(InputEvent& ev)
{
    if (vertical)
    {
        slLen = y2 - ev.mouseY;
    }
    else
    {
        slLen = ev.mouseX - x1;
    }

    if (slLen > length)
    {
        slLen = length;
    }
    else if (slLen < 0)
    {
        slLen = 0;
    }

    updValue();

    parent->handleChildEvent(this, ev);

    redraw();
}

void Slider36::handleMouseDown(InputEvent& ev)
{
    if (ev.leftClick)
    {
        handleMouseDrag(ev);
    }
}

void Slider36::remap()
{
    if (vertical)
        length = height;
    else
        length = width;

    updPosition();
}

std::string Slider36::getHint()
{
    if (prm == NULL)
        return "";

    std::string hint = prm->getName().data();

    hint += ": ";
    hint += prm->getValString();

    return hint;
}

void Slider36::drawSelf(Graphics& g)
{
    Instrument* instr = dynamic_cast<Instrument*>(parent);
    MixChannel* mc = dynamic_cast<MixChannel*>(parent);

    fill(g, .4f);

    rect(g, .32f);

    if (prm == NULL)
        return;

    if (vertical)
    {
        int ypos = y2 - 1 - slLen;
        int ycenter = y2 - 1 - RoundFloat((float)length * ((0 - prm->getOffset()) / prm->getRange()));
        int ysl = ypos;
        int ysl2 = y2 - 1;
        int ythumb = ysl2;

        if (prm->getOffset() < 0)
        {
            if (ypos >= ycenter)
            {
                ysl = ycenter;
            }
            else
            {
                ysl2 = ycenter;

                ythumb = ysl;
            }
        }

        setc(g, .4f);

        fillx(g, 1, ysl - y1, width - 2, ysl2 - y1 - 1);

        // Draw thumb for signed prms

        if (abs(prm->getOffset()) > (abs(prm->getRange()) / 10.f))
        {
            setc(g, .8f);
        }
    }
    else
    {
        int xstart = int((length)*((0.f - prm->getOffset())/prm->getRange()));
        int xpos = slLen;

        int xfrom = xstart;
        int xwidth = xpos - xstart;

        if(xfrom > xpos)
        {
            xfrom = xpos;
            xwidth = xstart - xpos;
        }

        if (xwidth > 0)
        {
            setc(g, .5f);

            fillx(g, xfrom, 0, xwidth, height);

            setc(g, .7f);

            rectx(g, xfrom, 0, xwidth, height);
        }

        // Draw thumb for signed prms
/*
        if (abs(param->offset) > (abs(param->range) / 10.f))
        {
            if(instr)
            {
                instr->setDrawColor(g, 1);
            }
            else
            {
                setMonoColor(.6f);
            }
        }
        */
    }
}

