//
//
//

#include <direct.h>

#include "36.h"
#include "36_audio_dev.h"
#include "36_browser.h"
#include "36_button.h"
#include "36_draw.h"
#include "36_effects.h"
#include "36_history.h"
#include "36_menu.h"
#include "36_macros.h"
#include "36_mixchannel.h"
#include "36_project.h"
#include "36_params.h"
#include "36_transport.h"
#include "36_text.h"
#include "36_utils.h"
#include "36_knob.h"
#include "36_dragndrop.h"









class EffEnableButton : public ToggleBox
{
public:

        EffEnableButton(Parameter* ptg) : ToggleBox(ptg)
        {
            ///
        }

        bool    isEnabled()
        {
            return param->getBoolValue();
        }

protected:

        void drawSelf(Graphics& g)
        {
            fill(g, 0xffFFFF40, .4f);

            setc(g, 0xffFFFF40, .2f, 1.f);
            fillx(g, 1, 1, width - 2, height - 2);

            if (param->getBoolValue())
            {
                setc(g, 0xffFFFF40, .5f, 1.f);
                fillx(g, 4, 4, width - 8, height - 8);
                
                setc(g, 0xffFFFF40);
                fillx(g, 5, 5, width - 10, height - 10);
            }
            else
            {
            }

            //if (param->getBoolValue())
            //    drawGlassRound(g, x1+1, y1+1, width-2, Colour(255,255,55), 1);
            //else
            //    drawGlassRound(g, x1+1, y1+1, width-2, Colour(55, 55, 55), 1);
        }

        void handleMouseDrag(InputEvent & ev) { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
        void handleMouseDown(InputEvent & ev)   { SelectorBox::handleMouseDown(ev); }
};


class EffGuiButton : public Button36
{
public:

        EffGuiButton() : Button36(false) 
        {
            fontId = FontInst;
        }

protected:

        FontId      fontId;

        void drawSelf(Graphics& g)
        {
            Eff* eff = (Eff*)parent;

            setc(g, .8f);

            if(isPressed())
            {
                setc(g, 0.8f);
            }
            else
            {
                setc(g, 0.4f);
            }

            rectx(g, 0, 0, width, height);

            bool wVis = (eff->getDevice() && eff->getDevice()->isWindowVisible());

            if(wVis)
            {
                gGradRect(g, 0xffFF9930, x1, y1, x2, y2);
            }


            {
                eff->setMyColor(g, .6f);
            }
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
};




Eff::Eff(Device36* dev)
{
    mixChannel = NULL;

    device = dev;
    addObject(guiButt = new EffGuiButton());
    addObject(enableButt = new EffEnableButton(device->enabled));
    addObject(device);

    device->placeControls2(FxMaxEffWidth - 16);
    device->setWH(FxMaxEffWidth - 16, device->getH());
    device->setContainer(this);

    setObjName(dev->getObjName());
}

Eff::~Eff()
{
    if (device)
    {
        deleteObject(device);

        device = NULL;
    }
}

void Eff::showDevice(bool show)
{
    if (show)
    {
        device->setEnable(true);
        device->setVis(true);

        setWH(device->getW() + 16, device->getH() + 8);
    }
    else
    {
        device->setEnable(false);
        device->setVis(false);
    }

}

void Eff::remap()
{
    if (MixViewSingle && device->isON())
    {
        enableButt->setCoords1(width - 16, 0, 16, 16);
        device->setCoords1(0, 8, device->getW(), device->getH());
    }
    else
    {
        //enableButt->setVis(false);
    }

    //guiButt->setCoords1(width - 60, 0, 15, 15);
    //foldButt->setCoords1(width - 40, 0, 15, 15);
}

void Eff::drawSelf(Graphics& g)
{
    fill(g, .3f);

    //rect(g, .38f);
    //setc(g, .2f);
    //fillx(g, 0, height-1, width-1, 1);

    if (device->guiWindow && device->guiWindow->isOpen())
    {
        uint32 color = 0xffFF9930;
        uint32 clrDecr = 0x80000000;

        for (int c = 0; c < 4; c++)
        {
            setc(g, (uint32)color);

            rectx(g, 0 + c, 0 + c, width - c*2, height - c*2);

            color -= clrDecr;
            clrDecr /=2;
        }
    }

    FontId fid = FontInst;

    int tw = gGetTextWidth(fid, device->getObjName());
    int th = gGetTextHeight(fid);

    //setc(g, .2f);
    //fillx(g, width/2 - tw/2 - 1, 0, tw + 2, th + 1);

    setc(g, 1.f);
    txt(g, fid, device->getObjName(), width/2 - tw/2, th - 1);

    setc(g, .2f);
    for (int c = 0; c < height; c += 2)
    {
        rectx(g, width - 16, c, 16, 1);
    }
/*
    if (MixViewSingle)
        txt(g, FontBold, device->getObjName(), width - gGetTextWidth(FontBold, device->getObjName()) - 8, height/2 + gGetTextHeight(FontBold)/2);
        //txt(g, FontInst, device->getObjName(), 0, 8);
        //txt(g, FontBold, device->getObjName(), width - gGetTextWidth(FontBold, device->getObjName()) - 4, 12);
    else
        txtfit(g, FontBold, device->getObjName().substr(0, 4) + ".", 0, gGetTextHeight(FontBold) + 1, width);
        */
}

Eff* Eff::clone()
{ 
    return CreateEffect(objId);
}

bool Eff::isEnabled()
{
    return enableButt->isEnabled();
}

ContextMenu* Eff::createContextMenu()
{
    return mixChannel->createContextMenuForEffect(this);
}

void Eff::activateMenuItem(std::string item)
{
    mixChannel->activateEffectMenuItem(this, item);
}

void Eff::setMixChannel(MixChannel* mc)
{
    mixChannel = mc;
}

void Eff::save(XmlElement * xmlEff)
{
    /*
    xmlEff->setAttribute(T("EffIndex"), devIdx);
    xmlEff->setAttribute(T("EffName"), String(objName.data()));
    xmlEff->setAttribute(T("EffPath"), String(filePath.data()));

    //if(devType != EffType_VSTPlugin)
    {
        saveStateData(*xmlEff, "Current", true);
    }
    */
}

void Eff::load(XmlElement * xmlEff)
{
    /*
    devIdx = xmlEff->getIntAttribute(T("EffIndex"));

    //if(devType != EffType_VSTPlugin)
    {
        XmlElement* stateNode = xmlEff->getChildByName(T("Module"));

        if(stateNode != NULL)
        {
            restoreStateData(*stateNode, true);
        }
    }
    */
}

void Eff::handleMouseDown(InputEvent& ev)
{
    if (mixChannel->instr)
    {
    //    MInstrPanel->setCurrInstr(mixChannel->instr);
    }

    if (ev.doubleClick)
    {
        device->showWindow(!device->isWindowVisible());
    }
}

void Eff::handleMouseUp(InputEvent& ev)
{
    if (ev.leftClick)
    {
        //device->showWindow(!device->isWindowVisible());
    }
}

void Eff::handleMouseDrag(InputEvent& ev)
{
    if(MDragDrop->canDrag())
    {
        MDragDrop->start(this, ev.mouseX, ev.mouseY);
    }
}

void Eff::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if (obj == guiButt)
    {
        if (ev.clickDown)
        {
            device->showWindow(!device->isWindowVisible());
        }
    }
    else if (obj == enableButt)
    {
        ///
    }
}

void Eff::handleMouseWheel(InputEvent & ev)
{
    parent->handleMouseWheel(ev);
}

Filter1::Filter1()
{
    objId = "eff.filter1";
    objName = "FILTER";
    //uniqueId = MAKE_FOURCC('C','F','L','T');

    //dspCoreCFilter3.useTwoStages(false);
    f_master = false;
    f_next = NULL;

    addParam(cutoff = new Parameter("CUT", Param_Freq, 0.f, 1.f, 0.6f, Units_Hz));
    addParam(resonance = new Parameter("RES", 0.0f, .97f, 0.f, Units_Percent));

    //addParam(bandwidth = new Parameter("BW", 0.25f, 4.f, 1.f, Units_Octave));

    //Q = new Parameter(0.f, 50.f, 0.f, Param_Default);
    //Q->SetName("Q");
    //Q->AddValueString(Units_Default);
    //AddParamWithParamcell(Q);

    addParam(filterType = new Parameter("FLT.TYPE", Param_Radio));

    filterType->addOption("LOW.PASS");
    filterType->addOption("HIGH.PASS");
    filterType->addOption("BAND.PASS");
    filterType->setCurrentOption(0);

    addParam(x2toggle = new Parameter("x2 MODE", false));

/*
    // Create radiobuttons set
    AddObj(ftype = new Radio36(&scope));
    f1 = new BoolParam("LP12", false, "LOWPASS");
    f2 = new BoolParam("HP12", false, "HIGHPASS");
    f3 = new BoolParam("BP", false, "BANDPASS");
    ftype->AddParamEntry(f1);
    ftype->AddParamEntry(f2);
    ftype->AddParamEntry(f3);
    ftype->SetCurrentItemByName("LP12");

    x2 = new BoolParam("Mode", false, "x2");
    addParam(x2);
    */
}

void Filter1::handleParamUpdate(Parameter* param)
{
    /*
    if(param == f1)
    {
        if(f1->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::LOWPASS);
        }
    }
    else if(param == f2)
    {
        if(f2->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::HIGHPASS);
        }
    }
    else if(param == f3)
    {
        if(f3->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::BANDPASS_12_12);
        }
    }
    else */
    if(param == cutoff)
    {
        dspCoreCFilter3.setCutoff((double)cutoff->getOutVal());
    }
    else if(param == resonance)
    {
        dspCoreCFilter3.setResonance((double)resonance->getOutVal());
        resonance->setValString(resonance->calcValStr((resonance->getOutVal()/.97f*100.0f))); // /66.0f*100.0f
    }
    else if(param == bandwidth)
    {
        //dspCoreCFilter2.setBandwidth(bandwidth->outval);
        //v_bandwidth->SetValue(bandwidth->outval);
    }
    /*
    else if(param == x2)
    {
        if(x2->outval == true)
        {
            dspCoreCFilter3.setOutputStage(4);
        }
        else
        {
            dspCoreCFilter3.setOutputStage(2);
        }
    }
    /*
    if(f_master == true)
    {
        Filter1* f = f_next;

        while(f != NULL)
        {
            if(param == f1 || param == f2 || param == f3)
            {
                f->dspCoreCFilter3.setMode(dspCoreCFilter3.getMode());
            }
            else if(param == cutoff)
            {
                f->dspCoreCFilter3.setCutoff(dspCoreCFilter3.getCutoff());
            }
            else if(param == resonance)
            {
                f->dspCoreCFilter3.setResonance(dspCoreCFilter3.getResonance());
            }
            else if(param == x2)
            {
                f->dspCoreCFilter3.setOutputStage(dspCoreCFilter3.getOutputStage());
            }

            f = f->f_next;
        }
    }*/
}

void Filter1::reset()
{
    dspCoreCFilter3.reset();
}

void Filter1::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];

        dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}


EQ1::EQ1()
{
    objId = "eff.eq1";
    objName = "1-BAND EQ";
    //uniqueId = MAKE_FOURCC('E','Q','0','1');

    //frequency = new Parameter(1000.0f, 20.0f, 19980.0f, Param_Default);
    addParam(frequency = new Parameter("FREQ", Param_Freq, 0.f, 1.f, 0.5f, Units_Hz));
    addParam(bandwidth = new Parameter("BW", 0.25f, 4.f, 1.f, Units_Octave));
    addParam(gain = new Parameter("GAIN", -24.0f, 24.0f, 0.f, Units_dBGain));

    reset();
}

void EQ1::handleParamUpdate(Parameter* param)
{
    if( param == frequency )
    {
        dspCoreEq1.setFrequency(frequency->getOutVal());
    }
    else if( param == gain )
    {
        dspCoreEq1.setGain(gain->getOutVal());
    }
    else if( param == bandwidth )
    {
        dspCoreEq1.setBandwidth(bandwidth->getOutVal());
    }
}

void EQ1::reset()
{
    dspCoreEq1.reset();
}

void EQ1::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCoreEq1.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}

//
// GraphicEQ Class Implementation
//

GraphicEQ::GraphicEQ()
{
    objId = "eff.grapheq";
    objName = "Graphic EQ";
    //uniqueId = MAKE_FOURCC('G','R','E','Q');

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 8000, 0, 1);
    f6 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 6500, 0, 1);
    f5 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 3000, 0, 1);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 900, 0, 1);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 500, 0, 1);
    f4 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 150, 0, 1);
    f9 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 70, 0, 1);

    addParam(gain1 = new Parameter("8 KHz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain6 = new Parameter("6.5 KHz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain5 = new Parameter("3 KHz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain2 = new Parameter("900 Hz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain3 = new Parameter("500 Hz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain4 = new Parameter("150 Hz", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(gain9 = new Parameter("70 Hz", -24.0f, 24.0f, 0.f, Units_dBGain));

    reset();
}

void GraphicEQ::handleParamUpdate(Parameter* param)
{
    if(param == gain5)
    {
        dspCoreEqualizer.setBandGain(f5, gain5->getOutVal());
    }
    else if(param == gain6)
    {
        dspCoreEqualizer.setBandGain(f6, gain6->getOutVal());
    }
    else if(param == gain1)
    {
        dspCoreEqualizer.setBandGain(f1, gain1->getOutVal());
    }
    else if(param == gain2)
    {
        dspCoreEqualizer.setBandGain(f2, gain2->getOutVal());
    }
    else if(param == gain3)
    {
        dspCoreEqualizer.setBandGain(f3, gain3->getOutVal());
    }
    else if(param == gain4)
    {
        dspCoreEqualizer.setBandGain(f4, gain4->getOutVal());
    }
    else if(param == gain9)
    {
        dspCoreEqualizer.setBandGain(f9, gain9->getOutVal());
    }
}

void GraphicEQ::reset()
{
    dspCoreEqualizer.reset();
}

void GraphicEQ::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCoreEqualizer.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}


EQ3::EQ3()
{
    objId = "eff.eq3";
    objName = "3-Band EQ";
    //uniqueId = MAKE_FOURCC('E','Q','0','3');

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 4000, 0);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 0, 0, 0);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 100, 0);

    freqLow = gainLow = freqCentr = gainCentr = freqHigh = gainHigh = centrBW = NULL;

    addParam(freqLow = new Parameter("LOW.FREQ", Param_Freq, 0.f, 1.f, 0.2f, Units_Hz));
    addParam(gainLow = new Parameter("LOW.GAIN", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(freqCentr = new Parameter("CENTR.FREQ", Param_Freq, 0.f, 1.f, 0.5f, Units_Hz));
    addParam(gainCentr = new Parameter("CENTR.GAIN", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(freqHigh = new Parameter("HIGH.FREQ", Param_Freq, 0.f, 1.f, 0.8f, Units_Hz));
    addParam(gainHigh = new Parameter("HIGH.GAIN", -24.0f, 24.0f, 0.f, Units_dBGain));
    addParam(centrBW = new Parameter("BANDWIDTH", 0.25f, 4.f, 1.5f, Units_Octave));


/*
    addParam(freq1 = new FrequencyParameter("High freq", 0.5, Units_Hz));

    freq3 = new FrequencyParameter("Low freq", 0.5, Units_Hz);
    AddParamWithParamcell(freq3);
*/

    reset();
}

void EQ3::drawCurve(Graphics& g, int x, int y, int w, int h)
{
    setc(g, 0.4f);

    gDrawRectWH(g, x, y, w, h);

    Point36 eq[7] = { 0,  10,
                        10, 0,
                        20, 10,
                        30, 0,
                        40, 10,
                        50, 0,
                        60, 10 };

    float band = centrBW->getNormalizedValue() * (width / 6);

    eq[0].x = 0;
    eq[0].y = h * (1 - gainLow->getNormalizedValue());

    // Low 
    eq[1].x = w * freqLow->getNormalizedValue();
    eq[1].y = eq[0].y;

    eq[2].x = w * freqCentr->getNormalizedValue() - band / 2;
    eq[2].y = h / 2;

    eq[3].x = w * freqCentr->getNormalizedValue();
    eq[3].y = h * (1 - gainCentr->getNormalizedValue());

    eq[4].x = w * freqCentr->getNormalizedValue() + band / 2;
    eq[4].y = h / 2;

    eq[5].x = w * freqHigh->getNormalizedValue();
    eq[5].y = h * (1 - gainHigh->getNormalizedValue());

    eq[6].x = w - 1;
    eq[6].y = eq[5].y;

    gFillRectWH(g, x + eq[1].x, y, 1, h);
    gFillRectWH(g, x + eq[3].x, y, 1, h);
    gFillRectWH(g, x + eq[5].x, y, 1, h);

    setc(g, 0.8f);

    Path p;
    p.startNewSubPath(float(x + eq[0].x), float(y + eq[0].y));

    for (int c = 1; c < 7; c++)
    {
        p.lineTo(float(x + eq[c].x), float(y + eq[c].y));
    }

    //p.closeSubPath();
    /*
    p.startNewSubPath(x1, y1);

    p.lineTo(x1 + 30, y1 + 30);
    p.lineTo(x1 + 40, y1 + 20);*/

    g.strokePath(p, PathStrokeType(1, PathStrokeType::JointStyle::curved));
}


void EQ3::drawOverChildren(Graphics & g)
{
    //drawCurve(g, x2 - 90, y1 + 10, 88, height - 20);
}

void EQ3::handleParamUpdate(Parameter* param)
{
    if(param == gainHigh)
    {
        dspCoreEqualizer.setBandGain(f1, gainHigh->getOutVal());
    }
    else if(param == gainCentr)
    {
        dspCoreEqualizer.setBandGain(f2, gainCentr->getOutVal());
    }
    else if(param == gainLow)
    {
        dspCoreEqualizer.setBandGain(f3, gainLow->getOutVal());
    }
    else if(param == freqHigh)
    {
        if (freqCentr != NULL && freqHigh->getNormalizedValue() < freqCentr->getNormalizedValue())
        {
            freqHigh->setNormalizedValue(freqCentr->getNormalizedValue());
        }

        dspCoreEqualizer.setBandFrequency(f1, freqHigh->getOutVal());
    }
    else if(param == freqCentr)
    {
        if (freqHigh != NULL && freqCentr->getNormalizedValue() > freqHigh->getNormalizedValue())
        {
            freqCentr->setNormalizedValue(freqHigh->getNormalizedValue());
        }

        if (freqLow != NULL && freqCentr->getNormalizedValue() < freqLow->getNormalizedValue())
        {
            freqCentr->setNormalizedValue(freqLow->getNormalizedValue());
        }

        dspCoreEqualizer.setBandFrequency(f2, freqCentr->getOutVal());
    }
    else if(param == freqLow)
    {
        if (freqCentr != NULL && freqLow->getNormalizedValue() > freqCentr->getNormalizedValue())
        {
            freqLow->setNormalizedValue(freqCentr->getNormalizedValue());
        }

        dspCoreEqualizer.setBandFrequency(f3, freqLow->getOutVal());
    }
    else if(param == centrBW)
    {
        dspCoreEqualizer.setBandBandwidth(f2, centrBW->getOutVal());
    }

    if (parent)
        parent->redraw();
}

void EQ3::reset()
{
    dspCoreEqualizer.reset();
}

void EQ3::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];
        dspCoreEqualizer.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}


CTremolo::CTremolo()
{
    objId = "eff.tremolo";
    objName = "Amp. Tremolo";
    //uniqueId = MAKE_FOURCC('T','R','E','M');

    addParam(speed = new Parameter("SPEED", 0.1f, 2.35f, 1.f));
    addParam(depth = new Parameter("DEPTH", 0.f, 1.0f, 0.5f));

    reset();
}

void CTremolo::handleParamUpdate(Parameter* param)
{
    if(param == speed)
    {
        dspCoreCTremolo.setCycleLength(speed->getOutVal());
    }
    else if(param == depth)
    {
        dspCoreCTremolo.setDepth(depth->getOutVal());
    }
}

void CTremolo::reset()
{
    dspCoreCTremolo.resetOscillatorPhases();
}

void CTremolo::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    dspCoreCTremolo.setTempoInBPM(MTransp->getBeatsPerMinute());

    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];
        dspCoreCTremolo.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}

Compressor::Compressor()
{
    objId = "eff.comp";
    objName = "Compressor";
    //uniqueId = MAKE_FOURCC('C','O','M','P');

    //addParam(mode = new BoolParam(false, "MODE", "LIMITER MODE"));

    addParam(thresh = new Parameter("THRESH", -60.0f, 0.0f, 0.f, Units_dB));
    addParam(ratio = new Parameter("RATIO", 1.0f, 31.0f, 1.0f));
    addParam(knee = new Parameter("KNEE", 0.0f, 48.0f, 0.f, Units_dB));
    addParam(attack = new Parameter("ATTACK", 1.f, 91.0f, 10.f, Units_ms));
    addParam(release = new Parameter("RELEASE", 10.f, 910.0f, 100.f, Units_ms));
    addParam(gain = new Parameter("GAIN", 0.f, 30.0f, 0.f, Units_dB));

/* // Unpredictable shit
    autogain = new BoolParam(false);
    autogain->SetName("Autogain");
    AddParamWithParamcell(autogain);
*/
}

void Compressor::handleParamUpdate(Parameter* param)
{
/*
    if(param == mode)
    {
    //    dspCoreComp.setLimiterMode(mode->outval);
    }
    else
    if(param == autogain)
    {
        if(autogain->outval == true)
        {
            //autogain_state->SetVstrValue("AUTOGAIN ON");
            dspCoreComp.setAutoGain(true);
        }
        else
        {
            //autogain_state->SetVstrValue("AUTOGAIN OFF");
            dspCoreComp.setAutoGain(false);
        }
    }
    else  */
    if(param == thresh)
    {
        dspCoreComp.setThreshold((double)thresh->getOutVal());
    }
    else if(param == knee)
    {
        dspCoreComp.setThreshold((double)knee->getOutVal());
    }
    else if(param == ratio)
    {
        dspCoreComp.setRatio((double)ratio->getOutVal());
    }
    else if(param == gain)
    {
        dspCoreComp.setOutputGain((double)gain->getOutVal());
    }
    else if(param == attack)
    {
        dspCoreComp.setAttackTime(attack->getOutVal());
    }
    else if(param == release)
    {
        dspCoreComp.setReleaseTime(release->getOutVal());
    }
}

void Compressor::reset()
{
	dspCoreComp.reset();
}

void Compressor::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCoreComp.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}

CWahWah::CWahWah()
{
    objId = "eff.wah";
    objName = "WahWah";
    //uniqueId = MAKE_FOURCC('W','A','H','W');

    addParam(modfreq = new Parameter("MODFREQ", 0.1f, 5.f, 1.25f, Units_Hz1));
    addParam(depth = new Parameter("DEPTH", 2.f, 48.f, 48.f, Units_Semitones));

    //addParam(frequency = new Parameter("FREQ.", Param_Freq, 0.5f, 0.f, 1.f, Units_Hz));
    //addParam(drywet = new Parameter("DRY/WET", 0.75f, 0.0f, 1.f, Units_DryWet));

    reset();
}

void CWahWah::handleParamUpdate(Parameter* param)
{
    /*
    if(param == drywet)
    {
        dspCoreWah.setDryWetRatio(drywet->outval);
    }
    else if(param == frequency)
    {
        dspCoreWah.setFrequency(frequency->outval);
    }
    else */
    if(param == modfreq)
    {
        dspCoreWah.setCycleLength(1.f/modfreq->getOutVal());
    }
    else if(param == depth)
    {
        dspCoreWah.setDepth(depth->getOutVal());
    }
}

void CWahWah::reset()
{
    dspCoreWah.reset();
}

void CWahWah::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];

        dspCoreWah.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

CDistort::CDistort()
{
    objId = "eff.dist";
    objName = "DISTORTION";
    //uniqueId = MAKE_FOURCC('D','I','S','T');

    addParam(drive = new Parameter("DRIVE", 0.0f, 48.f, 32.0f, Units_dB));
    addParam(postgain = new Parameter("POSTGAIN", -48.0f, 0.f, 0.0f, Units_dB));
    addParam(slope = new Parameter("SLOPE", -1.0f, 3.f, 1.0f));

    dspCoreDist.setOversampling(1);
    dspCoreDist.setAmount(10);
    dspCoreDist.setTransferFunction(3);

    reset();
}

void CDistort::handleParamUpdate(Parameter* param)
{
    if(param == drive)
    {
        dspCoreDist.setDrive(drive->getOutVal());
    }
    else if(param == postgain)
    {
        dspCoreDist.setOutputLevel(postgain->getOutVal());
    }
    else if(param == slope)
    {
        dspCoreDist.setPenticSlopeAtZero(slope->getOutVal());
    }
}

void CDistort::reset()
{
	dspCoreDist.reset();
}

void CDistort::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];

        dspCoreDist.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

CBitCrusher::CBitCrusher()
{
    objId = "eff.bitcrush";
    objName = "BitCrusher";
    //uniqueId = MAKE_FOURCC('B','I','T','C');

    addParam(decimation = new Parameter("DECIMATION", 1.0f, 128.f, 1.0f));
    addParam(quantization = new Parameter("QUANTIZATION", 0.0f, 1.f, 0.0f));

    dspCoreBC.setAmount(1);

    handleParamUpdate(decimation);
    handleParamUpdate(quantization);
}

void CBitCrusher::handleParamUpdate(Parameter* param)
{
    if(param == decimation)
    {
        dspCoreBC.setDecimationFactor(int(decimation->getOutVal()));
    }
    else if(param == quantization)
    {
        dspCoreBC.setQuantizationInterval(0.001f*pow(1000.0f, quantization->getOutVal()));
    }
}

void CBitCrusher::reset()
{
    dspCoreBC.reset();
}

void CBitCrusher::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];
        dspCoreBC.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

CStereo::CStereo()
{
    objId = "eff.stereo";
    objName = "Stereo X";
    //uniqueId = MAKE_FOURCC('S','T','E','R');

    addParam(offset = new Parameter("OFFSET", 1.0f, 100.f, 10.0f));

    handleParamUpdate(offset);
}

void CStereo::handleParamUpdate(Parameter* param)
{
    if(param == offset)
    {
        dspCoreStereo.delayLine.setDelayInMilliseconds(offset->getOutVal());
    }
}

void CStereo::reset()
{
    //dspCoreStereo.reset();
}

void CStereo::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];

        dspCoreStereo.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

XDelay::XDelay() : dspCorePingPongDelay()
{
    objId = "eff.delay";
    objName = "Delay";
    //uniqueId = MAKE_FOURCC('P','P','D','L');

    dspCorePingPongDelay.setTrueStereoMode(true);
    dspCorePingPongDelay.setPingPongMode(true);
    dspCorePingPongDelay.setStereoSwap(true);

    addParam(delayMode = new Parameter("PING-PONG", true));

    addParam(delay = new Parameter("DELAY", 0.5f, 20.f, 3, Units_Ticks));
    delay->setInterval(0.25f);
    
    addParam(amount = new Parameter("AMOUNT", 0.f, 1.f, 1.f, Units_Percent));
    addParam(feedback = new Parameter("FEEDBACK", 0.0f, 100.f, 55.f, Units_Percent));
    addParam(pan = new Parameter("PAN", -1.0f, 1.f, 0.0f));

    //highCut = new FrequencyParameter(0.2f);
    //highCut->SetName("HighCut");
    //highCut->AddValueString(Units_Hz);
    //AddParamWithParamcell(highCut);

    addParam(lowcut = new Parameter("LOWCUT", Param_Freq, 0.f, 1.f, 0.9f, Units_Hz));
    addParam(drywet = new Parameter("DRY/WET", 0.0f, 1.f, .4f, Units_DryWet));

/*
    addParam(selectTypes = new Parameter("Define:", Param_Selector));
    selectTypes->addOption("1 tick", false);
    selectTypes->addOption("2 tick", false);
    selectTypes->addOption("3 tick", true);
    selectTypes->addOption("4 tick", false);
    selectTypes->addOption("5 tick", false);
    selectTypes->addOption("6 tick", true);
*/

    reset();
}

void XDelay::handleParamUpdate(Parameter* param)
{
    if(param == delayMode)
    {
        dspCorePingPongDelay.setPingPongMode(delayMode->getBoolValue());
        dspCorePingPongDelay.setStereoSwap(delayMode->getBoolValue());
        dspCorePingPongDelay.setPan((delayMode->getBoolValue() ? -1 : 1)*delayMode->getBoolValue());
    }
    else if(param == amount)
    {
        //dspCorePingPongDelay.setGlobalGainFactor(ggain->outval); // old, obsolete
        dspCorePingPongDelay.setWetLevel(amp2dB(amount->getOutVal()));
        amount->setValString(amount->calcValStr(int(100*amount->getOutVal())));
    }
    /*
    else if( param->getName() == "High.cut" )
    {
        dspCorePingPongDelay.setLowDamp(param->outval);
    }
    */
    else if(param == delay)
    {
        dspCorePingPongDelay.setDelayTime(delay->getOutVal()/MTransp->getTicksPerBeat());
    }
    else if( param == drywet )
    {
        dspCorePingPongDelay.setDryWetRatio((float)(drywet->getOutVal()));
    }
    else if( param == feedback )
    {
        dspCorePingPongDelay.setFeedbackInPercent(feedback->getOutVal());
    }
    else if( param == pan )
    {
        dspCorePingPongDelay.setPan((delayMode->getBoolValue() == true ? -1 : 1)*pan->getOutVal());
    }
    else if( param == lowcut )
    {
        dspCorePingPongDelay.setHighDamp(lowcut->getOutVal());
    }
}

void XDelay::reset()
{
    dspCorePingPongDelay.reset();
}

void XDelay::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    dspCorePingPongDelay.setTempoInBPM(MTransp->getBeatsPerMinute());

    double inOutL;
    double inOutR;

    for(int i=0; i<2*num_frames; i+=2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCorePingPongDelay.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}

CReverb::CReverb() : dspCoreReverb()
{
    objId = "eff.reverb";
    objName = "Reverb";
    //uniqueId = MAKE_FOURCC('R','E','V','R');

    addParam(preDelay  = new Parameter("PREDELAY", 0.0f, 200.0f, 0.0f, Units_ms));
    addParam(roomsize = new Parameter("ROOM", 4.0f, 100.0f, 100.0f));
    addParam(decay = new Parameter("DECAY", 1.0f, 15.f, 6.f, Units_Seconds));
    addParam(highCut = new Parameter("HIGHCUT", Param_Freq, 0.f, 1.f, 0.2f, Units_Hz));
    addParam(lowCut = new Parameter("LOWCUT", Param_Freq, 0.f, 1.f, 1.0f, Units_Hz));
    addParam(lowscale = new Parameter("LOW.SCALE", Param_Log, 0.1f, 100.1f, 0.5f));
    addParam(highscale = new Parameter("HIGH.SCALE", Param_Log, 0.1f, 100.1f, 0.24f));
    addParam(drywet = new Parameter("DRY/WET", 0.0f, 1.f, .5f, Units_DryWet));

    reset();
}

void CReverb::handleParamUpdate(Parameter* param)
{
    if(param == roomsize)
    {
        dspCoreReverb.setReferenceDelayTime(roomsize->getOutVal());
    }
    else if(param == preDelay)
    {
        dspCoreReverb.setPreDelay(preDelay->getOutVal());
    }
    else if(param == drywet)
    {
        dspCoreReverb.setDryWetRatio((float)(drywet->getOutVal()));
    }
    else if(param == decay)
    {
        dspCoreReverb.setMidReverbTime(decay->getOutVal());
    }
    else if(param == highCut)
    {
        dspCoreReverb.setWetHighpassCutoff(highCut->getOutVal());
    }
    else if(param == lowCut)
    {
        dspCoreReverb.setWetLowpassCutoff(lowCut->getOutVal());
    }
    else if(param == highscale)
    {
        dspCoreReverb.setHighReverbTimeScale(highscale->getOutVal());
    }
    else if(param == lowscale)
    {
        dspCoreReverb.setLowReverbTimeScale(lowscale->getOutVal());
    }
}

void CReverb::reset()
{
    dspCoreReverb.reset();
}

void CReverb::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCoreReverb.getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}

CChorus::CChorus()
{
    objId = "eff.chorus";
    objName = "Chorus";
    //uniqueId = MAKE_FOURCC('C','H','O','R');

    dspCoreChorus = new rosic::Chorus(65535);
    dspCoreChorus->setTempoSync(false);

    addParam(delay = new Parameter("DELAY", 1.f, 50.0f, 5.f, Units_ms));
    addParam(freq = new Parameter("MODFREQ", 0.1f, 4.9f, 2.f, Units_Hz));
    addParam(depth = new Parameter("DEPTH", 0, 1.5f, 0.25f, Units_Semitones));
    addParam(drywet = new Parameter("DRY/WET", 0.0f, 1.f, 0.5f, Units_DryWet));
}

void CChorus::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCoreChorus->setDryWetRatio(drywet->getOutVal());
    }
    else if(param == delay)
    {
        dspCoreChorus->setAverageDelayTime(delay->getOutVal());
    }
    else if(param == freq)
    {
        dspCoreChorus->setCycleLength(1.f/freq->getOutVal());
    }
    else if(param == depth)
    {
        dspCoreChorus->setDepth(depth->getOutVal());
    }
}

void CChorus::reset()
{
    //dspCoreChorus.reset();
}

void CChorus::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];

        dspCoreChorus->getSampleFrameStereo(&inOutL, &inOutR);

        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}


CFlanger::CFlanger()
{
    objId = "eff.flanger";
    objName = "Flanger";
    //uniqueId = MAKE_FOURCC('F','L','N','G');

    dspCoreFlanger.setTempoSync(false);

    //fmemory = (float*)malloc(65535);
    //dspCoreFlanger.setSharedMemoryAreaToUse(fmemory, 65535); -> not needed anymore - Flanger allocates its own memory now

    reset();

    addParam(frequency = new Parameter("DELAY", Param_Freq, 0.f, 1.f, .5f, Units_ms2));

    frequency->setReversed(true);

    addParam(modfreq = new Parameter("MODFREQ", 0.1f, 10.f, 3.f, Units_Seconds));
    addParam(feedback = new Parameter("FEEDBACK", -1.f, 1.f, 0));
    addParam(depth = new Parameter("DEPTH", 2.f, 48.f, 24.f, Units_Semitones));

    //    invert = new BoolParam("Invert wet signal", false, "Invert wet signal");
    //    AddParamWithParamcell(invert);

    addParam(drywet = new Parameter("DRY/WET", 0.0f, 0.5f, 0.5f, Units_DryWet));
}

void CFlanger::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCoreFlanger.setDryWetRatio(drywet->getOutVal());
    }
    else if(param == frequency)
    {
        dspCoreFlanger.setFrequency(frequency->getOutVal());

        frequency->setValString(frequency->calcValStr(1.f/frequency->getOutVal()*1000));
    }
    else if(param == modfreq)
    {
        dspCoreFlanger.setCycleLength(modfreq->getOutVal());
    }
    else if(param == depth)
    {
        dspCoreFlanger.setDepth(depth->getOutVal());
    }
    else if(param == feedback)
    {
        dspCoreFlanger.setFeedbackFactor(feedback->getOutVal());
    }
    /*
    else if(param == invert)
    {
        dspCoreFlanger.setNegativePolarity(invert->outval);
    }*/
}

void CFlanger::reset()
{
    dspCoreFlanger.reset();
}

void CFlanger::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];
        dspCoreFlanger.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}


CPhaser::CPhaser()
{
    objId = "eff.phaser";
    objName = "Phaser";
    //uniqueId = MAKE_FOURCC('P','H','A','S');

    dspCorePhaser.setTempoSync(false);

    //fmemory = (float*)malloc(65535);
    //dspCorePhaser.setSharedMemoryAreaToUse(fmemory, 65535);

    reset();

    addParam(frequency = new Parameter("DELAY", Param_Freq, 0.f, 1.f, 0.5f, Units_ms2));
    frequency->setReversed(true);
    addParam(modfreq = new Parameter("MODFREQ", 0.1f, 10.f, 3.f, Units_Seconds));
    addParam(feedback = new Parameter("FEEDBACK", -1.f, 1.f, 0));
    addParam(depth = new Parameter("DEPTH", 2.f, 48.f, 24.f, Units_Semitones));
    addParam(numstages = new Parameter("STAGES", 1, 24, 4));
    numstages->setInterval(1);
    addParam(stereo = new Parameter("STEREO", 0, 180, 0));
    stereo->setInterval(1);

    addParam(drywet = new Parameter("DRY/WET", 0.0f, 0.5f, 0.5f, Units_DryWet));

    dspCorePhaser.setFilterMode(rosic::AllpassChain::FIRST_ORDER_ALLPASS);
}

void CPhaser::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCorePhaser.setDryWetRatio(drywet->getOutVal());
    }
    else if(param == frequency)
    {
        dspCorePhaser.setFrequency(frequency->getOutVal());

        frequency->setValString(frequency->calcValStr(1.f/frequency->getOutVal()*1000));
    }
    else if(param == modfreq)
    {
        dspCorePhaser.setCycleLength(modfreq->getOutVal());
    }
    else if(param == depth)
    {
        dspCorePhaser.setDepth(depth->getOutVal());
    }
    else if(param == feedback)
    {
        dspCorePhaser.setFeedbackFactor(feedback->getOutVal());
    }
    else if(param == numstages)
    {
        dspCorePhaser.setNumStages(int(numstages->getOutVal()));
    }
    else if(param == stereo)
    {
        dspCorePhaser.setStereoPhaseOffsetInDegrees((stereo->getOutVal()));
        dspCorePhaser.resetOscillatorPhases();
    }
}

void CPhaser::reset()
{
    dspCorePhaser.reset();
}

void CPhaser::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;

    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];
        dspCorePhaser.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
    }
}


