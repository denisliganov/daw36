
#include "rosic/rosic.h"

#include "36_knob.h"
#include "36_paramnum.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_macros.h"
#include "36_window.h"
#include "36_instr.h"



Knob::Knob(Parameter* par)
{
    setFontId(FontSmall);
    param = par;
    prm = par;
    angleRange = PI * 1.9f;
    angleOffset = float(2*PI - angleRange)*.5f;

    updPosition();
}

void Knob::updValue()
{
    if (prm != NULL)
    {
        prm->adjustFromControl(this, 0, (1 - positionAngle/angleRange));
    }

    redraw();
}

void Knob::updPosition()
{
    if (prm != NULL)
    {
        positionAngle = float((1 - prm->getValueNormalized()) * angleRange);
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

void Knob::remap()
{
    delete parentImage;

    parentImage = NULL;
}

void Knob::drawSelf(Graphics& g)
{
/*
    if(parentImage == NULL)
    {
        parentImage = window->createSnapshot(x1, y1, width, height);
    }
    else
    {
        gDrawImageRegion(g, parentImage, x1, y1, dwidth, dheight);
    }*/

    Instrument* instr = dynamic_cast<Instrument*>(parent);

    fill(g, .3f);

    setc(g, .8f);
    txt(g, fontId, prm->getName(), 2, 10);

    //txt(g, FontSmall, prm->getValString(), 0, height - 2);
    //txt(g, FontSmall, prm->getUnitString(), width - gGetTextWidth(FontSmall, prm->getUnitString()), height - 2);
    //gSetMonoColor(g, .5f, 1);
    //gPie(g, x1 + 1, y1+1, width-2, height-2, PI - angleOffset, PI + angleOffset);
    //gSetMonoColor(g, .3f);
    //gEllipseFill(g, x1 + 3, y1 + 3, width-6, height-6);

    //float s = 1.f/2*PI*((width-2)/2);

    gSetMonoColor(g, .3f);

    int w = width;
    int h = height - headerHeight;

    w = MAX(w, h) - 8;
    h = w;

    int x = x1 + 4;
    int y = y1 + headerHeight + 4;

    gSetMonoColor(g, .4f);

    gEllipseFill(g, x, y, w, h);

    if(prm->getOffset() < 0)
    {
        float o = prm->getOffset() / prm->getRange();
        float oa = abs(o*angleRange);

        float rad = float(w-2)/2;
        float singleAngle = 1.f/(2*PI);
        float ratio = 1.f/(2*PI*rad);
        float singlePixelAngle = ratio*2*PI;
        gSetMonoColor(g, .2f);
        gPie(g, x1+1, y1+1, width-2, height-2, PI + angleOffset + oa, PI + angleOffset + oa);
    }

    //setc(g, .2f);
    //gPie(g, x, y, w, h, PI + angleOffset, 3*PI - angleOffset);
    //gPie(g, x, y, w, h, PI, 3*PI);


    //gEllipseFill(g, x1, y1, width, height);

    float xadv0, xadv1, yadv0, yadv1;
    int rrad0 = w/2 - 3;
    int rrad1 = rrad0/2.f;
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
        instr->setMyColor(g,.9);
    else
        parent->setMyColor(g, .9f);

    int kx1 = (float)RoundFloat(x + w/2 + xadv1);
    int ky1 = (float)RoundFloat(y + h/2 - yadv1);
    int kx2 = (float)RoundFloat(x + w/2 + xadv0);
    int ky2 = (float)RoundFloat(y + h/2 - yadv0);

    gDoubleLine(g, kx1, ky1, kx2, ky2, 2);

    //Gobj::setc(g, 8.f);
    //std::string val = prm->getValString() + " " + prm->getUnitString();
    //txt(g, fontId, val, width/2 - gGetTextWidth(fontId, prm->getValString())/2, height/2 + headerHeight);

    setc(g, 0.35f);
    fillx(g, 0, 0, width, headerHeight);

    setc(g, 0.8f);
    txtfit(g, fontId, prm->getName(), 3, 12, w - 2);

    //setc(g, 0.5f);
    //rectx(g, 0, headerHeight, width, height - headerHeight);
}



