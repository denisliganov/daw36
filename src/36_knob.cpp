
#include "rosic/rosic.h"

#include "36_knob.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_macros.h"
#include "36_window.h"
#include "36_instr.h"
#include "36_mixchannel.h"
#include "36_effects.h"




Knob::Knob(Parameter* par, bool knob)
{
    setFontId(FontSmall);
    setParam(par);

    knobMode = knob;
    angleRange = PI * 1.5f;
    angleOffset = float(2*PI - angleRange)*.5f;
    defPos = 0;
    savedHeight = 0;
    dim = false;
    dimOnZero = false;
    hasText = true;
    instr = NULL;
    sliding = false;

    updPosition();
}

std::string Knob::getClickHint()
{
    return ""; //param->getName() + ":  " + param->getValString() + " " + param->getUnitString();
}

void Knob::updValue()
{
    if (param != NULL)
    {
        param->adjustFromControl(this, 0, (1 - positionAngle/angleRange));
    }

    redraw();
}

void Knob::updPosition()
{
    if (param != NULL)
    {
        positionAngle = float((1 - param->getNormalizedValue()) * angleRange);
    }
    else
    {
        positionAngle = 0;
    }

    redraw();
}

void Knob::handleMouseWheel(InputEvent& ev)
{
    if (!active)
        return;

    if (knobMode)
    {
        positionAngle -= (float)((PI / 32) * ev.wheelDelta);

        if (positionAngle < 0)
        {
            positionAngle = 0 ;
        }
        else if (positionAngle > angleRange)
        {
            positionAngle = angleRange;
        }

        updValue();
    }
    else
    {
        param->adjustFromControl(this, (float)ev.wheelDelta);

        redraw();
    }
}

void Knob::handleSliding(InputEvent& ev)
{
    if (abs(defPos - (ev.mouseX - x1)) < 2)
    {
        param->setValue(param->getDefaultValue());
    }
    else
    {
        if (ev.clickDown && !hasText || (ev.mouseX - x1 <= width/2))
        {
            sliding = true;
        }

        if (sliding)
        {
            param->adjustFromControl(this, 0, float(ev.mouseX - x1)/(hasText ? width/2 : width));
        }
    }

    redraw();
}


void Knob::handleMouseDrag(InputEvent& ev)
{
    if (!active)
        return;

    if (knobMode)
    {
        int dy = ev.mouseY - ys;

        positionAngle += (float)dy / 25;

        if (positionAngle < 0)
        {
            positionAngle = (float)0;
        }
        else if (positionAngle > angleRange)
        {
            positionAngle = angleRange;
        }

        updValue();

        ys = ev.mouseY;
    }
    else
    {
        handleSliding(ev);
    }
}

void Knob::handleMouseDown(InputEvent & ev)
{
    if (!active)
        return;

    if (knobMode)
    {
        ys = ev.mouseY;
    }
    else
    {
        handleSliding(ev);
    }
}

void Knob::handleMouseUp(InputEvent & ev)
{
    sliding = false;
}

void Knob::remap()
{
    MixChannel* mchan = dynamic_cast<MixChannel*>(parent);
    Eff* eff = dynamic_cast<Eff*>(parent);
    Instr* i = dynamic_cast<Instr*>(parent);
    Device36* dev = dynamic_cast<Device36*>(parent);

    if (i)
    {
        instr = i;
    }
    else if (eff)
    {
        instr = eff->getMixChannel()->getInstr();
    }
    else if (mchan)
    {
        //instr = mchan->getInstr();
    }
    else if (dev)
    {
        eff = dynamic_cast<Eff*>(dev->getContainer());

        i = dynamic_cast<Instr*>(dev->getContainer());

        if (i)
        {
            instr = i;
        }
        else if (eff)
        {
            mchan = eff->getMixChannel();

            if (mchan)
            {
                //instr = mchan->getInstr();
            }
        }
    }
    
    // Temp w/a
    //instr = NULL;

    if (param)
    {
        defPos = int(float(width-1)*param->getDefaultValueNormalized());
    }

    delSnap();
}

void Knob::drawText(Graphics& g)
{
    int w = width/2 - 6;
    int x = width - width/2;
    int textX = x + 6;  // height + 2
    int textY = 10;
    int namestrLen = gGetTextWidth(fontId, param->getName());
    int unitstrLen = gGetTextWidth(fontId, param->getUnitString());
    int valstrLen = gGetTextWidth(fontId, param->getValString());

    setc(g, .3f);
    fillx(g, x, 0, w, height);

    setc(g, .8f);
    if (unitstrLen > 0)
        //txt(g, fontId, param->getName() + "." + param->getUnitString(), textX, textY);
        txt(g, fontId, param->getName(), textX, textY);
    else
        txt(g, fontId, param->getName(), textX, textY);

    setc(g, 1.f);
    txt(g, fontId, param->getValString() + " " + param->getUnitString(), textX + w - valstrLen - unitstrLen - 4, textY);

    /*
    if (instr)
        instr->setMyColor(g, 1.f);
    else
        setc(g, .9f);
    txt(g, fontId, param->getValString(), textX + w - valstrLen - 2, textY);

    setc(g, .52f);
    txt(g, fontId, param->getUnitString(), textX + w - unitstrLen - 2, height - 3);
    */
}

void Knob::drawSlider(Graphics& g)
{
    int w = hasText ? width - width/2 : width;
    float offs = param->getOffset();
    float range = param->getRange();
    float val = param->getValue();
    float def = param->getDefaultValue();
    float baseVal = (offs <= 0 ? 0 : offs);

    int xoffs = int(float(w-1)*((baseVal - offs)/range));
    int xval = int(float(w-1)*((val - offs)/range));
    int xdef = int(float(w-1)*((def - offs)/range));
    int xstart = xoffs;
    int xend = xval;

    if(xval < xoffs)
    {
        xstart = xval;
        xend = xoffs;
    }

    int wsl = xend - xstart;
    int sh = height;     //height - (textHeight + 1);
    int ysl = height - sh;

    // Black notch for default (initial) value
    //setc(g, 0.f);
    //fillx(g, defPos, height - sh, 1, sh);

    if (instr)
        instr->setMyColor(g, .1f);
    else
        setc(g, .0f);

    rectx(g, 0, 0, w, height);

    if (instr)
        instr->setMyColor(g, .3f);
    else
        setc(g, .2f);

    fillx(g, 0, 0, w, height);

    if (instr)
        instr->setMyColor(g, .66f, .42f);
    else
        setc(g, .5f);

    fillx(g, xstart, ysl+1, wsl, sh-2);

    //if (instr)
    //    instr->setMyColor(g, .1f);
    //else
    //    setc(g, 0.1f);
    //rectx(g, xstart, ysl, w, sh);

    //drawGlassRect(g, x1 + (float)xstart, y1 + (float)(height - sh+1), w, sh-1, Colour(180, 120, 120), 0, 0, true, true, true, true);

    if (instr)
        instr->setMyColor(g, .34f);
    else
        setc(g, .48f);

    fillx(g, xoffs, ysl+1, 1, sh-2);

    if (xdef != xoffs)
    {
        setc(g, .0f);
        fillx(g, xdef, ysl+1, 1, sh-2);
    }

    setc(g, 1.f);
    //fillx(g, xval, height - sh + 1, 1, sh - 2);
}

void Knob::drawKnob(Graphics& g)
{
    int     brd = 1;
    int     w = height - brd*2;// *0.8f;
    int     h = height - brd*2;// *0.8f;
    int     x = x1 + brd;
    int     y = y1 + brd;
    float   val = param->getNormalizedValue();
    bool    old = dim;

    dim = (dimOnZero && val == 0);

    if (dim != old)
    {
        bgSaved = false;
        savedHeight = h;
    }

    if (snap != NULL)
    {
        g.setColour(Colours::white);

        drawSnap(g);
    }
    else
    {
        Colour clr;

        if (objId == "snd")
        {
            clr = Colour(1.f, 0.f, dim ? .32 : .8f, 1.f);
        }
        else if (objId == "fx")
        {
            float br = .5f;

            /*
            if (val == 1.f)
                br = .8f;
            else if (val == 0.f)
                br = .3f;*/

            clr = Colour(1.f, 0.f, br, 1.f);
        }
        else if (instr)
        {
            float s = .4f;
            float b = dim ? .22 : .82f;
            float a = 1;

            clr = Colour(instr->getColorHue(), s, b, a);
        }

        drawGlassRound(g, x, y, w, clr, 1);

        //setc(g, .2f);
        //gPie(g, x, y, w, h, PI + angleOffset, 3*PI - angleOffset);
        //gPie(g, x, y, w, h, PI, 3*PI);
        //gEllipseFill(g, x1, y1, width, height);

        createSnap();
        bgSaved = true;
        savedHeight = h;
    }

    if (objId == "snd")
    {
        setc(g, .14f, 1.f);
    }
    else if (objId == "fx")
    {
        setc(g, .1f, 1.f);
    }
    else if (instr)
    {
        setc(g, .14f, 1.f);
    }

    float xadv0, xadv1, yadv0, yadv1;

    int rrad0 = w/2 - 1;
    int rrad1 = 0; // rrad0 / 5.f;

    float ang = positionAngle - (PI/2.f - angleOffset);

    if (positionAngle >= PI / 2)
    {
        xadv0 = rrad0 * cos(ang);
        xadv1 = rrad1 * cos(ang);
    }
    else
    {
        xadv0 = -float(rrad0 * cos(PI - ang));
        xadv1 = -float(rrad1 * cos(PI - ang));
    }

    yadv1 = rrad1 * sin(ang);
    yadv0 = rrad0 * sin(ang);

    int kx1 = (float)RoundFloat(x + w/2 + xadv1);
    int ky1 = (float)RoundFloat(y + h/2 - yadv1);
    int kx2 = (float)RoundFloat(x + w/2 + xadv0);
    int ky2 = (float)RoundFloat(y + h/2 - yadv0);

    gDoubleLine(g, kx1, ky1, kx2, ky2, 2);
}

void Knob::drawSelf(Graphics& g)
{
    //fill(g, .32f);
    setc(g, .1f);
    txt(g, fontId, param->getName(), 0, 12);

    if (!knobMode)
    {
        //fill(g, .18f);

        drawSlider(g);

        if (hasText)
        {
            drawText(g);
        }
    }

    // Knob

    if (knobMode)
    {
        drawKnob(g);
    }
}



