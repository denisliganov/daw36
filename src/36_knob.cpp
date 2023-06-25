
#include "rosic/rosic.h"

#include "36_knob.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_macros.h"
#include "36_window.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_mixchannel.h"
#include "36_effects.h"



ParamBox::ParamBox()
{
    valueReplace = false;
    hasText = true;
    textInside = true;
    valueReplace = false;
    widthDivider = 1;
    hoverOption = -1;
}

void ParamBox::setTextParams(bool txt, bool inside, bool value_replace, float width_divider)
{
    hasText = txt;

    textInside = inside;

    valueReplace = value_replace;

    widthDivider = (hasText && !textInside ? width_divider : 1);

    remap();
}

void ParamBox::drawText(Graphics& g)
{
    int txtAreaX = width*widthDivider;
    int txtAreaWidth = textInside ? width : width - width*widthDivider - 6;

    int textX = textInside ? 4 : txtAreaX + 6;
    int textY = 10;

    int namestrLen = gGetTextWidth(fontId, param->getName());
    int unitstrLen = gGetTextWidth(fontId, param->getUnitString());
    int valstrLen = gGetTextWidth(fontId, param->getValString());


    if (!textInside)
    {
        setc(g, .3f);
        fillx(g, txtAreaX, 0, width - txtAreaX, height);

        if (valueReplace )
        {
            if (isUnderMouse())
            {
                setc(g, .9f);
                std::string str = param->getValString() + " " + param->getUnitString();

                if (hoverOption >= 0 && (param->getType() == Param_Selector || param->getType() == Param_Radio))
                {
                    setc(g, .8f);

                    str = param->getOptionStr(hoverOption);

                    if (hoverOption == param->getCurrentOption())
                    {
                        setc(g, 1.f);
                    }
                }

                txt(g, fontId, str, textX, textY);
            }
            else
            {
                setc(g, .8f);
                txt(g, fontId, param->getName(), textX, textY);
            }
        }
        else
        {
            setc(g, .8f);
            txt(g, fontId, param->getName(), textX, textY);
        
            setc(g, 1.f);
            txt(g, fontId, param->getValString() + " " + param->getUnitString(), textX + txtAreaWidth*0.6, textY);
        }
    }
    else
    {
        setc(g, .8f);
        txt(g, fontId, param->getName(), textX, textY);

        setc(g, .9f);
        txt(g, fontId, param->getValString(), textX + txtAreaWidth - valstrLen - 2, textY);

        setc(g, .52f);
        txt(g, fontId, param->getUnitString(), textX + txtAreaWidth - unitstrLen - 2, textY);
    }
}

void ParamBox::remap()
{
    MixChannel* mchan = dynamic_cast<MixChannel*>(parent);
    eff = dynamic_cast<Eff*>(parent);
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
        instr = mchan->getInstr();
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


    //instr = NULL;
}


Knob::Knob(Parameter* par, bool knob, bool vert)
{
    setFontId(FontSmall);
    
    setParam(par);

    vertical = vert;
    knobMode = knob;
    angleRange = PI * 1.5f;
    angleOffset = float(2*PI - angleRange)*.5f;
    defaultPos = 0;
    savedHeight = 0;
    hoverOption = 0;
    dim = false;
    dimOnZero = false;
    instr = NULL;
    eff = NULL;
    sliding = false;

    setTextParams(true, false);

    updPosition();
}

std::string Knob::getClickHint()
{
    return param->getName() + ":  " + param->getValString() + " " + param->getUnitString();
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

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Knob::handleSliding(InputEvent& ev)
{
    float len = vertical ? float(height) : float(width) * widthDivider;
    int delta = vertical ? (y2 - ev.mouseY + 1) : (ev.mouseX - x1 + 1);

    if (abs(defaultPos - (delta)) < 2)
    {
        param->setValue(param->getDefaultValue());
    }
    else
    {
        if (ev.clickDown && (delta <= (int)len))
        {
            sliding = true;
        }

        if (sliding)
        {
            param->adjustFromControl(this, 0, float(delta)/len);
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
    else //if (ev.mouseX <= x1 + width*widthDiv)
    {
        handleSliding(ev);
    }

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
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
        if (instr)
        {
            MInstrPanel->setCurrInstr(instr);
        }

        handleSliding(ev);
    }

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Knob::handleMouseUp(InputEvent & ev)
{
    sliding = false;

    if (ev.mouseX > x1 + width*widthDivider)
    {
        //parent->handleMouseUp(ev);
    }

    if (parent != NULL)
    {
        parent->handleChildEvent(this, ev);
    }
}

void Knob::remap()
{
    ParamBox::remap();

    if (param)
    {
        if (!vertical)
            defaultPos = int(float(width*widthDivider)*param->getDefaultValueNormalized());
        else
            defaultPos = int(float(height) * param->getDefaultValueNormalized());
    }

    delSnap();
}

void Knob::drawHorizontalSlider(Graphics& g)
{
    int w = width*widthDivider;
    int h = height;
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

    int ws = xend - xstart;
    int hs = h;
    int ys = h - hs;

    // Black notch for default (initial) value
    //setc(g, 0.f);
    //fillx(g, defPos, height - hs, 1, hs);

    if (0 && instr)
        instr->setMyColor(g, .1f);
    else
        setc(g, .1f);

    rectx(g, 0, 0, w, h);

    if (0 && instr)
        instr->setMyColor(g, .2f);
    else
        setc(g, .2f);

    fillx(g, 0, 0, w, h);

    if (0 && instr)
        instr->setMyColor(g, .6f, .42f);
    else
        setc(g, .4f);

    fillx(g, xstart, ys + 1, ws, hs - 2);


    if (0 && instr)
        instr->setMyColor(g, .7f);
    else
        setc(g, .6f);

    fillx(g, xoffs, ys + 1, 1, hs - 2);
}

void Knob::drawVerticalSlider(Graphics& g)
{
    int h = height;
    int w = width;
    float offs = param->getOffset();
    float range = param->getRange();
    float val = param->getValue();
    float def = param->getDefaultValue();
    float baseVal = (offs <= 0 ? 0 : offs);

    int yoffs = int(float(h-1)*((baseVal - offs)/range));
    int yval = int(float(h-1)*((val - offs)/range));
    int ydef = int(float(h-1)*((def - offs)/range));

    int ystart = yoffs;
    int yend = yval;

    if(yval < yoffs)
    {
        ystart = yval;
        yend = yoffs;
    }

    int slen = yend - ystart;
    int ws = w;
    int xs = w - ws;

    if (instr)
        instr->setMyColor(g, .1f);
    else
        setc(g, .1f);

    rectx(g, 0, 0, w, h);

    if (instr)
        instr->setMyColor(g, .34f);
    else
        setc(g, .34f);

    fillx(g, 0, 0, w, h);

    if (instr)
        instr->setMyColor(g, .8f, .42f);
    else
        setc(g, .4f);

    fillx(g, xs + 1, h - ystart - slen, ws - 2, slen);

    if (instr)
        instr->setMyColor(g, .4f);
    else
        setc(g, .4f);

    fillx(g, xs + 1, h - yoffs - 1, ws - 2, 1);
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
            clr = Colour(1.f, 0.f, dim ? .2f : .8f, 1.f);
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
        fill(g, .18f);

        if (vertical)
        {
            drawVerticalSlider(g);
        }
        else
        {
            drawHorizontalSlider(g);
        }

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

ContextMenu* Knob::createContextMenu()
{
    int x = window->getLastEvent().mouseX - x1;

    if (x > (width*widthDivider))
        return parent->createContextMenu();
    else
        return NULL;
}







SelectorBox::SelectorBox(Parameter* param_sel)
{
    setParam(param_sel);

    radioMode = (param_sel->getType() == Param_Radio);

    setFontId(FontSmall);

    setTextParams(true, false);
}

void SelectorBox::drawSelf(Graphics& g)
{
    int x = 0;
    int opt = 0;

    for (std::string str : param->getAllOptions())
    {
        setc(g, 0.2f);
        fillx(g, x, 0, itemWidth, height);

        if (radioMode && param->getCurrentOption() == opt || !radioMode && param->getOptionVal(opt))
        {
            setc(g, 0.8f);
        }

        fillx(g, x + 1, 1, itemWidth - 2, height - 2);

        x += itemWidth + 1;

        opt++;
    }

    if (hasText)
    {
        drawText(g);
    }
}

void SelectorBox::remap()
{
    ParamBox::remap();

    itemWidth = int(float(width*widthDivider)/param->getNumOptions());
}

void SelectorBox::handleMouseDown(InputEvent & ev)
{
    int x = ev.mouseX - x1;

    if (param->getNumOptions() == 1)
    {
        //if (x <= (width*widthDiv))
        {
            param->toggleValue();
        }

        redraw();
    }
    else
    {
        if (x < (width*widthDivider))
        {
            int option = x / itemWidth;

            if (option >= param->getNumOptions())
            {
                option = param->getNumOptions() - 1;
            }

            if (radioMode)
            {
                param->setCurrentOption(option);
            }
            else
            {
                param->toggleOption(option);
            }

            redraw();
        }
    }
}


ContextMenu* SelectorBox::createContextMenu()
{
    int x = window->getLastEvent().mouseX - x1;

    if (x > (width*widthDivider))
        return parent->createContextMenu();
    else
        return NULL;
}

void SelectorBox::handleMouseDrag(InputEvent& ev)
{
    if (!active)
        return;

    if (eff)
    {
        //parent->handleMouseDrag(ev);
    }
}


void SelectorBox::handleMouseMove(InputEvent & ev)
{
    hoverOption = -1;

    if ((param->getType() == Param_Selector || param->getType() == Param_Radio) && widthDivider < 1)
    {
        int x = ev.mouseX - x1;

        if (x < (width*widthDivider))
        {
            hoverOption = x / itemWidth;
            redraw();
        }
    }
}

