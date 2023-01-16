//
//
//

#include <direct.h>

#include "36_effects.h"
#include "36_menu.h"
#include "36_mixer.h"
#include "36_vst.h"
#include "36_project.h"
#include "36_history.h"
#include "36_pattern.h"
#include "36_params.h"
#include "36_audio_dev.h"
#include "36_transport.h"
#include "36_browser.h"
#include "36_utils.h"
#include "36_button.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_config.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_slider.h"
#include "36_macros.h"
#include "36.h"
#include "36_effwin.h"




class EffGuiButton : public Button36
{
protected:

        void drawSelf(Graphics& g)
        {
            Instrument* instr = (Instrument*)parent;

           fill(g, 0.45f);

           rect(g, 0.35f);
        }

        void handleMouseEnter(InputEvent & ev)  { redraw(); }
        void handleMouseLeave(InputEvent & ev)  { redraw(); }
        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
        //void handleMouseUp(InputEvent & ev)     { redraw();  parent->redraw();  }

public:

        EffGuiButton() : Button36(true) {}
};


Eff::Eff()
{
    folded = false;
    bypass = false;
    muteCount = 0;
    visible = false;
    devIdx = -1;

/*
    Add(fold_toggle = new Toggle(&folded, &scope), OgTgEffFold, MixChanWidth - 17, 5, 15, 10);
    fold_toggle->SetOnClickHandler(&EffFoldClick);
    fold_toggle->SetHint("Fold/unfold effect");

    Add(bypass_toggle = new Toggle(&bypass, &scope), OToggleEffBypass, MixChanWidth - 15, 2, 15, 10);
    bypass_toggle->SetOnClickHandler(&EffBypassClick);
    bypass_toggle->SetHint("Effect on/bypass");

    Add(wnd_toggle = new Toggle(&wndvisible, &scope), OToggleEffWnd, 0, 0, 18, 18);

    wnd_toggle->SetOnClickHandler(&EffWindowClick);
    wnd_toggle->SetHint("Show editor window");
*/

    addObject(guiButt = new EffGuiButton(), MixChanWidth - 21, 0, 20, 15);

    //addParamWithControl(new Parameter(1, 0, 1), "", sliderAmount = new Slider36(false));
}

Eff::~Eff()
{
    ///
}

void Eff::mapObjects()
{
    //guiButt->setCoords1(width - 13, 0, 12, 12);

    //sliderAmount->setCoords2(4, height - 12, 30, height - 4);
}

void Eff::drawSelf(Graphics& g)
{
    fill(g, .2f);
    rect(g, .5f);
    setc(g, .2f);
    lineH(g, height-1, 0, width-1);

    int th = gGetTextHeight(FontBold);
    setc(g, 1.f);
    gTextFit(g, FontSmall, objName, x1, y2, width - 2);
}

Eff* Eff::makeClone(Eff* eff)
{
    auto itr1 = params.begin();
    auto itr2 = eff->params.begin();

    for(; itr1 != params.end(); itr1++, itr2++)
    {
        (*itr2)->setValue((*itr1)->getValue());
    }

    return eff;
}

Eff* Eff::clone()
{ 
    return makeClone(CreateEffect(objId));
}

ContextMenu* Eff::createContextMenu()
{
    return mixChannel->createContextMenuForEffect(this);
}

void Eff::activateContextMenuItem(std::string item)
{
    mixChannel->activateEffectMenuItem(this, item);
}

void Eff::setMixChannel(MixChannel* mc)
{
    mixChannel = mc;
}

void Eff::process(float* in_buff, float* out_buff, int num_frames)
{
    if(envelopes == NULL && (bypass == false || muteCount < DECLICK_COUNT))
    {
        processData(in_buff, out_buff, num_frames);
    }
    else if(envelopes != NULL)
    {
        long frames_to_process;
        long buffframe = 0;
        long frames_remaining = num_frames;

        while(frames_remaining > 0)
        {
            if(frames_remaining > 32)
            {
                frames_to_process = 32;
            }
            else
            {
                frames_to_process = frames_remaining;
            }

            /*
            tgenv = envelopes;
            while(tgenv != NULL)
            {
                env = (Envelope*)tgenv->el;
                if(buffframe >= env->last_buffframe)
                {
                    param = env->param;
                    param->SetValueFromEnvelope(env->buffoutval[buffframe], env);
                }
                tgenv = tgenv->group_prev;
            }
            */

            if(bypass == false || muteCount < DECLICK_COUNT)
            {
                processData(&in_buff[buffframe*2], &out_buff[buffframe*2], frames_to_process);
            }

            frames_remaining -= frames_to_process;
            buffframe += frames_to_process;
        }
    }

    if(bypass == true && muteCount >= DECLICK_COUNT)
    {
        memcpy(out_buff, in_buff, num_frames*2*sizeof(float));
    }
}

void Eff::save(XmlElement * xmlEff)
{
    xmlEff->setAttribute(T("EffIndex"), devIdx);
    xmlEff->setAttribute(T("EffName"), String(objName.data()));
    xmlEff->setAttribute(T("EffPath"), String(filePath.data()));
    xmlEff->setAttribute(T("Folded"), folded ? 1 : 0);
    xmlEff->setAttribute(T("Bypass"), bypass ? 1 : 0);

    //if(devType != EffType_VSTPlugin)
    {
        saveStateData(*xmlEff, "Current", true);
    }
}

void Eff::load(XmlElement * xmlEff)
{
    devIdx = xmlEff->getIntAttribute(T("EffIndex"));
    folded = xmlEff->getBoolAttribute(T("Folded"));
    bypass = xmlEff->getBoolAttribute(T("Bypass"));

    //if(devType != EffType_VSTPlugin)
    {
        XmlElement* stateNode = xmlEff->getChildByName(T("Module"));

        if(stateNode != NULL)
        {
            restoreStateData(*stateNode, true);
        }
    }
}

void Eff::handleMouseDown(InputEvent& ev)
{
    MMixer->setCurrentEffect(this);

    if (mixChannel->instr)
    {
    //    MInstrPanel->setCurrInstr(mixChannel->instr);
    }
}

void Eff::handleMouseUp(InputEvent& ev)
{
    if (ev.leftClick)
    {
        showWindow(!isWindowVisible());
    }
}

void Eff::handleMouseDrag(InputEvent& ev)
{
    if(MObject->canDrag(this))
    {
        MObject->dragAdd(this, ev.mouseX, ev.mouseY);
    }
}

void Eff::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if(obj == guiButt)
    {
        showWindow(guiButt->isPressed());
    }
}

SubWindow* Eff::createWindow()
{
    SubWindow* win =  window->addWindow(new EffParamObject(this));

    //int xb = window->getLastEvent().mouseX + 20;
    //int yb = window->getLastEvent().mouseY - guiWindow->getHeight()/2;

    win->setBounds(window->getLastEvent().mouseX + 40, win->getY(), win->getWidth(), win->getHeight());

    return win;
}


BasicLP::BasicLP()
{
    objId = "eff.lp";
    objName = "Basic Filter";
    uniqueId = MAKE_FOURCC('F','L','T','R');

    addParamWithControl(cutoff = new Parameter("CUTOFF", 700.0f, 200.0f, 9800.0f, Units_kHz));
    addParamWithControl(resonance = new Parameter("RESONANCE", 5.5f, 1.0f, 10.0f, Units_Percent));

    // Filter work data
    lx1 = lx2 = ly1 = ly2 = 0.0f;
    rx1 = rx2 = ry1 = ry2 = 0.0f;

    reset();
}

void BasicLP::handleParamUpdate(Parameter* param)
{
    cutoff->setValString(cutoff->calcValStr((float)(cutoff->outVal/1000.0f)));
    resonance->setValString(resonance->calcValStr(((resonance->outVal - 1.0f)/10.0f*100)));

    reset();
}

void BasicLP::reset()
{
    float alpha, omega, sn, cs;
    float a0, a1, a2, b0, b1, b2;

    omega = (float)(2.0f*PI*cutoff->outVal/MAudio->getSampleRate());
    sn = sin (omega); 
    cs = cos (omega);
    alpha = sn/resonance->outVal;
    b0 = (1.0f - cs)/2.0f;
    b1 = 1.0f - cs;
    b2 = (1.0f - cs)/2.0f;
    a0 = 1.0f + alpha;
    a1 = -2.0f*cs;
    a2 = 1.0f - alpha;

    filtCoefTab[0] = b0/a0;
    filtCoefTab[1] = b1/a0;
    filtCoefTab[2] = b2/a0;
    filtCoefTab[3] = -a1/a0;
    filtCoefTab[4] = -a2/a0;
}

void BasicLP::processData(float* in_buff, float* out_buff, int num_frames)
{
    float inL, inR, outL, outR, temp_y;
    int i;

    long fc = 0;
    for(i=0; i < num_frames; i++)
    {
        inL = in_buff[fc];
        inR = in_buff[fc + 1];

        outL = inL;
        outR = inR;

        // Left
        temp_y = filtCoefTab[0]*outL +
                 filtCoefTab[1]*lx1 +
                 filtCoefTab[2]*lx2 +
                 filtCoefTab[3]*ly1 +
                 filtCoefTab[4]*ly2;

        ly2 = ly1; 
        ly1 = temp_y; 
        lx2 = lx1; 
        lx1 = outL ; 
        outL = temp_y;

        // Right
        temp_y = filtCoefTab[0]*outR +
                 filtCoefTab[1]*rx1 +
                 filtCoefTab[2]*rx2 +
                 filtCoefTab[3]*ry1 +
                 filtCoefTab[4]*ry2;

        ry2 = ry1; 
        ry1 = temp_y; 
        rx2 = rx1; 
        rx1 = outR ; 
        outR = temp_y;

        out_buff[fc++] = outL;
        out_buff[fc++] = outR;
    };
}

Filter1::Filter1()
{
    objId = "eff.filter1";
    objName = "FLT1";
    uniqueId = MAKE_FOURCC('C','F','L','T');

    //dspCoreCFilter3.useTwoStages(false);
    f_master = false;
    f_next = NULL;

    addParamWithControl(cutoff = new Parameter("CUT", Param_Freq, 0.6f, 0.f, 1.f, Units_Hz));
    addParamWithControl(resonance = new Parameter("RES", 0.f, 0.0f, .97f, Units_Percent));
    //addParamWithControl(bandwidth = new Parameter("BW", 1.f, 0.25f, 3.75f, Units_Octave));

    //Q = new Parameter(0.f, 0.f, 50.f, Param_Default);
    //Q->SetName("Q");
    //Q->AddValueString(Units_Default);
    //AddParamWithParamcell(Q);

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
    addParamWithControl(x2);
    */
}

void Filter1::handleParamUpdate(Parameter* param)
{
    if(param == f1)
    {
        BoolParam* bp = (BoolParam*)param;

        if(bp->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::LOWPASS);
        }
    }
    else if(param == f2)
    {
        BoolParam* bp = (BoolParam*)param;

        if(bp->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::HIGHPASS);
        }
    }
    else if(param == f3)
    {
        BoolParam* bp = (BoolParam*)param;

        if(bp->getOutVal())
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::BANDPASS_12_12);
        }
    }
    else if(param->prmName == "CUT")
    {
        dspCoreCFilter3.setCutoff((double)cutoff->outVal);
    }
    else if(param->prmName == "RES")
    {
        dspCoreCFilter3.setResonance((double)resonance->outVal);
        resonance->setValString(resonance->calcValStr((resonance->outVal/.97f*100.0f))); // /66.0f*100.0f
    }
    else if(param->prmName == "BW")
    {
        //dspCoreCFilter2.setBandwidth(bandwidth->outval);
        //v_bandwidth->SetValue(bandwidth->outval);
    }
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
    }
}

void Filter1::reset()
{
    dspCoreCFilter3.reset();
}

void Filter1::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "EQ1";
    uniqueId = MAKE_FOURCC('E','Q','0','1');

    //frequency = new Parameter(1000.0f, 20.0f, 19980.0f, Param_Default);
    addParamWithControl(frequency = new Parameter("FREQ", Param_Freq, 0.5f, 0.f, 1.f, Units_Hz));
    addParamWithControl(bandwidth = new Parameter("BW", 1.f, 0.25f, 3.75f, Units_Octave));
    addParamWithControl(gain = new Parameter("GAIN", 0.f, -24.0f, 48.0f, Units_dBGain));

    reset();
}

void EQ1::handleParamUpdate(Parameter* param)
{
    if( param == frequency )
    {
        dspCoreEq1.setFrequency(frequency->outVal);
    }
    else if( param == gain )
    {
        dspCoreEq1.setGain(gain->outVal);
    }
    else if( param == bandwidth )
    {
        dspCoreEq1.setBandwidth(bandwidth->outVal);
    }
}

void EQ1::reset()
{
	dspCoreEq1.reset();
}

void EQ1::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "GQ1";
    uniqueId = MAKE_FOURCC('G','R','E','Q');

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 8000, 0, 1);
    f6 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 6500, 0, 1);
    f5 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 3000, 0, 1);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 900, 0, 1);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 500, 0, 1);
    f4 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 150, 0, 1);
    f9 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 70, 0, 1);

    addParamWithControl(gain1 = new Parameter("8 KHz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain6 = new Parameter("6.5 KHz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain5 = new Parameter("3 KHz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain2 = new Parameter("900 Hz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain3 = new Parameter("500 Hz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain4 = new Parameter("150 Hz", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain9 = new Parameter("70 Hz", 0.f, -24.0f, 48.0f, Units_dBGain));

    reset();
}

void GraphicEQ::handleParamUpdate(Parameter* param)
{
    if(param == gain5)
    {
        dspCoreEqualizer.setBandGain(f5, gain5->outVal);
    }
    else if(param == gain6)
    {
        dspCoreEqualizer.setBandGain(f6, gain6->outVal);
    }
    else if(param == gain1)
    {
        dspCoreEqualizer.setBandGain(f1, gain1->outVal);
    }
    else if(param == gain2)
    {
        dspCoreEqualizer.setBandGain(f2, gain2->outVal);
    }
    else if(param == gain3)
    {
        dspCoreEqualizer.setBandGain(f3, gain3->outVal);
    }
    else if(param == gain4)
    {
        dspCoreEqualizer.setBandGain(f4, gain4->outVal);
    }
    else if(param == gain9)
    {
        dspCoreEqualizer.setBandGain(f9, gain9->outVal);
    }
}

void GraphicEQ::reset()
{
    dspCoreEqualizer.reset();
}

void GraphicEQ::processData(float* in_buff, float* out_buff, int num_frames)
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

//
// EQ3 Class Implementation
//

EQ3::EQ3()
{
    objId = "eff.eq3";
    objName = "EQ3";
    uniqueId = MAKE_FOURCC('E','Q','0','3');

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 4000, 0);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 0, 0, 0);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 100, 0);


    addParamWithControl(freq2 = new Parameter("CENTERFREQ", Param_Freq, 0.6f, 0.f, 1.f, Units_Hz));
    addParamWithControl(bandwidth = new Parameter("BW", 1.5f, 0.25f, 3.75f, Units_Octave));

    addParamWithControl(gain1 = new Parameter("HIGH", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain2 = new Parameter("CENTER", 0.f, -24.0f, 48.0f, Units_dBGain));
    addParamWithControl(gain3 = new Parameter("LOW", 0.f, -24.0f, 48.0f, Units_dBGain));

/*
    addParamWithControl(freq1 = new FrequencyParameter("High freq", 0.5, Units_Hz));

    freq3 = new FrequencyParameter("Low freq", 0.5, Units_Hz);
    AddParamWithParamcell(freq3);
*/


    reset();
}

void EQ3::handleParamUpdate(Parameter* param)
{
    if(param == gain1)
    {
        dspCoreEqualizer.setBandGain(f1, gain1->outVal);
    }
    else if(param == gain2)
    {
        dspCoreEqualizer.setBandGain(f2, gain2->outVal);
    }
    else if(param == gain3)
    {
        dspCoreEqualizer.setBandGain(f3, gain3->outVal);
    }
    else if(param == freq1)
    {
        dspCoreEqualizer.setBandFrequency(f1, freq1->outVal);
    }
    else if(param == freq2)
    {
        dspCoreEqualizer.setBandFrequency(f2, freq2->outVal);
    }
    else if(param == freq3)
    {
        dspCoreEqualizer.setBandFrequency(f3, freq3->outVal);
    }
    else if(param == bandwidth)
    {
        dspCoreEqualizer.setBandBandwidth(f2, bandwidth->outVal);
    }
}

void EQ3::reset()
{
    dspCoreEqualizer.reset();
}

void EQ3::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "TR1";
    uniqueId = MAKE_FOURCC('T','R','E','M');

    addParamWithControl(speed = new Parameter("SPEED", 1.f, 0.1f, 2.25f));
    addParamWithControl(depth = new Parameter("DEPTH", 0.5f, 0.f, 1.0f));

    reset();
}

void CTremolo::handleParamUpdate(Parameter* param)
{
    if(param == speed)
    {
        dspCoreCTremolo.setCycleLength(speed->outVal);
    }
    else if(param == depth)
    {
        dspCoreCTremolo.setDepth(depth->outVal);
    }
}

void CTremolo::reset()
{
    dspCoreCTremolo.resetOscillatorPhases();
}

void CTremolo::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "CMP";
    uniqueId = MAKE_FOURCC('C','O','M','P');


    //addParamWithControl(mode = new BoolParam(false, "MODE", "LIMITER MODE"));
    addParamWithControl(new Parameter("THRESH", 0.f, -60.0f, 60.0f, Units_dB));
    addParamWithControl(new Parameter("RATIO", 1.0f, 1.0f, 30.0f));
    addParamWithControl(new Parameter("KNEE", 0.f, 0.0f, 48.0f, Units_dB));
    addParamWithControl(new Parameter("ATTACK", 10.f, 1.f, 90.0f, Units_ms));
    addParamWithControl(new Parameter("RELEASE", 100.f, 10.f, 900.0f, Units_ms));
    addParamWithControl(new Parameter("GAIN", 0.f, 0.f, 30.0f, Units_dB));

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
    if(param->prmName == "THRESH")
    {
        dspCoreComp.setThreshold((double)param->outVal);
    }
    else if(param->prmName == "KNEE")
    {
        dspCoreComp.setThreshold((double)param->outVal);
    }
    else if(param->prmName == "RATIO")
    {
        dspCoreComp.setRatio((double)param->outVal);
    }
    else if(param->prmName == "GAIN")
    {
        dspCoreComp.setOutputGain((double)param->outVal);
    }
    else if(param->prmName == "ATTACK")
    {
        dspCoreComp.setAttackTime(param->outVal);
    }
    else if(param->prmName == "RELEASE")
    {
        dspCoreComp.setReleaseTime(param->outVal);
    }
}

void Compressor::reset()
{
	dspCoreComp.reset();
}

void Compressor::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "WAH";
    uniqueId = MAKE_FOURCC('W','A','H','W');

    addParamWithControl(new Parameter("MODFREQ", 1.25f, 0.1f, 4.9f, Units_Hz1));
    addParamWithControl(new Parameter("DEPTH", 48.f, 2.f, 46.f, Units_Semitones));

    //addParamWithControl(frequency = new Parameter("FREQ.", Param_Freq, 0.5f, 0.f, 1.f, Units_Hz));
    //addParamWithControl(drywet = new Parameter("DRY/WET", 0.75f, 0.0f, 1.f, Units_DryWet));

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
    if(param->prmName == "MODFREQ")
    {
        dspCoreWah.setCycleLength(1.f/param->outVal);
    }
    else if(param->prmName == "DEPTH")
    {
        dspCoreWah.setDepth(param->outVal);
    }
}

void CWahWah::reset()
{
    dspCoreWah.reset();
}

void CWahWah::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "DIS";
    uniqueId = MAKE_FOURCC('D','I','S','T');

    addParamWithControl(new Parameter("DRIVE", 32.0f, 0.0f, 48.f, Units_dB));
    addParamWithControl(new Parameter("POSTGAIN", 0.0f, -48.0f, 48.f, Units_dB));
    addParamWithControl(new Parameter("SLOPE", 1.0f, -1.0f, 4.f));

    dspCoreDist.setOversampling(1);
    dspCoreDist.setAmount(10);
    dspCoreDist.setTransferFunction(3);

    reset();
}

void CDistort::handleParamUpdate(Parameter* param)
{
    if(param->prmName == "DRIVE")
    {
        dspCoreDist.setDrive(param->outVal);
    }
    else if(param->prmName == "POSTGAIN")
    {
        dspCoreDist.setOutputLevel(param->outVal);
    }
    else if(param->prmName == "SLOPE")
    {
        dspCoreDist.setPenticSlopeAtZero(param->outVal);
    }
}

void CDistort::reset()
{
	dspCoreDist.reset();
}

void CDistort::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "BC";
    uniqueId = MAKE_FOURCC('B','I','T','C');

    addParamWithControl(decimation = new Parameter("DECIMATION", 1.0f, 1.0f, 127.f, Units_Integer));
    addParamWithControl(quantization = new Parameter("QUANTIZATION", 0.0f, 0.0f, 1.f));

    dspCoreBC.setAmount(1);

    handleParamUpdate(decimation);
    handleParamUpdate(quantization);
}

void CBitCrusher::handleParamUpdate(Parameter* param)
{
    if(param == decimation)
    {
        dspCoreBC.setDecimationFactor(int(decimation->outVal));
    }
    else if(param == quantization)
    {
        dspCoreBC.setQuantizationInterval(0.001f*pow(1000.0f, quantization->outVal));
    }
}

void CBitCrusher::reset()
{
    dspCoreBC.reset();
}

void CBitCrusher::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "STR";
    uniqueId = MAKE_FOURCC('S','T','E','R');

    addParamWithControl(offset = new Parameter("OFFSET", 10.0f, 1.0f, 99.f, Units_Integer));

    handleParamUpdate(offset);
}

void CStereo::handleParamUpdate(Parameter* param)
{
    if(param == offset)
    {
        dspCoreStereo.delayLine.setDelayInMilliseconds(offset->outVal);
    }
}

void CStereo::reset()
{
    //dspCoreStereo.reset();
}

void CStereo::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "DL1";
    uniqueId = MAKE_FOURCC('P','P','D','L');

    dspCorePingPongDelay.setTrueStereoMode(true);
    dspCorePingPongDelay.setPingPongMode(true);
    dspCorePingPongDelay.setStereoSwap(true);

    ppmode = new BoolParam("Mode", true, "Ping-pong");

    addParamWithControl(delay = new Parameter("DELAY", 3, 0.5f, 19.5f, Units_Ticks));
    addParamWithControl(new Parameter("AMOUNT", 1.f, 0.f, 1.f, Units_Percent));
    addParamWithControl(new Parameter("FEEDBACK", 55.f, 0.0f, 100.f, Units_Percent));
    addParamWithControl(new Parameter("PAN", 0.0f, -1.0f, 2.f));

    delay->setInterval(0.25f);

    //highCut = new FrequencyParameter(0.2f);
    //highCut->SetName("HighCut");
    //highCut->AddValueString(Units_Hz);
    //AddParamWithParamcell(highCut);

    addParamWithControl(new Parameter("LOWCUT", Param_Freq, 0.9f, 0.f, 1.f, Units_Hz));
    addParamWithControl(new Parameter("DRY/WET", .4f, 0.0f, 1.f, Units_DryWet));

    reset();
}

void XDelay::handleParamUpdate(Parameter* param)
{
    if(param == ppmode)
    {
        dspCorePingPongDelay.setPingPongMode(ppmode->getOutVal());
        dspCorePingPongDelay.setStereoSwap(ppmode->getOutVal());
        dspCorePingPongDelay.setPan((ppmode->getOutVal() ? -1 : 1)*param->getOutVal());
    }
    else if(param->prmName == "AMOUNT")
    {
        //dspCorePingPongDelay.setGlobalGainFactor(ggain->outval); // old, obsolete
        dspCorePingPongDelay.setWetLevel(amp2dB(param->getOutVal()));
        param->setValString(param->calcValStr(int(100*param->getOutVal())));
    }
    /*
    else if( param->prmName == "High.cut" )
    {
        dspCorePingPongDelay.setLowDamp(param->outval);
    }
    */
    else if(param->prmName == "DELAY")
    {
        dspCorePingPongDelay.setDelayTime(param->outVal/MTransp->getTicksPerBeat());
    }
    else if( param->prmName == "DRY/WET" )
    {
        dspCorePingPongDelay.setDryWetRatio((float)(param->outVal));
    }
    else if( param->prmName == "FEEDBACK" )
    {
        dspCorePingPongDelay.setFeedbackInPercent(param->outVal);
    }
    else if( param->prmName == "PAN" )
    {
        dspCorePingPongDelay.setPan((ppmode->outval == true ? -1 : 1)*param->outVal);
    }
    else if( param->prmName == "LOWCUT" )
    {
        dspCorePingPongDelay.setHighDamp(param->outVal);
    }
}

void XDelay::reset()
{
    dspCorePingPongDelay.reset();
}

void XDelay::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "RV1";
    uniqueId = MAKE_FOURCC('R','E','V','R');

    addParamWithControl(preDelay  = new Parameter("PREDELAY", 0.0f, 0.0f, 200.0f, Units_ms));
    addParamWithControl(roomsize = new Parameter("ROOM", 100.0f, 4.0f, 96.0f, Units_Integer));
    addParamWithControl(decay = new Parameter("DECAY", 6.f, 1.0f, 14.f, Units_Seconds));
    addParamWithControl(highCut = new Parameter("HIGHCUT", Param_Freq, 0.2f, 0.f, 1.f, Units_Hz));
    addParamWithControl(lowCut = new Parameter("LOWCUT", Param_Freq, 1.0f, 0.f, 1.f, Units_Hz));
    addParamWithControl(lowscale = new Parameter("LOW.SCALE", Param_Log, 0.5f, 0.1f, 100));
    addParamWithControl(highscale = new Parameter("HIGH.SCALE", Param_Log, 0.24f, 0.1f, 100));
    addParamWithControl(drywet = new Parameter("DRY/WET", .5f, 0.0f, 1.f, Units_DryWet));

    reset();
}

void CReverb::handleParamUpdate(Parameter* param)
{
    if(param == roomsize)
    {
        dspCoreReverb.setReferenceDelayTime(roomsize->outVal);
    }
    else if(param == preDelay)
    {
        dspCoreReverb.setPreDelay(preDelay->outVal);
    }
    else if(param == drywet)
    {
        dspCoreReverb.setDryWetRatio((float)(drywet->outVal));
    }
    else if(param == decay)
    {
        dspCoreReverb.setMidReverbTime(decay->outVal);
    }
    else if(param == highCut)
    {
        dspCoreReverb.setWetHighpassCutoff(highCut->outVal);
    }
    else if(param == lowCut)
    {
        dspCoreReverb.setWetLowpassCutoff(lowCut->outVal);
    }
    else if(param == highscale)
    {
        dspCoreReverb.setHighReverbTimeScale(highscale->outVal);
    }
    else if(param == lowscale)
    {
        dspCoreReverb.setLowReverbTimeScale(lowscale->outVal);
    }
}

void CReverb::reset()
{
    dspCoreReverb.reset();
}

void CReverb::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "CHO";
    uniqueId = MAKE_FOURCC('C','H','O','R');

    dspCoreChorus = new rosic::Chorus(65535);
    dspCoreChorus->setTempoSync(false);

    addParamWithControl(delay = new Parameter("DELAY", 5.f, 1.f, 49.0f, Units_ms));
    addParamWithControl(freq = new Parameter("MODFREQ", 2.f, 0.1f, 4.9f, Units_Hz2));
    addParamWithControl(depth = new Parameter("DEPTH", 0.25f, 0, 1.5f, Units_Semitones));
    addParamWithControl(drywet = new Parameter("DRY/WET", 0.5f, 0.0f, 1.f, Units_DryWet));
}

void CChorus::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCoreChorus->setDryWetRatio(drywet->outVal);
    }
    else if(param == delay)
    {
        dspCoreChorus->setAverageDelayTime(delay->outVal);
    }
    else if(param == freq)
    {
        dspCoreChorus->setCycleLength(1.f/freq->outVal);
    }
    else if(param == depth)
    {
        dspCoreChorus->setDepth(depth->outVal);
    }
}

void CChorus::reset()
{
    //dspCoreChorus.reset();
}

void CChorus::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "FL1";
    uniqueId = MAKE_FOURCC('F','L','N','G');

    dspCoreFlanger.setTempoSync(false);

    //fmemory = (float*)malloc(65535);
    //dspCoreFlanger.setSharedMemoryAreaToUse(fmemory, 65535); -> not needed anymore - Flanger allocates its own memory now

    reset();

    addParamWithControl(frequency = new Parameter("DELAY", Param_Freq, 0.5f, 0.f, 1.f, Units_ms2));

    frequency->setReversed(true);

    addParamWithControl(modfreq = new Parameter("MODFREQ", 3.f, 0.1f, 9.9f, Units_Seconds));
    addParamWithControl(feedback = new Parameter("FEEDBACK", 0, -1.f, 2.f));
    addParamWithControl(depth = new Parameter("DEPTH", 24.f, 2.f, 46.f, Units_Semitones));

    //    invert = new BoolParam("Invert wet signal", false, "Invert wet signal");
    //    AddParamWithParamcell(invert);

    addParamWithControl(drywet = new Parameter("DRY/WET", 0.5f, 0.0f, 0.5f, Units_DryWet));
}

void CFlanger::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCoreFlanger.setDryWetRatio(drywet->outVal);
    }
    else if(param == frequency)
    {
        dspCoreFlanger.setFrequency(frequency->outVal);

        frequency->setValString(frequency->calcValStr(1.f/frequency->outVal*1000));
    }
    else if(param == modfreq)
    {
        dspCoreFlanger.setCycleLength(modfreq->outVal);
    }
    else if(param == depth)
    {
        dspCoreFlanger.setDepth(depth->outVal);
    }
    else if(param == feedback)
    {
        dspCoreFlanger.setFeedbackFactor(feedback->outVal);
    }
    else if(param == invert)
    {
        dspCoreFlanger.setNegativePolarity(invert->outval);
    }
}

void CFlanger::reset()
{
    dspCoreFlanger.reset();
}

void CFlanger::processData(float* in_buff, float* out_buff, int num_frames)
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
    objName = "PH1";
    uniqueId = MAKE_FOURCC('P','H','A','S');

    dspCorePhaser.setTempoSync(false);

    //fmemory = (float*)malloc(65535);
    //dspCorePhaser.setSharedMemoryAreaToUse(fmemory, 65535);

    reset();

    addParamWithControl(frequency = new Parameter("DELAY", Param_Freq, 0.5f, 0.f, 1.f, Units_ms2));
    frequency->setReversed(true);
    addParamWithControl(modfreq = new Parameter("MODFREQ", 3.f, 0.1f, 9.9f, Units_Seconds));
    addParamWithControl(feedback = new Parameter("FEEDBACK", 0, -1.f, 2.f));
    addParamWithControl(depth = new Parameter("DEPTH", 24.f, 2.f, 46.f, Units_Semitones));
    addParamWithControl(numstages = new Parameter("STAGES", 4, 1, 23, Units_Integer));
    numstages->setInterval(1);
    addParamWithControl(stereo = new Parameter("STEREO", 0, 0, 180));
    stereo->setInterval(1);

    addParamWithControl(drywet = new Parameter("DRY/WET", 0.5f, 0.0f, 0.5f, Units_DryWet));

    dspCorePhaser.setFilterMode(rosic::AllpassChain::FIRST_ORDER_ALLPASS);
}

void CPhaser::handleParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCorePhaser.setDryWetRatio(drywet->outVal);
    }
    else if(param == frequency)
    {
        dspCorePhaser.setFrequency(frequency->outVal);

        frequency->setValString(frequency->calcValStr(1.f/frequency->outVal*1000));
    }
    else if(param == modfreq)
    {
        dspCorePhaser.setCycleLength(modfreq->outVal);
    }
    else if(param == depth)
    {
        dspCorePhaser.setDepth(depth->outVal);
    }
    else if(param == feedback)
    {
        dspCorePhaser.setFeedbackFactor(feedback->outVal);
    }
    else if(param == numstages)
    {
        dspCorePhaser.setNumStages(int(numstages->outVal));
    }
    else if(param == stereo)
    {
        dspCorePhaser.setStereoPhaseOffsetInDegrees((stereo->outVal));
        dspCorePhaser.resetOscillatorPhases();
    }
}

void CPhaser::reset()
{
    dspCorePhaser.reset();
}

void CPhaser::processData(float* in_buff, float* out_buff, int num_frames)
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


