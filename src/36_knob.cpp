
#include "rosic/rosic.h"

#include "36_knob.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_macros.h"
#include "36_window.h"
#include "36_instr.h"



Knob::Knob(Parameter* par)
{
    setFontId(FontSmall);

    addParam(par);

    angleRange = PI * 1.5f;

    angleOffset = float(2*PI - angleRange)*.5f;

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
//    delete parentImage;

 //   parentImage = NULL;
}

void Knob::drawSelf(Graphics& g)
{
    Instrument* instr = dynamic_cast<Instrument*>(parent);

    fill(g, .28f);
    //fill(g, .32f);

    int textX = 4;  // height + 2
    int namestrLen = gGetTextWidth(fontId, param->getName());
    int unitstrLen = gGetTextWidth(fontId, param->getUnitString());

    setc(g, .6f);
    txt(g, fontId, param->getName() + ": ", textX, 12);
    setc(g, .9f);
    txt(g, fontId, param->getValString(), textX + namestrLen + 4, 12);
    //txt(g, fontId, param->getValString(), textX, height - 5);
    setc(g, .6f);
    txt(g, fontId, param->getUnitString(), width - unitstrLen - 2, 12);

    //setc(g, 1.f);
    //fillx(g, height + 2, height/2, param->getValueNormalized()*(width - height - 2), 1);

    //txt(g, FontSmall, prm->getValString(), 0, height - 2);
    //txt(g, FontSmall, prm->getUnitString(), width - gGetTextWidth(FontSmall, prm->getUnitString()), height - 2);
    //gSetMonoColor(g, .5f, 1);
    //gPie(g, x1 + 1, y1+1, width-2, height-2, PI - angleOffset, PI + angleOffset);
    //gSetMonoColor(g, .3f);
    //gEllipseFill(g, x1 + 3, y1 + 3, width-6, height-6);

    //float s = 1.f/2*PI*((width-2)/2);

    // Knob
    if (false)
    {

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
}



