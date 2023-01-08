


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
    gSetMonoColor(g, 0.4f);
    gFillRect(g, x1, y1, x2, y2);

    gSetMonoColor(g, 0.22f);
    gDrawRect(g, x1, y1, x2, y2);
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

    ySet += yDelta;

    if(ev.mouseY < 11)
    {
        ySet = CtrlPanelHeight + 20;

        window->setMousePosition(ev.mouseX, DAW_MAINWINDOW_TITLEBAR_HEIGHT + ySet);
    }
    else if(ev.mouseY > CtrlPanelHeight + 20)
    {
        ySet = 11;

        window->setMousePosition(ev.mouseX, DAW_MAINWINDOW_TITLEBAR_HEIGHT + ySet);
    }

    count += abs(yDelta);

    if (count > 5)
    {
        handleNumDrag(yDelta > 0 ? count/6 : -(count/6));

        count = 0;
    }
}

void NumBox::handleMouseUp(InputEvent& ev)
{
    window->setMousePosition(xOld, DAW_MAINWINDOW_TITLEBAR_HEIGHT + yOld);

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

    gSetMonoColor(g, 1);

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

    gSetMonoColor(g, 1);

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

    gSetMonoColor(g, 1);

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
    gSetMonoColor(g, 0.44f);
    gFillRect(g, x1, y1, x2, y2);

    gSetMonoColor(g, 0.22f);
    gDrawRect(g, x1, y1, x2, y2);

    gSetMonoColor(g, 1);

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

    calcTextCoords();
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

void ParamBox::calcTextCoords()
{
    tw1 = gGetTextWidth(FontSmall, param->getName());
    tw3 = gGetTextWidth(FontSmall, param->getMaxValString());
    tw4 = gGetTextWidth(FontSmall, param->getUnitStr());

    tx1 = 2;
    tx2 = tx1 + tw1 + 6;
    tx3 = tx2 + 6;
    tx4 = tx3 + tw3 + 4;

    width = tx4 + tw4 + 4;
}

void ParamBox::getTextCoords(int* txp1, int* txp2, int* txp3, int* txp4)
{
   *txp1 = tx1;
   *txp2 = tx2;
   *txp3 = tx3;
   *txp4 = tx4;
}

void ParamBox::setTextCoords(int txs1, int txs2, int txs3, int txs4)
{
    tx1 = txs1;
    tx2 = txs2;
    tx3 = txs3;
    tx4 = txs4;

    if (txs2 > tx2) tx2 = txs2;
    if (txs3 > tx3) tx3 = txs3;
    if (txs4 > tx4) tx4 = txs4;

    width = tx4 + tw4 + 4;
}

void ParamBox::adjustTx2(int txmin2)
{
    tx2 = txmin2;

    tx3 = tx2 + 6;
    tx4 = tx3 + tw3 + 4;

    width = tx4 + tw4 + 4;
}

void ParamBox::adjustTx3(int txmin3)
{
    tx3 = txmin3;
    tx4 = tx3 + tw3 + 4;

    width = tx4 + tw4 + 4;
}

void ParamBox::adjustTx4(int txmin4)
{
    tx4 = txmin4;

    width = tx4 + tw4 + 4;
}

void ParamBox::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.22f);
    gFillRect(g, x1, y1, x2, y2);

    //gSetMonoColor(g, 0.4f);
    //gDrawRect(g, x1, y1, x2, y2);

    int txy = y2 - height/2 + th/2 - 2;

    gSetMonoColor(g, 0.6f);
    gText(g, FontSmall, param->getName(), x1 + tx1, txy);

    gSetMonoColor(g, 0.99f);
    gText(g, FontSmall, param->getSignStr(), x1 + tx2, txy);
    gText(g, FontSmall, param->getValString(), x1 + tx3, txy);

    gSetMonoColor(g, 0.6f);
    gText(g, FontSmall, param->getUnitStr(), x1 + tx4, txy);
}



