


#include "36_parambox.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_instr.h"



ParamBox::ParamBox(Parameter* par)
{
    param = par;

    setParam(param);

    setFontId(FontSmall);

    tw1 = gGetTextWidth(fontId, param->getName());
    tw2 = gGetTextWidth(fontId, param->getMaxValString());
    tw3 = gGetTextWidth(fontId, param->getUnitString());

    sliderOnly = false;

    height = textHeight + 2;
    width = tw1 + tw2 + tw3;
    width += 20;
}

void ParamBox::drawSlider(Graphics& g)
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
    int sh = height - (sliderOnly ? 0 : textHeight + 1);

    // Black notch for default (initial) value
    //setc(g, 0.f);
    //fillx(g, defPos, height - sh, 1, sh);

    setc(g, 0.4f);
    fillx(g, xstart, height - sh, w, sh);

    setc(g, 0.6f);
    fillx(g, xstart, height - sh+1, w, sh-2);

    //drawGlassRect(g, x1 + (float)xstart, y1 + (float)(height - sh+1), w, sh-1, Colour(180, 120, 120), 0, 0, true, true, true, true);

    setc(g, 0.2f);
    fillx(g, xoffs, height - sh, 1, sh);

    setc(g, 1.f);
    fillx(g, xval, height - sh + 1, 1, sh - 2);
}

void ParamBox::drawText(Graphics& g)
{
    int txty = textHeight - 3;

    //setc(g, 0.8f);
    setc(g, 1.f);

    txtfit(g, fontId, param->getName(), 3, txty, width/2);

    setc(g, 1.f);

    std::string valstr = param->getValString();

    int sub = 0;

    if(valstr.data()[0] == '-' || 
       valstr.data()[0] == '+' ||
       valstr.data()[0] == '<')
    {
        int poffs = gGetTextWidth(fontId, valstr.substr(0, 1));

        txt(g, fontId, param->getValString().substr(0, 1), width/2 - poffs, txty);

        sub = 1;
    }

    setc(g, 1.f);

    txt(g, fontId, param->getValString().substr(sub), width/2, txty);
    txt(g, fontId, param->getUnitString(), width - tw3 - 2, txty);
}


void ParamBox::drawSelf(Graphics& g)
{
    setc(g, 0.25f);
    fillx(g, 0, 0, width, height);

    drawSlider(g);

    if (sliderOnly == false)
    {
        drawText(g);
    }
}

float ParamBox::getMinStep()
{
    return 1.f/width;
}

std::string ParamBox::getClickHint()
{
    if (sliderOnly)
    {
        return param->getName() + ":  " + param->getValString() + " " + param->getUnitString();
    }
    else
    {
        return "";
    }
}

void ParamBox::handleNumDrag(int dragCount)
{
    param->adjustFromControl(this, -(float)dragCount);

    redraw();
}

void ParamBox::handleMouseDown(InputEvent & ev)
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

void ParamBox::handleMouseDrag(InputEvent & ev)
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

void ParamBox::handleMouseUp(InputEvent & ev)
{
    
}

void ParamBox::handleMouseWheel(InputEvent & ev)
{
    param->adjustFromControl(this, (float)ev.wheelDelta);

    redraw();
}

void ParamBox::remap()
{
    defPos = int(float(width-1)*param->getDefaultValueNormalized());
}

void ParamBox::redraw()
{
    Gobj::redraw();
}

ToggleBox::ToggleBox(Parameter* param_tg)
{
    setParam(param_tg);

    setFontId(FontSmall);

    height = textHeight + 4;
}

void ToggleBox::drawSelf(Graphics& g)
{
    int d = height/4;
    
    setc(g, 0.18f);
    fillx(g, d, d, height-d*2, height-d*2);

    if (param->getBoolValue())
    {
        setc(g, 0.6f);
        fillx(g, d+1, d+1, height-d*2 - 2, height-d*2 - 2);
    }

//    setc(g, 0.4f);
//    rectx(g, width - width/4, 0, height, height);

    //setc(g, .74f);
    //txt(g, FontSmall, param->getName(), height + 6 /*(width - width/4)/2 - gGetTextWidth(fontId, prmToggle->getName())/2*/, height/2 + 4);
}

void ToggleBox::handleMouseDown(InputEvent & ev)
{
    param->toggleValue();

    redraw();
}

void ToggleBox::handleMouseUp(InputEvent & ev)
{
}




RadioBox::RadioBox(Parameter* param_radio, int initHeight)
{
    param = param_radio;

    setFontId(FontSmall);

    hLine = initHeight/param->getNumOptions() - 1;

    if (hLine < 1)
    {
        hLine = 1;
    }

    height = (hLine + 1)*param->getNumOptions();
}

void RadioBox::drawSelf(Graphics& g)
{
    int y = 0;
    int opt = 0;

    for (std::string str : param->getAllOptions())
    {
        if (param->getCurrentOption() == opt)
        {
            setc(g, 0.6f);
        }
        else
        {
            setc(g, 0.2f);
        }

        fillx(g, 0, y, width, hLine);

        y += hLine + 1;

        opt++;
    }
}

void RadioBox::handleMouseDown(InputEvent & ev)
{
    param->setCurrentOption((ev.mouseY - y1) / (hLine + 1));

    redraw();
}

SelectorBox::SelectorBox(Parameter* param_sel, int initHeight)
{
    param = param_sel;

    setFontId(FontSmall);

    hLine = initHeight/param->getNumOptions() - 1;

    if (hLine < 1)
    {
        hLine = 1;
    }

    height = (hLine + 1)*param->getNumOptions();
}

void SelectorBox::drawSelf(Graphics& g)
{
    int y = 0;
    int opt = 0;

    for (std::string str : param->getAllOptions())
    {
        if (param->getOptionVal(opt))
        {
            setc(g, 0.6f);
        }
        else
        {
            setc(g, 0.2f);
        }

        fillx(g, 0, y, width, hLine);

        y += hLine + 1;

        opt++;
    }
}

void SelectorBox::handleMouseDown(InputEvent & ev)
{
    param->toggleOption((ev.mouseY - y1) / (hLine + 1));

    redraw();
}


