


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




void NumBox::drawSelf(Graphics& g)
{
    fill(g, .2f);

    rect(g, 0.22f);
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

    int tw = gGetTextWidth(FontBig, str);

    gText(g, FontBig, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
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

    int tw = gGetTextWidth(FontBig, str);

    gText(g, FontBig, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
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
        tpbVal -= count;
    else
        bpbVal -= count;

    LIMIT(tpbVal, 1, 8);
    LIMIT(bpbVal, 1, 8);

    redraw();
}

void MeterBox::updAfterDrag()
{
    if(xDrag < width*0.55f)
        MTransp->setTicksPerBeat(tpbVal);
    else
        MTransp->setBeatsPerBar(bpbVal);
}


OctaveBox::OctaveBox(int val)
{
    value = val;

    setHint("Octave (upper row)");
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

void OctaveBox::drawSelf(Graphics& g)
{
    NumBox::drawSelf(g);

    setc(g, 1.f);

    std::string str = String::formatted(T("%d"), value);

    int tw = gGetTextWidth(FontBig, str);

    gText(g, FontBig, str, x1 + width/2 - tw/2, y2 - height/2 + 7);
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

    gText(g, FontBig, str, x1 + 7, y2 - height/2 + 7);

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

    th = gGetTextHeight(FontSmall);

    tw1 = gGetTextWidth(FontSmall, param->getName());
    tw2 = gGetTextWidth(FontSmall, param->getMaxValString());
    tw3 = gGetTextWidth(FontSmall, param->getUnitStr());

    height = th + 2;
    width = tw1 + tw2 + tw3;
    width += 20;
}

void ParamBox::updValue()
{
    if (param != NULL)
    {
        param->setValueFromControl(this, value);
    }
}

void ParamBox::handleNumDrag(int dragCount)
{
    param->adjustValue(-(float)dragCount/50);

    redraw();
}

void ParamBox::handleMouseWheel(InputEvent & ev)
{
/*
    if(ev.mouseX - x1 < width*0.55f)
    {
        value += ev.wheelDelta;
    }
    else
    {
        value += float(ev.wheelDelta*0.01f);
    }*/

    param->adjustValue((float)ev.wheelDelta/50);

    redraw();
}

void ParamBox::redraw()
{
    int a = 1;
    Gobj::redraw();
}

void ParamBox::drawSelf(Graphics& g)
{
    fill(g, 0.16f);

    int txy = y2 - height/2;

    setc(g, 0.7f);

    gTextFit(g, FontSmall, param->getName(), x1 + 2, txy + 2, width/2);

    std::string valstr = param->getValString();

    int sub = 0;

    if(valstr.data()[0] == '-' || 
       valstr.data()[0] == '+' ||
       valstr.data()[0] == '<')
    {
        int poffs = gGetTextWidth(FontSmall, valstr.substr(0, 1));
        gText(g, FontSmall, param->getValString().substr(0, 1), x1 + width/2 - poffs, txy + 2);
        sub = 1;
    }

    setc(g, .9f);

    //gText(g, FontSmall, param->getSignStr(), x1 + tx2, txy);
    //gText(g, FontVis, param->getValString(), x1 + tx2 - offs, txy - 3);
    gText(g, FontSmall, param->getValString().substr(sub), x1 + width/2, txy + 2);

    setc(g, .7f);

    //gText(g, FontSmall, param->getUnitStr(), x1 + tx3, txy - 2);
    gText(g, FontSmall, param->getUnitStr(), x2 - tw3 - 1, txy + 3);

    float offs = param->getOffset();
    float range = param->getRange();
    float val = param->getValue();

    int xoffs = int(float(width)*((0.f - offs)/range));
    int xval = int(float(width)*(val - offs)/range);

    int xstart = xoffs;
    int xend = xval;

    if(xval < xoffs)
    {
        xstart = xval;
        xend = xoffs;
    }

    int w = xend - xstart;

    //float nval = param->getNormalizedValue();
    //int w = 1 + int((width - 1)*nval);

    setc(g, 0xffC0C000);
    fillx(g, xoffs, height - 3, 1, 3);

    fillx(g, xstart, height - 1, w, 1);

    setc(g, 0xff606000);
    fillx(g, xstart, height - 2, w, 1);

    setc(g, 0xff303000);
    fillx(g, xstart, height - 3, w, 1);
}


