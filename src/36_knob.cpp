
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
    param = par;

    angleRange = PI * 1.9f;

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
        positionAngle = float((1 - param->getNormalizedValue()) * angleRange);
    }
    else
    {
        positionAngle = 0;
    }

    redraw();
}

void Knob::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    positionAngle -= (float)((PI / 32) * delta);

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

void Knob::mapObjects()
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

    //gSetMonoColor(g, .5f, 1);
    //gPie(g, x1 + 1, y1+1, width-2, height-2, PI - angleOffset, PI + angleOffset);

    //gSetMonoColor(g, .3f);
    //gEllipseFill(g, x1 + 3, y1 + 3, width-6, height-6);

    //float s = 1.f/2*PI*((width-2)/2);

    gSetMonoColor(g, .3f);

    //gPie(g, x1+1, y1+1, width-2, height-2, PI + angleOffset, 3*PI - angleOffset);
    gPie(g, x1+1, y1+1, width-2, height-2, PI, 3*PI);


    gSetMonoColor(g, .4f);

    gEllipseFill(g, x1 + 3, y1 + 3, width-6, height-6);

    if(param->getOffset() < 0)
    {
        float o = param->getOffset() / param->getRange();
        float oa = abs(o*angleRange);

        float rad = float(width-2)/2;
        float singleAngle = 1.f/(2*PI);
        float ratio = 1.f/(2*PI*rad);
        float singlePixelAngle = ratio*2*PI;

        gSetMonoColor(g, .5f);

        //gPie(g, x1+1, y1+1, width-2, height-2, PI + angleOffset + oa, PI + angleOffset + oa);
    }

    //gEllipseFill(g, x1, y1, width, height);

    float xadv0, xadv1, yadv0, yadv1;
    int rrad0 = width/2 - 3;
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

    int kx1 = (float)RoundFloat(x1 + width/2 + xadv1);
    int ky1 = (float)RoundFloat(y1 + height/2 - yadv1);
    int kx2 = (float)RoundFloat(x1 + width/2 + xadv0);
    int ky2 = (float)RoundFloat(y1 + height/2 - yadv0);

    gDoubleLine(g, kx1, ky1, kx2, ky2, 2);
}



