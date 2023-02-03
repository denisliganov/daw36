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
#include "36_devwin.h"




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

    addObject(previewButton = new EffGuiButton(), MixChanWidth - 21, 0, 20, 15);

    //addParamWithControl(new Parameter(1, 0, 1), "", sliderAmount = new Slider36(false));
}

Eff::~Eff()
{
    ///
}

void Eff::remap()
{
    //previewButton->setCoords1(width - 13, 0, 12, 12);

    //sliderAmount->setCoords2(4, height - 12, 30, height - 4);
}

void Eff::drawSelf(Graphics& g)
{
    fill(g, .5f);

    setc(g, .45f);
    fillx(g, 0, 0, width, height/2);

    //rect(g, .3f);

    setc(g, .2f);
    lineH(g, height-1, 0, width-1);

    if (guiWindow && guiWindow->isOpen())
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

    int th = gGetTextHeight(FontSmall);

    setc(g, .0f);
    txtfit(g, FontSmall, objName, 3, th - 2, width - 2);

    setc(g, 1.f);
    txtfit(g, FontSmall, objName, 1, th - 4, width - 2);
}

Eff* Eff::makeClone(Eff* eff)
{
    auto itr1 = params.begin();
    auto itr2 = eff->params.begin();

    for(; itr1 != params.end(); itr1++, itr2++)
    {
        ((Parameter*)(*itr2))->setValue(((Parameter*)(*itr1))->getValue());
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

void Eff::activateMenuItem(std::string item)
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
    if(obj == previewButton)
    {
        showWindow(previewButton->isPressed());
    }
}

SubWindow* Eff::createWindow()
{
    SubWindow* win =  window->addWindow(new DevParamObject(this));

    //int xb = window->getLastEvent().mouseX + 20;
    //int yb = window->getLastEvent().mouseY - guiWindow->getHeight()/2;

    win->setBounds(window->getLastEvent().mouseX + 40, win->getY(), win->getWidth(), win->getHeight());

    return win;
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

    addParam(filterType = new ParamRadio("Type"));

    filterType->addOption("LowPass");
    filterType->addOption("HighPass");
    filterType->addOption("BandPass");
    filterType->setCurrent(0);

    addParam(x2toggle = new ParamToggle("X2", false));

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

void Filter1::handleParamUpdate(Param* param)
{
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
    else if(param == cutoff)
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

void EQ1::handleParamUpdate(Param* param)
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

void GraphicEQ::handleParamUpdate(Param* param)
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

void EQ3::handleParamUpdate(Param* param)
{
    if(param == gain1)
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
    else if(param == freq1)
    {
        dspCoreEqualizer.setBandFrequency(f1, freq1->getOutVal());
    }
    else if(param == freq2)
    {
        dspCoreEqualizer.setBandFrequency(f2, freq2->getOutVal());
    }
    else if(param == freq3)
    {
        dspCoreEqualizer.setBandFrequency(f3, freq3->getOutVal());
    }
    else if(param == bandwidth)
    {
        dspCoreEqualizer.setBandBandwidth(f2, bandwidth->getOutVal());
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

void CTremolo::handleParamUpdate(Param* param)
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

    addParamWithControl(thresh = new Parameter("THRESH", 0.f, -60.0f, 60.0f, Units_dB));
    addParamWithControl(ratio = new Parameter("RATIO", 1.0f, 1.0f, 30.0f));
    addParamWithControl(knee = new Parameter("KNEE", 0.f, 0.0f, 48.0f, Units_dB));
    addParamWithControl(attack = new Parameter("ATTACK", 10.f, 1.f, 90.0f, Units_ms));
    addParamWithControl(release = new Parameter("RELEASE", 100.f, 10.f, 900.0f, Units_ms));
    addParamWithControl(gain = new Parameter("GAIN", 0.f, 0.f, 30.0f, Units_dB));

/* // Unpredictable shit
    autogain = new BoolParam(false);
    autogain->SetName("Autogain");
    AddParamWithParamcell(autogain);
*/
}

void Compressor::handleParamUpdate(Param* param)
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

    addParamWithControl(modfreq = new Parameter("MODFREQ", 1.25f, 0.1f, 4.9f, Units_Hz1));
    addParamWithControl(depth = new Parameter("DEPTH", 48.f, 2.f, 46.f, Units_Semitones));

    //addParamWithControl(frequency = new Parameter("FREQ.", Param_Freq, 0.5f, 0.f, 1.f, Units_Hz));
    //addParamWithControl(drywet = new Parameter("DRY/WET", 0.75f, 0.0f, 1.f, Units_DryWet));

    reset();
}

void CWahWah::handleParamUpdate(Param* param)
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

    addParamWithControl(drive = new Parameter("DRIVE", 32.0f, 0.0f, 48.f, Units_dB));
    addParamWithControl(postgain = new Parameter("POSTGAIN", 0.0f, -48.0f, 48.f, Units_dB));
    addParamWithControl(slope = new Parameter("SLOPE", 1.0f, -1.0f, 4.f));

    dspCoreDist.setOversampling(1);
    dspCoreDist.setAmount(10);
    dspCoreDist.setTransferFunction(3);

    reset();
}

void CDistort::handleParamUpdate(Param* param)
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

void CBitCrusher::handleParamUpdate(Param* param)
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

void CStereo::handleParamUpdate(Param* param)
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

    addParam(delayMode = new ParamToggle("PING-PONG MODE", true));

    addParamWithControl(delay = new Parameter("DELAY", 3, 0.5f, 19.5f, Units_Ticks));
    delay->setInterval(0.25f);
    addParamWithControl(amount = new Parameter("AMOUNT", 1.f, 0.f, 1.f, Units_Percent));
    addParamWithControl(feedback = new Parameter("FEEDBACK", 55.f, 0.0f, 100.f, Units_Percent));
    addParamWithControl(pan = new Parameter("PAN", 0.0f, -1.0f, 2.f));

    //highCut = new FrequencyParameter(0.2f);
    //highCut->SetName("HighCut");
    //highCut->AddValueString(Units_Hz);
    //AddParamWithParamcell(highCut);

    addParamWithControl(lowcut = new Parameter("LOWCUT", Param_Freq, 0.9f, 0.f, 1.f, Units_Hz));
    addParamWithControl(drywet = new Parameter("DRY/WET", .4f, 0.0f, 1.f, Units_DryWet));

    reset();
}

void XDelay::handleParamUpdate(Param* param)
{
    if(param == delayMode)
    {
        dspCorePingPongDelay.setPingPongMode(delayMode->getValue());
        dspCorePingPongDelay.setStereoSwap(delayMode->getValue());
        dspCorePingPongDelay.setPan((delayMode->getValue() ? -1 : 1)*delayMode->getValue());
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
        dspCorePingPongDelay.setPan((delayMode->getValue() == true ? -1 : 1)*pan->getOutVal());
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

void CReverb::handleParamUpdate(Param* param)
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

void CChorus::handleParamUpdate(Param* param)
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

void CFlanger::handleParamUpdate(Param* param)
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

void CPhaser::handleParamUpdate(Param* param)
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


