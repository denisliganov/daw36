
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




Knob::Knob(Parameter* par)
{
    setFontId(FontSmall);

    addParam(par);

    angleRange = PI * 1.5f;

    angleOffset = float(2*PI - angleRange)*.5f;

    instr = NULL;

    updPosition();
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
        positionAngle = float((1 - param->getValueNormalized()) * angleRange);
    }
    else
    {
        positionAngle = 0;
    }

    redraw();
}

void Knob::handleMouseWheel(InputEvent& ev)
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

void Knob::handleMouseDrag(InputEvent& ev)
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

void Knob::handleMouseDown(InputEvent & ev)
{
    ys = ev.mouseY;
}

void Knob::handleMouseUp(InputEvent & ev)
{
}

void Knob::setVis(bool vis)
{
    if (vis == false)
    {
        int a = 1;
    }

    Gobj::setVis(vis);
}

void Knob::remap()
{
    MixChannel* mchan = dynamic_cast<MixChannel*>(parent);
    Eff* eff = dynamic_cast<Eff*>(parent);
    Instrument* i = dynamic_cast<Instrument*>(parent);
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
        instr = mchan->getInstr();
    }
    else if (dev)
    {
        eff = dynamic_cast<Eff*>(dev->getContainer());
        i = dynamic_cast<Instrument*>(dev->getContainer());

        if (i)
        {
            instr = i;
        }
        else if (eff)
        {
            mchan = eff->getMixChannel();

            if (mchan)
            {
                instr = mchan->getInstr();
            }
        }
    }
}

void Knob::drawText(Graphics& g)
{
    int textX = 4;  // height + 2
    int namestrLen = gGetTextWidth(fontId, param->getName());
    int unitstrLen = gGetTextWidth(fontId, param->getUnitString());
    int valstrLen = gGetTextWidth(fontId, param->getValString());

    setc(g, .7f);
    if (unitstrLen > 0)
        txt(g, fontId, param->getName() + "." + param->getUnitString() + ": ", textX, 11);
    else
        txt(g, fontId, param->getName() + ": ", textX, 11);

    //setc(g, .9f);
    if (instr)
        instr->setMyColor(g, 1.f);
    else
        setc(g, 1.f);
    //txt(g, fontId, param->getValString(), textX + namestrLen + 6, 12);
    //txt(g, fontId, param->getValString(), textX + 60, 12);
    txt(g, fontId, param->getValString(), width - valstrLen - 2, 11);

    setc(g, .6f);
    //txt(g, fontId, param->getUnitString(), width - unitstrLen - 2, 12);
}

void Knob::drawSlider(Graphics& g)
{
    float offs = param->getOffset();
    float range = param->getRange();
    float val = param->getValue();
    float baseVal = (offs <= 0 ? 0 : offs);

    int xoffs = int(float(width-1)*((baseVal - offs)/range));
    int xval = int(float(width-1)*((val - offs)/range));
    int xstart = xoffs;
    int xend = xval;

    if(xval < xoffs)
    {
        xstart = xval;
        xend = xoffs;
    }

    int w = xend - xstart;
    int sh = 2;     //height - (textHeight + 1);

    // Black notch for default (initial) value
    //setc(g, 0.f);
    //fillx(g, defPos, height - sh, 1, sh);

    if (instr)
        instr->setMyColor(g, .8f);
    else
        setc(g, 0.8f);
    fillx(g, xstart, height - sh, w, sh);

    if (instr)
        instr->setMyColor(g, 1.f);
    else
        setc(g, 1.f);
    fillx(g, xstart, height - sh+1, w, sh-2);

    //drawGlassRect(g, x1 + (float)xstart, y1 + (float)(height - sh+1), w, sh-1, Colour(180, 120, 120), 0, 0, true, true, true, true);

    if (instr)
        instr->setMyColor(g, .4f);
    else
        setc(g, 0.4f);
    fillx(g, xoffs, height - sh, 1, sh);

    setc(g, 1.f);
    //fillx(g, xval, height - sh + 1, 1, sh - 2);
}

void Knob::drawKnob(Graphics& g)
{
    //gSetMonoColor(g, .5f, 1);
    //gPie(g, x1 + 1, y1+1, width-2, height-2, PI - angleOffset, PI + angleOffset);
    //gSetMonoColor(g, .3f);
    //gEllipseFill(g, x1 + 3, y1 + 3, width-6, height-6);
    //float s = 1.f/2*PI*((width-2)/2);


    gSetMonoColor(g, .3f);

    int w = height*0.8f;
    int h = height*0.8f;

    //w = MAX(w, h)/2;
    //h = w;

    int x = x1 + 2;
    int y = y1 + 2;

    gSetMonoColor(g, .4f);
    gEllipseFill(g, x, y, w, h);

    //(55, 45, 35)
    //drawGlassRound(g,  x, y, w, Colour(90, 80, 10), 1);

    if(0 && param->getOffset() < 0)
    {
        float o = param->getOffset() / param->getRange();
        float oa = abs(o*angleRange);

        float rad = float(w-2)/2;
        float singleAngle = 1.f/(2*PI);
        float ratio = 1.f/(2*PI*rad);
        float singlePixelAngle = ratio*2*PI;

        gSetMonoColor(g, .6f);

        gPie(g, x1+1, y1+1, width-2, height-2, PI + angleOffset + oa, PI + angleOffset + oa);
    }

    //setc(g, .2f);
    //gPie(g, x, y, w, h, PI + angleOffset, 3*PI - angleOffset);
    //gPie(g, x, y, w, h, PI, 3*PI);

    //gEllipseFill(g, x1, y1, width, height);

    float xadv0, xadv1, yadv0, yadv1;

    int rrad0 = w/2 - 1;
    int rrad1 = rrad0/4.f;

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

    //if(instr)
    //    instr->setMyColor(g,.9);
    //else
    //    parent->setMyColor(g, .9f);

    setc(g, 0.9f);

    int kx1 = (float)RoundFloat(x + w/2 + xadv1);
    int ky1 = (float)RoundFloat(y + h/2 - yadv1);
    int kx2 = (float)RoundFloat(x + w/2 + xadv0);
    int ky2 = (float)RoundFloat(y + h/2 - yadv0);

    gDoubleLine(g, kx1, ky1, kx2, ky2, 2);
}

void Knob::drawSelf(Graphics& g)
{
    //Instrument* instr = dynamic_cast<Instrument*>(parent);

    fill(g, .28f);
    //fill(g, .32f);

    drawText(g);

    // Knob
    if (false)
    {
        drawKnob(g);
    }

    drawSlider(g);
}



