


#include "36_numbox.h"
#include "36_button.h"
#include "36_grid.h"
#include "36_draw.h"
#include "36_transport.h"
#include "36_system.h"
#include "36_window.h"
#include "36_text.h"
#include "36_macros.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_device.h"
#include "36_instr.h"


NumBox::NumBox()
{
    fontId = FontBig;
}

void NumBox::drawSelf(Graphics& g)
{
    fill(g, .2f);

    //rect(g, 0.22f);
}

void NumBox::handleMouseDown(InputEvent& ev)
{
    ySet = ev.mouseY;

    count = 0;

    xDrag = ev.mouseX - x1;

    xOld = ev.mouseX;
    yOld = ev.mouseY;
}

void NumBox::handleMouseDrag(InputEvent& ev)
{
    window->setCursor(Cursor_Null);

    int yDelta = ev.mouseY - ySet;

    if(ev.mouseY < 0)
    {
        ev.mouseY = window->getH() - 1;
        window->setMousePosition(ev.mouseX, ev.mouseY);
    }
    else if(ev.mouseY >= window->getH())
    {
        ev.mouseY = 0;
        window->setMousePosition(ev.mouseX, ev.mouseY);
    }

    ySet = ev.mouseY;

    count += abs(yDelta);

    if (count > 5)
    {
        handleNumDrag(yDelta > 0 ? count/6 : -(count/6));

        count = 0;
    }
}

void NumBox::handleMouseUp(InputEvent& ev)
{
    window->setMousePosition(xOld, yOld);

    window->setCursor(Cursor_Arrow);

    updAfterDrag();
}


BpmBox::BpmBox(float val)
{
    value = val;

    setHint("BPM");
}

void BpmBox::drawSelf(Graphics& g)
{
    NumBox::drawSelf(g);

    setc(g, 1.f);

    std::string str;

    if(value >= 100)
        str = String::formatted(T("%.2f"), value);
    else
        str = String::formatted(T("0%.2f"), value);

    int tw = gGetTextWidth(fontId, str);

    gText(g, fontId, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
}

void BpmBox::handleNumDrag(int dragCount)
{
    float tmpVal = value;
    
    if(xDrag < width*0.55f)
        tmpVal -= dragCount;
    else
        tmpVal -= (float)dragCount/100;

    LIMIT(tmpVal, 10, 999);

    if(tmpVal != value)
    {
        value = tmpVal;

        redraw();
    }
}

void BpmBox::updAfterDrag()
{
    MTransp->setBeatsPerMinute(value);
}

void BpmBox::handleMouseWheel(InputEvent & ev)
{
    if(ev.mouseX - x1 < width*0.55f)
    {
        value += ev.wheelDelta;
    }
    else
    {
        value += float(ev.wheelDelta*0.01f);
    }

    LIMIT(value, 10, 999);

    MTransp->setBeatsPerMinute(value);

    redraw();
}


MeterBox::MeterBox(int tpb, int bpb)
{
    tpbVal = tpb;
    bpbVal = bpb;

    setHint("Meter (Ticks Per Beat / Beats Per Bar)");
}

void MeterBox::drawSelf(Graphics& g)
{
    NumBox::drawSelf(g);

    setc(g, 1.f);

    std::string str = String::formatted(T("%d / %d"), tpbVal, bpbVal);

    int tw = gGetTextWidth(fontId, str);

    gText(g, fontId, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
}

void MeterBox::handleMouseWheel(InputEvent & ev)
{
    if(ev.mouseX - x1 < width/2)
    {
        tpbVal += ev.wheelDelta;

        LIMIT(tpbVal, 1, 8);

        MTransp->setTicksPerBeat(tpbVal);
    }
    else
    {
        bpbVal += ev.wheelDelta;

        LIMIT(bpbVal, 1, 8);

        MTransp->setBeatsPerBar(bpbVal);
    }

    redraw();
}

void MeterBox::handleNumDrag(int count)
{
    if(xDrag < width*0.55f)
    {
        tpbVal -= count;
    }
    else
    {
        bpbVal -= count;
    }

    LIMIT(tpbVal, 1, 8);
    LIMIT(bpbVal, 1, 8);

    redraw();
}

void MeterBox::updAfterDrag()
{
    if(xDrag < width*0.55f)
    {
        MTransp->setTicksPerBeat(tpbVal);
    }
    else
    {
        MTransp->setBeatsPerBar(bpbVal);
    }
}


OctaveBox::OctaveBox(int val)
{
    value = val;

    setHint("Octave (upper row)");
}

void OctaveBox::drawSelf(Graphics& g)
{
    NumBox::drawSelf(g);

    setc(g, 1.f);

    std::string str = String::formatted(T("%d"), value);

    int tw = gGetTextWidth(fontId, str);

    gText(g, fontId, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
}

void OctaveBox::handleNumDrag(int count)
{
    value -= count;

    LIMIT(value, 1, 8);

    redraw();
}

void OctaveBox::handleMouseWheel(InputEvent & ev)
{
    value += ev.wheelDelta;

    LIMIT(value, 1, 8);

    redraw();
}



TimeScreen::TimeScreen()
{
    ///
}

void TimeScreen::drawSelf(Graphics& g)
{
    fill(g, 0.44f);

    rect(g, 0.22f);

    setc(g, 1.f);

    std::string str = String::formatted(T("%2d:%02d"), MTransp->getCurrMin(), MTransp->getCurrSec());

    gText(g, fontId, str, x1 + 7, y2 - height/2 + 7);

    std::string str1 = String::formatted(T(":%03d"), MTransp->getCurrMs());

    gText(g, FontFix, str1, x1 + 50, y2 - height/2 + 6);

    //g.setColour(Colour(0x9060FFFF));
    //gText(g, FontFix, String::formatted(T("%02dm %02ds %03d ms"), _MClock->getCurrMin(), _MClock->getCurrSec(), _MClock->getCurrMs()), x1 + 9, y2 - 7);

    //int w = gGetTextWidth(FontSmall, (const char*)String::formatted(T("%d:%d"), _MClock->getCurrMin(), _MClock->getCurrSec()));
    //g.setColour(Colour(0xc090EAF0));
    //gText(g, FontSmall, String::formatted(T(":%2d"), _MClock->getCurrMs()), x1 + TimerTxtX + w, y1 + TimerTxtY + 16);
}

void TimeScreen::handleNumDrag(int count)
{
    int val = -count;

    MTransp->adjustTime(0, val, 0);

    redraw();
}

void TimeScreen::handleMouseWheel(InputEvent & ev)
{
    int val = ev.wheelDelta;

    MTransp->adjustTime(0, val, 0);

    redraw();
}


ParamBox::ParamBox(Parameter* param)
{
    addParam(param);

    prm = param;

    setFontId(FontSmall);

    tw1 = gGetTextWidth(fontId, prm->getName());
    tw2 = gGetTextWidth(fontId, prm->getMaxValString());
    tw3 = gGetTextWidth(fontId, prm->getUnitString());

    sliderOnly = false;

    height = textHeight + 2;
    width = tw1 + tw2 + tw3;
    width += 20;
}

void ParamBox::drawSelf(Graphics& g)
{
    Instrument* i = dynamic_cast<Instrument*>(parent);

    if (i) 
        i->setMyColor(g, .3f);
    else
        setc(g, 0.3f);

    fillx(g, 0, 0, width, height);

    int txy = 0;

    //setc(g, 0.2f);
    //fillx(g, 0, txy, width, height - txy);

    float offs = prm->getOffset();
    float range = prm->getRange();
    float val = prm->getValue();
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

    int sh = height - txy;

    setc(g, 0.f);
    fillx(g, defPos, height - sh, 1, sh);

    //setc(g, 0xffB0B000);
    if (i) 
        i->setMyColor(g, .8f);
    else
        setc(g, 0.6f);
    fillx(g, xstart, height - sh, w, sh);

    if (i) 
        i->setMyColor(g, .6f);
    else
        setc(g, 0.5f);
    //setc(g, 0xffA0A060);
    fillx(g, xstart, height - sh+1, w, sh-1);

    if (i) 
        i->setMyColor(g, .4f);
    else
        setc(g, 0.4f);
    //setc(g, 0xff505030);
    fillx(g, xoffs, height - sh, 1, sh);

    if (i) 
        i->setMyColor(g, .99f);
    else
        setc(g, 0.99f);
    //setc(g, 0xffFFFFA0);
    fillx(g, xval, height - sh, 1, sh);

    if (sliderOnly == false)
    {
        txy = textHeight;

        int txty = txy - 3;

        //setc(g, 0.8f);
        setc(g, 1.f);

        txtfit(g, fontId, prm->getName(), 3, txty, width/2);

        setc(g, 1.f);

        std::string valstr = prm->getValString();

        int sub = 0;

        if(valstr.data()[0] == '-' || 
           valstr.data()[0] == '+' ||
           valstr.data()[0] == '<')
        {
            int poffs = gGetTextWidth(fontId, valstr.substr(0, 1));
            txt(g, fontId, prm->getValString().substr(0, 1), width/2 - poffs, txty);
            sub = 1;
        }

        setc(g, 1.f);

        txt(g, fontId, prm->getValString().substr(sub), width/2, txty);

        setc(g, .9f);

        txt(g, fontId, prm->getUnitString(), width - tw3 - 2, txty);
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
        return prm->getName() + ":  " + prm->getValString() + " " + prm->getUnitString();
    }
    else
    {
        return "";
    }
}

void ParamBox::handleNumDrag(int dragCount)
{
    prm->adjustFromControl(this, -(float)dragCount);

    redraw();
}

void ParamBox::handleMouseDown(InputEvent & ev)
{
    if (abs(defPos - (ev.mouseX - x1)) < 2)
    {
        prm->setValue(prm->getDefaultValue());
    }
    else
    {
        prm->adjustFromControl(this, 0, float(ev.mouseX - x1)/width);
    }

    redraw();
}

void ParamBox::handleMouseDrag(InputEvent & ev)
{
    if (abs(defPos - (ev.mouseX - x1)) < 2)
    {
        prm->setValue(prm->getDefaultValue());
    }
    else
    {
        prm->adjustFromControl(this, 0, float(ev.mouseX - x1)/width);
    }

    redraw();
}

void ParamBox::handleMouseUp(InputEvent & ev)
{
    
}

void ParamBox::handleMouseWheel(InputEvent & ev)
{
    prm->adjustFromControl(this, (float)ev.wheelDelta);

    redraw();
}

void ParamBox::remap()
{
    defPos = int(float(width-1)*prm->getDefaultValueNormalized());
}

void ParamBox::redraw()
{
    Gobj::redraw();
}

ToggleBox::ToggleBox(ParamToggle* param_tg)
{
    prmToggle = param_tg;

    setFontId(FontSmall);

    height = textHeight + 4;
}

void ToggleBox::drawSelf(Graphics& g)
{
    fill(g, 0.3f);

    if (prmToggle->getValue())
    {
        setc(g, 0.8f);
        fillx(g, 0, 0, height, height);
    }
    else
    {
        setc(g, 0.45f);
        fillx(g, 0, 0, height, height);
    }

//    setc(g, 0.4f);
//    rectx(g, width - width/4, 0, height, height);

    setc(g, .8f);
    txtfit(g, fontId, prmToggle->getName(), height + 6 /*(width - width/4)/2 - gGetTextWidth(fontId, prmToggle->getName())/2*/, height/2 + 4, width - height);
}

void ToggleBox::handleMouseDown(InputEvent & ev)
{
    prmToggle->toggle();

    redraw();
}

void ToggleBox::handleMouseUp(InputEvent & ev)
{
}

RadioBox::RadioBox(ParamRadio* param_radio)
{
    prmRad = param_radio;

    setFontId(FontSmall);

    height = (textHeight + 4)*prmRad->getNumOptions();
}

void RadioBox::drawSelf(Graphics& g)
{
    fill(g, 0.3f);

    int h1 = (textHeight + 4);
    int y = 0;
    int opt = 0;

    for (std::string str : prmRad->getOptions())
    {
        if (prmRad->getCurrent() == opt)
        {
            setc(g, 0.8f);
            fillx(g, 2, y + 2, textHeight, textHeight);
        }
        else
        {
            setc(g, 0.4f);
            fillx(g, 2, y + 2, textHeight, textHeight);
        }

        setc(g, 1.f);
        txtfit(g, fontId, str, (textHeight + 4) + 6, y + textHeight - 1, width - h1);

        y += textHeight + 4;
        opt++;
    }
}

void RadioBox::handleMouseDown(InputEvent & ev)
{
    prmRad->setCurrent((ev.mouseY - y1) / (textHeight + 4));

    redraw();
}

SelectorBox::SelectorBox(ParamSelector* param_sel)
{
    prmSelector = param_sel;

    setFontId(FontSmall);

    height = (textHeight + 4)*prmSelector->getNumOptions();
}

void SelectorBox::drawSelf(Graphics& g)
{
    fill(g, 0.3f);

    int h1 = (textHeight + 4);
    int y = 0;
    int opt = 0;

    for (std::string str : prmSelector->getOptions())
    {
        if (prmSelector->getValue(opt))
        {
            setc(g, 0.8f);
            fillx(g, 2, y + 2, textHeight, textHeight);
        }
        else
        {
            setc(g, 0.4f);
            fillx(g, 2, y + 2, textHeight, textHeight);
        }

        setc(g, 1.f);
        txtfit(g, fontId, str, (textHeight + 4) + 6, y + textHeight - 1, width - h1);

        y += textHeight + 4;
        opt++;
    }
}

void SelectorBox::handleMouseDown(InputEvent & ev)
{
    prmSelector->toggleValue((ev.mouseY - y1) / (textHeight + 4));

    redraw();
}


