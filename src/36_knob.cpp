
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

    addParam(par);

    angleRange = PI * 1.5f;
    angleOffset = float(2*PI - angleRange)*.5f;

    knobMode = knob;

    defPos = 0;
    savedHeight = 0;

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
        param->adjustFromControl(this, 0, float(ev.mouseX - x1)/width);
    }

    redraw();
}


void Knob::handleMouseDrag(InputEvent& ev)
{
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
    
    // Temp w/a
    //instr = NULL;

    defPos = int(float(width-1)*param->getDefaultValueNormalized());
}

void Knob::drawText(Graphics& g)
{
    int textX = 3;  // height + 2
    int textY = 10;
    int namestrLen = gGetTextWidth(fontId, param->getName());
    int unitstrLen = gGetTextWidth(fontId, param->getUnitString());
    int valstrLen = gGetTextWidth(fontId, param->getValString());

    setc(g, .6f);
    if (unitstrLen > 0)
        //txt(g, fontId, param->getName() + "." + param->getUnitString(), textX, textY);
        txt(g, fontId, param->getName(), textX, textY);
    else
        txt(g, fontId, param->getName(), textX, textY);

    //setc(g, .9f);
    if (instr)
        instr->setMyColor(g, 1.f);
    else
        setc(g, .76f);
    //txt(g, fontId, param->getValString(), textX + namestrLen + 6, 12);
    //txt(g, fontId, param->getValString(), textX + 60, 12);
    txt(g, fontId, param->getValString(), width - valstrLen - 2, textY);

    setc(g, .52f);
    txt(g, fontId, param->getUnitString(), width - unitstrLen - 2, height - 3);
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
    int sh = height;     //height - (textHeight + 1);
    int ysl = height - sh;

    // Black notch for default (initial) value
    //setc(g, 0.f);
    //fillx(g, defPos, height - sh, 1, sh);

    if (instr)
        instr->setMyColor(g, .2f);
    else
        setc(g, 0.2f);

    fillx(g, xstart, ysl, w, sh);

    if (instr)
        instr->setMyColor(g, .3f);
    else
        setc(g, .36f);

    fillx(g, xstart, ysl+1, w, sh-2);

    //drawGlassRect(g, x1 + (float)xstart, y1 + (float)(height - sh+1), w, sh-1, Colour(180, 120, 120), 0, 0, true, true, true, true);

    if (instr)
        instr->setMyColor(g, .34f);
    else
        setc(g, .1f);

    fillx(g, xoffs, ysl, 1, sh);

    setc(g, 1.f);
    //fillx(g, xval, height - sh + 1, 1, sh - 2);
}

void Knob::drawKnob(Graphics& g)
{
    int w = height - 4;// *0.8f;
    int h = height - 4;// *0.8f;
    int x = x1 + 2;
    int y = y1 + 2;

    if (bgSaved && savedHeight == h)
    {
        g.setColour(Colours::white);

        drawSnap(g);
    }
    else
    {
        //float s = 1.f/2*PI*((width-2)/2);

        if (0)
        {
            gSetMonoColor(g, .4f);
            gEllipseFill(g, x, y, w, h);
        }
        else
        {
            //setc(g, .2f);
            //gPie(g, x1, y1, width, height, PI - angleOffset, PI + angleOffset);
            //gArc(g, x1, y1, width, height, PI - angleOffset, PI + angleOffset);

            if(instr)
                instr->setMyColor(g, 1.f);
            else
                //parent->setMyColor(g, 1.f);
                setc(g, .3f);
            
            //gEllipseFill(g, x1+1, y1+1, width-2, height-2);

            Colour clr = Colour(1.f, 0.f, 1.f, 1.f);

            if (instr)
            {
                float s = .4f;
                float b = .68f;
                float a = 1;

                clr = Colour(instr->getColorHue(), s, b, a);
            }

            drawGlassRound(g, x, y, w, clr, 1);

            ///gSetMonoColor(g, 0, .3f);
            //gPie(g, x+2, y+2, w-2, w-2, PI - angleOffset, PI + angleOffset);
        }

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

        createSnap();
        //window->makeSnapshot(&snap, xRel, yRel, width, height, instr->getSnap());

        bgSaved = true;
        savedHeight = h;
    }

    float xadv0, xadv1, yadv0, yadv1;

    int rrad0 = w/2 - 2;
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

    if(instr)
        instr->setMyColor(g, .2f);
    else
        parent->setMyColor(g, .2f);

    setc(g, .9f);

    int kx1 = (float)RoundFloat(x + w/2 + xadv1);
    int ky1 = (float)RoundFloat(y + h/2 - yadv1);
    int kx2 = (float)RoundFloat(x + w/2 + xadv0);
    int ky2 = (float)RoundFloat(y + h/2 - yadv0);

    gDoubleLine(g, kx1, ky1, kx2, ky2, 2);
}

void Knob::drawSelf(Graphics& g)
{
    //fill(g, .32f);

    if (!knobMode)
    {
        fill(g, .18f);

        instr = NULL;

        drawSlider(g);

        drawText(g);
    }

    // Knob

    if (knobMode)
    {
        drawKnob(g);
    }
}



