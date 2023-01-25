


#include "36_mixer.h"
#include "36_slider.h"
#include "36_button.h"
#include "36_params.h"
#include "36_vu.h"
#include "36_browser.h"
#include "36_effects.h"
#include "36_vst.h"
#include "36_utils.h"
#include "36_env.h"
#include "36_scroller.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_menu.h"
#include "36_project.h"
#include "36_draw.h"
#include "36_keyboard.h"
#include "36_audio_dev.h"
#include "36_brwentry.h"
#include "36_dragndrop.h"
#include "36_text.h"
#include "36_vsteff.h"
#include "36_knob.h"





Eff* CreateEffect(std::string effalias)
{
    Eff* eff = NULL;

    if(effalias == "eff.eq1")
    {
        eff = new EQ1();
    }
    else if(effalias == "eff.eq3")
    {
        eff = new EQ3();
    }
    else if(effalias == "eff.grapheq")
    {
        eff = new GraphicEQ();
    }
    else if(effalias == "eff.delay")
    {
        eff = new XDelay();
    }
    else if(effalias == "eff.reverb")
    {
        eff = new CReverb();
    }
    else if(effalias == "eff.tremolo")
    {
        eff = new CTremolo();
    }
    else if(effalias == "eff.comp")
    {
        eff = new Compressor();
    }
    else if(effalias == "eff.chorus")
    {
        eff = new CChorus();
    }
    else if(effalias == "eff.flanger")
    {
        eff = new CFlanger();
    }
    else if(effalias == "eff.phaser")
    {
        eff = new CPhaser();
    }
    else if(effalias == "eff.wah")
    {
        eff = new CWahWah();
    }
    else if(effalias == "eff.dist")
    {
        eff = new CDistort();
    }
    else if(effalias == "eff.bitcrush")
    {
        eff = new CBitCrusher();
    }
    else if(effalias == "eff.stereo")
    {
        eff = new CStereo();
    }
    else if(effalias == "eff.filter1")
    {
        eff = new Filter1();
    }

    return eff;
}


MixChannel::MixChannel()
{
    init(NULL);
}

MixChannel::MixChannel(Instrument* i)
{
    init(i);
}

MixChannel::~MixChannel()
{
    //
}

void MixChannel::init(Instrument* ins)
{
    objId = "mixchan";

    master = false;

    voffs = 0;
    contentheight = 0;
    mutecount = 0;

    if(ins != NULL)
    {
        instr = ins;

        int slLen = MixChanWidth - 10;

        //addObject(volslider = new Slider36(true), "sl.vol");
        //addObject(panslider = new Slider36(false), "sl.pan");
        //addParamWithControl(volParam = new Parameter("Volume", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB), "sl.vol", volslider);
        //addParamWithControl(panParam = new Parameter("Panning", Param_Pan, 0.f, -1.f, 2.f, Units_Percent), "sl.pan", panslider);

        addParam(volParam = new Parameter("Volume", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB));
        addParam(panParam = new Parameter("Panning", Param_Pan, 0.f, -1.f, 2.f, Units_Percent));

        addObject(volKnob = new Knob(volParam));
        addObject(panKnob = new Knob(panParam));

        muteparam = NULL; // instr->muteparam;
        soloparam = NULL; // instr->soloparam;

        // Add send knobs for instrument channels

        for(int mc = 0; mc < NUM_SENDS; mc++)
        {
            sends[mc].amount = new Parameter("sendchan.amount", 0.0f, 0.0f, 1.0f);
        }
    }
    else    // send or master
    {
        instr = NULL;
        mchanout = NULL;
        muteparam = NULL;
        soloparam = NULL;
        mutetoggle = NULL;
        solotoggle = NULL;
        volslider = NULL;
        panslider = NULL;
        volKnob = NULL;
        volKnob = NULL;

        addParamWithControl(volParam = new Parameter("Volume", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB), "sl.vol", volslider = new Slider36(true));
        addParamWithControl(panParam = new Parameter("Panning", Param_Pan, 0.f, -1.f, 2.f, Units_Percent), "sl.pan", panslider = new Slider36(true));

        for(int mc = 0; mc < NUM_SENDS; mc++)
        {
            sends[mc].amount = NULL;
            sends[mc].r_amount = NULL;
        }
    }

    addObject(vu = new ChanVU(true), ObjGroup_VU);

    defineMonoColor(.08f);
}

void MixChannel::remap()
{
    //if(volslider)
    //    volslider->setCoords1(width - 30, 1, 10, height - 2);

    confine();

    //if(volKnob)
    //    volKnob->setCoords1(width - 30, 0, 22, 22);
    //if(panKnob)
    //    panKnob->setCoords1(width - 30, 22, 22, 22);

    //if(panslider)
    //    panslider->setCoords1(width - 18, 1, 10, height - 2);


    //if(vu)
    //    vu->setCoords1(1, 1, 10, height - 2);

    confine(0, 0, width - 1, height);

    int xeff = 0;

    for(Eff* eff : effs)
    {
        eff->setCoords1(xeff, 1, 32, height - 2);

        xeff += eff->getW() + 1;
    }

/*
    confine(1, 1, MixChanWidth - 1, height - MixChannelPadHeight - 3);

    int yeff = 1 - voffs;

    for(Eff* eff : effs)
    {
        eff->setCoords1(1, yeff, MixChanWidth - 2, EffHeaderHeight);

        yeff += eff->getH() + 2;
    }
*/
}

void MixChannel::drawself(Graphics& g)
{
    int baseheight = MixChannelPadHeight;

    fill(g, .25f);

    setc(g, 1.f);

    gTextFit(g, FontSmall, instr->getObjName(), 0, 5, width);

    gText(g, FontSmall, "123", 0, 5);

    if(instr != NULL)
    {
    /*
        instr->setDrawColor(g, .47f);

        gFillRect(g, x1 + 1, y1 + 2, x2 - 1, y1 + MixerTopHeight - 2);
        //gSetColor2(g, instr->getColor(), .57f);
        //gLineHorizontal(g, y1 + 2, x1 + 1, x2 - 1);

        if (instr == MInstrPanel->getCurrInstr())
        {
            gSetColor(g, 0xffFFDD60);
            gLineHorizontal(g, getY1(), getX1(), getX2());
            gSetColor(g, 0x9fFFDD60);
            gLineHorizontal(g, getY1() + 1, getX1(), getX2());

            gSetColor(g, 0xffFFDD60);
        }
        else
        {
            instr->setDrawColor(g, .8f);
        }

        std::string str = instr->instrAlias;

        gTextFit(g, FontSmall, instr->getName(), x1 + 4, y1 + MixerTopHeight - 4, width - 4);*/
    }
    else
    {
    /*
        gSetMonoColor(g, 0.7f);

        if(master)
        {
            gText(g, FontFix, "MASTER", x1 + width/2 - gGetTextWidth(FontFix, "MASTER")/2, y1 + MixerTopHeight - 4);
        }
        else
        {
            gText(g, FontFix, chanTitle,  x1 + width/2 - gGetTextWidth(FontFix, chanTitle)/2, y1 + MixerTopHeight - 4);
        }
        */
    }
}

void MixChannel::addEffect(Eff* eff)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    effs.push_back(eff);

    eff->setMixChannel(this);

    addObject(eff);

    remapAndRedraw();
    
    ReleaseMutex(MixerMutex);
}

void MixChannel::removeEffect(Eff* eff)
{
    effs.remove(eff);

    if(effs.empty())
    {
        voffs = 0;
    }

    eff->setMixChannel(NULL);

    removeObject(eff);

    remapAndRedraw();
}

void MixChannel::process(int num_frames, float* out_buff)
{
    if(effs.size() > 0)
    {
        for(Eff* eff : effs)
        {
            eff->process(inbuff, outbuff, num_frames);

            if(eff->bypass == false)
            {
                // Copy output back to input for the next effect to process

                if(eff->muteCount > 0)
                {
                    long tc = 0;
                    float aa;

                    while(tc < num_frames)
                    {
                        aa = float(DECLICK_COUNT - eff->muteCount)/DECLICK_COUNT;

                        inbuff[tc*2] = inbuff[tc*2]*(1.f - aa) + outbuff[tc*2]*aa;
                        inbuff[tc*2 + 1] = inbuff[tc*2 + 1]*(1.f - aa) + outbuff[tc*2 + 1]*aa;

                        tc++;

                        if(eff->muteCount > 0)
                        {
                            eff->muteCount--;
                        }
                    }
                }
                else
                {
                    memcpy(inbuff, outbuff, sizeof(float)*num_frames*2);
                }
            }
            else
            {
                if(eff->muteCount < DECLICK_COUNT)
                {
                    long tc = 0;
                    float aa;

                    while(tc < num_frames && eff->muteCount < DECLICK_COUNT)
                    {
                        aa = float(DECLICK_COUNT - eff->muteCount)/DECLICK_COUNT;

                        inbuff[tc*2] = inbuff[tc*2]*(1.f - aa) + outbuff[tc*2]*aa;
                        inbuff[tc*2 + 1] = inbuff[tc*2 + 1]*(1.f - aa) + outbuff[tc*2 + 1]*aa;

                        tc++;

                        eff->muteCount++;
                    }
                }
            }
        }
    }
    else
    {
        memcpy(outbuff, inbuff, sizeof(float)*num_frames*2);
    }

    {
        float panv, volv, volL, volR;

        bool off = false;

        if(muteparam != NULL)
        {
            if(!muteparam->getOutVal() || (SoloMixChannel != NULL && SoloMixChannel != this))
            {
                off = true;
            }
        }

        bool fill;

        if(off == false || mutecount < DECLICK_COUNT)
        {
            fill = true;
        }
        else
        {
            fill = false;
        }

        if(fill)
        {
            Envelope* venv = NULL;
            Envelope* penv = NULL;

            if(volParam != NULL && panParam != NULL)
            {
                if(volParam->envelopes != NULL)
                {
                    //venv = (Envelope*)((Command*)volParam->envelopes->el)->paramedit;
                }

                if(panParam->envelopes != NULL)
                {
                    ///penv = (Envelope*)((Command*)panParam->envelopes->el)->paramedit;
                }

                volv = volParam->outVal;

                if(volParam->lastval == -1)
                {
                    volParam->setLastVal(volParam->outVal);
                }
                else if(volParam->lastval != volParam->outVal)
                {
                    if(volParam->declickCount == 0)
                    {
                        volParam->declickCoeff = float(volParam->outVal - volParam->lastval)/DECLICK_COUNT;

                        volv = volParam->lastval;

                        volParam->declickCount = DECLICK_COUNT;
                    }
                    else
                    {
                        volv = volParam->lastval + (DECLICK_COUNT - volParam->declickCount)*volParam->declickCoeff;
                    }
                }
                else if(volParam->declickCount > 0) // (params->volparam->lastval == params->volparam->outval)
                {
                    volParam->declickCount = 0;
                    volParam->declickCoeff = 0;
                }

                panv = panParam->outVal;
            }
            else
            {
                volv = 1;
                panv = 0;
            }

            PanLinearRule(panv, &volL, &volR);

            float aa = 1;
            float lMax, rMax, outL, outR;

            lMax = rMax = 0;

            float mL, mR;
            long tc = 0;

            for(long cc = 0; cc < num_frames; cc++)
            {
                if(venv != NULL && volParam->declickCount == 0)
                {
                    volv = venv->buffoutval[cc];
                }

                if(volParam->declickCount > 0)
                {
                    volv += volParam->declickCoeff;

                    volParam->declickCount--;

                    if(volParam->declickCount == 0)
                    {
                        volParam->setLastVal(volParam->outVal);
                    }
                }

                if(penv != NULL)
                {
                    panv = penv->buffoutval[cc];

                    PanLinearRule(panv, &volL, &volR);
                }

                if(off == true)
                {
                    // Fadeout case

                    if(mutecount < DECLICK_COUNT)
                    {
                        aa = float(DECLICK_COUNT - mutecount)/DECLICK_COUNT;

                        mutecount++;
                    }
                    else
                    {
                        aa = 0;
                    }
                }
                else
                {
                    // Fadein case

                    if(mutecount > 0)
                    {
                        aa = float(DECLICK_COUNT - mutecount)/DECLICK_COUNT;

                        mutecount--;
                    }
                    else if(mutecount == 0)
                    {
                        aa = 1;
                    }
                }

                if(volL > volR)
                {
                    mR = (volL - volR)*inbuff[tc + 1];
                }
                else
                {
                    mR = 0;
                }

                if(volR > volL)
                {
                    mL = (volR - volL)*inbuff[tc];
                }
                else
                {
                    mL = 0;
                }

                outL = inbuff[tc]*volL + mR;
                outR = inbuff[tc + 1]*volR + mL;

                outL *= volv*aa;
                outR *= volv*aa;

                if(outL > lMax)
                {
                    lMax = outL;
                }

                if(outR > rMax)
                {
                    rMax = outR;
                }

                if(out_buff != NULL)
                {
                    out_buff[tc++] += outL;
                    out_buff[tc++] += outR;
                }
                else // master
                {
                    inbuff[tc++] = outL;
                    inbuff[tc++] = outR;
                }
            }

            if(vu)
                vu->setValues(lMax, rMax);
        }
        else
        {
            //memset(out_buff, 0, num_frames*sizeof(float)*2);
        }
    }
}

void MixChannel::doSend(float * sendbuff, float amount, int num_frames)
{
    if(sendbuff != NULL)
    {
        float outL, outR;

        int i = 0;
        int fc = 0;

        while(i < num_frames)
        {
            outL = outbuff[fc];
            outR = outbuff[fc + 1];

            sendbuff[fc++] += outL*amount;
            sendbuff[fc++] += outR*amount;

            i++;
        }
    }
}

void MixChannel::save(XmlElement * xmlChanNode)
{
    //xmlChanNode->setAttribute(T("Index"), instr->instrAlias);

    xmlChanNode->addChildElement(volParam->save());
    xmlChanNode->addChildElement(panParam->save());

    //xmlChanNode->addChildElement(amount1->save());

    xmlChanNode->setAttribute(T("Mute"), muteparam->getOutVal() ? 1 : 0);
    xmlChanNode->setAttribute(T("Solo"), soloparam->getOutVal() ? 1 : 0);
    xmlChanNode->setAttribute(T("VOffset"), voffs);

    for(Eff* eff : effs)
    {
        XmlElement* xmlEff = new XmlElement(T("Effect"));

        eff->save(xmlEff);

        xmlChanNode->addChildElement(xmlEff);
    }

    /*
    Parameter* pParam = this->firstParam;
    while (pParam != NULL)
    {
        if(pParam->IsPresetable())
        {
            XmlElement * xmlParam = pParam->save();
            xmlChanNode->addChildElement(xmlParam);
        }
        pParam = pParam->next;
    }
    */
}

void MixChannel::load(XmlElement * xmlNode)
{
    voffs = xmlNode->getIntAttribute(T("VOffset"));
    bool bval = xmlNode->getBoolAttribute(T("Mute"));
    muteparam->SetBoolValue(bval);
    bval = xmlNode->getBoolAttribute(T("Solo"));
    soloparam->SetBoolValue(bval);

    if(soloparam->getOutVal())
        SoloMixChannel = this;

    XmlElement* xmlParam = NULL;

    forEachXmlChildElementWithTagName(*xmlNode, xmlParam, T("Parameter"))
    {
        String sname = xmlParam->getStringAttribute(T("devName"));

        if(sname == T("Vol"))
            volParam->load(xmlParam);
        else if(sname == T("Pan"))
            panParam->load(xmlParam);

        /*
        else if(sname == T("send1.amount"))
        {
            amount1->load(xmlParam);
        }
        */
    }
}

ContextMenu* MixChannel::createmenu()
{
    ContextMenu* menu = new ContextMenu(this);

    menu->addMenuItem("1-band Equalizer");
    menu->addMenuItem("3-band Equalizer");
    menu->addMenuItem("Graphic Equalizer");
    menu->addMenuItem("Delay");
    menu->addMenuItem("Reverb");
    menu->addMenuItem("Flanger");
    menu->addMenuItem("Phaser");
    menu->addMenuItem("Filter");
    menu->addMenuItem("WahWah");
    menu->addMenuItem("Distortion");
    menu->addMenuItem("Compressor");
    menu->addMenuItem("Stereo Expander");

    return menu;
}

ContextMenu* MixChannel::createContextMenuForEffect(Eff* eff)
{
    ContextMenu* menu = new ContextMenu(eff);

    menu->addMenuItem("Delete");
    menu->addMenuItem("Clone");
    menu->addMenuItem("Replace");

    return menu;
}

void MixChannel::activateEffectMenuItem(Eff * eff, std::string mi)
{
    if(mi == "Delete")
    {
        deleteEffect(eff);
    }
}

void MixChannel::activatemenuitem(std::string mi)
{
    Eff* eff = NULL;

    if(mi == "1-band Equalizer")
    {
        eff = CreateEffect("eff.eq1");
    }
    else if(mi == "3-band Equalizer")
    {
        eff = CreateEffect("eff.eq3");
    }
    else if(mi == "Graphic Equalizer")
    {
        eff = CreateEffect("eff.grapheq");
    }
    else if(mi == "Delay")
    {
        eff = CreateEffect("eff.delay");
    }
    else if(mi == "Reverb")
    {
        eff = CreateEffect("eff.reverb");
    }
    else if(mi == "Flanger")
    {
        eff = CreateEffect("eff.flanger");
    }
    else if(mi == "Phaser")
    {
        eff = CreateEffect("eff.phaser");
    }
    else if(mi == "Filter")
    {
        eff = CreateEffect("eff.filter1");
    }
    else if(mi == "WahWah")
    {
        eff = CreateEffect("eff.wah");
    }
    else if(mi == "Distortion")
    {
        eff = CreateEffect("eff.dist");
    }
    else if(mi == "Compressor")
    {
        eff = CreateEffect("eff.comp");
    }
    else if(mi == "Stereo Expander")
    {
        eff = CreateEffect("eff.stereo");
    }

    if(eff != NULL)
    {
        WaitForSingleObject(MixerMutex, INFINITE);

        addEffect(eff);

        MMixer->setCurrentEffect(eff);

        ReleaseMutex(MixerMutex);
    }

    redraw();
}

void MixChannel::deleteEffect(Eff* eff)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    if(eff == MMixer->getCurrentEffect())
    {
        MMixer->setCurrentEffect(NULL);
    }

    effs.remove(eff);

    deleteObject(eff);

    remapAndRedraw();

    MProject.setChange();

    ReleaseMutex(MixerMutex);
}

Eff* MixChannel::addEffectFromBrowser(BrwEntry * de)
{
    Eff* eff = NULL;

    if(de->isExternal())
    {
        VstEffect* vsteff = new VstEffect((char*)de->path.data());

        eff = vsteff;
    }
    else
    {
        eff = CreateEffect(de->alias);
    }

    if(eff != NULL)
    {
        addEffect(eff);

        MMixer->setCurrentEffect(eff);
    }

    return eff;
}

void MixChannel::handleMouseWheel(InputEvent& ev)
{
    MInstrPanel->setOffset((int)(MInstrPanel->getOffset() - ev.wheelDelta*int(InstrHeight*1.1f)));
}

void MixChannel::handleMouseDown(InputEvent& ev)
{
    if(instr != NULL)
    {
        MInstrPanel->setcurr(instr);

        if(ev.keyFlags & kbd_ctrl && ev.mouseY < y1 + MixerTopHeight)
        {
            instr->preview();
        }
    }
}

void MixChannel::handleMouseUp(InputEvent& ev)
{
    if(instr != NULL)
    {
        MInstrPanel->adjustOffset();
    }

    MAudio->releaseAllPreviews();

    //if(ev.mouseY < y1 + MixerTopHeight)
    {
        if(instr != NULL)
        {
            //_MInstrPanel->setCurrentInstrument(instr);
        }
    }
}

void MixChannel::setBufferSize(unsigned int bufferSize)
{
    for(Eff* eff : effs)
    {
        eff->setBufferSize(bufferSize);
    }
}

void MixChannel::setSampleRate(float sampleRate)
{
    for(Eff* eff : effs)
    {
        eff->setSampleRate(sampleRate);
    }
}


void MixChannel::placeEffectBefore(Eff* eff, Eff* before)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    effs.remove(eff);

    auto it = effs.begin();

    if(before == NULL)      // Insert to end
    {
        it = effs.end();
    }
    else
    {
        for(; it != effs.end() && *it != before; it++);
    }

    effs.insert(it, eff);

    remapAndRedraw();

    if(instr)
    {
        // MInstrPanel->setCurrInstr(instr);
    }

    ReleaseMutex(MixerMutex);
}

bool MixChannel::handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my)
{
    Gobj* o1 = NULL;
    Gobj* o2 = NULL;

    Gobj* o = CheckNeighborObjectsX(objs, "eff", mx, (Gobj**)&o1, (Gobj**)&o2);

    redraw();

    dropObj = o;

    int xh = x1 + 1;
    int yh1 = y1;
    int yh2 = y2;

/*
    if(o2 != NULL)
    {
        xh = o2->getX1() + 1;
        yh1 = o2->getY1();
        yh2 = o2->getY2();
    }
*/

    if(o1 != NULL)
    {
        xh = o1->getX2() + 1;
        yh1 = o1->getY1();
        yh2 = o1->getY2();
    }

    drag.dropHighlightVertical->setCoordsUn(xh - 4, yh1, xh + 4, yh2);

    int tw = gGetTextWidth(FontSmall, obj->getObjName());
    int th = gGetTextHeight(FontSmall);

    drag.setCoords1(mx - tw/2, my - th/2, tw, th);

    return true;
}

bool MixChannel::handleObjDrop(Gobj * obj,int mx,int my,unsigned flags)
{
    Eff* eff = NULL;

    BrwEntry* be = dynamic_cast<BrwEntry*>(obj);

    if(be)
    {
        eff = addEffectFromBrowser((BrwEntry*)obj);
        
        if(eff != NULL)
        {
            placeEffectBefore(eff, (Eff*)dropObj);
        }
        else
        {
            MWindow->showAlertBox("Can't load effect");
        }

        return true;
    }
    else
    {
        eff = dynamic_cast<Eff*>(obj);

        if(eff)
        {
            WaitForSingleObject(MixerMutex, INFINITE);
            //std::unique_lock<std::mutex> lock(MixMutex);

            if(flags & kbd_ctrl)
            {
                eff = eff->clone();
            }
            else if(eff != dropObj)
            {
                eff->mixChannel->removeEffect(eff);
            }

            if(eff != dropObj)
            {
                addEffect(eff);

                placeEffectBefore(eff, (Eff*)dropObj);
            }

            remapAndRedraw();

            ReleaseMutex(MixerMutex);
        }

        return true;
    }

    return false;
}

void MixChannel::reset()
{
    for(Eff* eff : effs)
    {
        eff->reset();
    }
}



Mixer::Mixer()
{
    init();
}

Mixer::~Mixer()
{
    int a = 1;
}

void Mixer::init()
{
    objId = "mixer";

    xOffset = 0;
    currentEffect = NULL;

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        addObject(sendChannel[sc] = new MixChannel(), "mchan.send");

        sendChannel[sc]->chanTitle = "Send #";
        sendChannel[sc]->chanTitle += char(short(sc + 48));
    }

    addObject(masterChannel = new MixChannel(), "mchan.master");

    masterChannel->master = true;
    masterChannel->chanTitle = "MASTER";

    addObject(scroller = new Scroller(false));
}


void Mixer::cleanBuffers(int num_frames)
{
    for(Instrument* instr : MInstrPanel->instrs)
    {
        memset(instr->mixChannel->inbuff, 0, sizeof(float)*num_frames*2);
    }

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        memset(sendChannel[sc]->inbuff, 0, sizeof(float)*num_frames*2);
    }

    memset(masterChannel->inbuff, 0, sizeof(float)*num_frames*2);
}

void Mixer::mixAll(int num_frames)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    for(Instrument* instr : MInstrPanel->instrs)
    {
        if (instr->mixChannel != masterChannel)
        {
            instr->mixChannel->process(num_frames, masterChannel->inbuff);

            for (int s = 0; s < NUM_SENDS; s++)
            {
                float val = instr->mixChannel->sends[s].amount->getOutVal();

                if (val > 0)
                {
                    instr->mixChannel->doSend(sendChannel[s]->inbuff, val, num_frames);
                }
            }
        }
    }

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        sendChannel[sc]->process(num_frames, masterChannel->inbuff);
    }

    masterChannel->process(num_frames, NULL);

    ReleaseMutex(MixerMutex);
}

void Mixer::resetAll()
{
    for(Instrument* instr : MInstrPanel->instrs)
    {
        instr->mixChannel->reset();
    }

    for(int sc = 0; sc < NUM_SENDS; sc++)
        sendChannel[sc]->reset();

    masterChannel->reset();
}

void Mixer::setCurrentEffect(Eff * eff)
{
    WaitForSingleObject(MixerMutex, INFINITE);
    //std::unique_lock<std::mutex> lock(MixMutex);

    currentEffect = eff;

    redraw();

    ReleaseMutex(MixerMutex);
}

void Mixer::setOffset(int offs)
{
    xOffset = offs; 

    if(xOffset < 0)
    {
        xOffset = 0; 
    }

    remapAndRedraw();
}

void Mixer::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    Scroller* scr = dynamic_cast<Scroller*>(obj);

    if(scr != NULL)
    {
        setOffset((int)scr->getoffs());
    }
}

void Mixer::remap()
{
    int yCh = 0 - MInstrPanel->getOffset();

    for(Instrument* instr : MInstrPanel->instrs)
    {
        if(!instr->previewOnly)
        {
            if((yCh + InstrHeight > 0) && yCh < getH())
            {
                instr->mixChannel->setCoords1(0, yCh, width, instr->getH());
            }
            else if(instr->mixChannel->isshown())
            {
                instr->mixChannel->setVisible(false);
            }

            yCh += InstrHeight + 1;
        }
    }

/*
    int base = 30;
    int chanX = 0 ;
    int mchanHeight = getH() - base - 2;

    masterSectionWidth = (NUM_SENDS + 1)*(MixChanWidth + 1) + 50;

    confine(0, 2, width - masterSectionWidth, 2 + mchanHeight);

    // Instrument mixchannels, ordered according to the instruments

    for(Instrument* instr : MInstrPanel->instrs)
    {
        if(!instr->previewOnly)
        {
            instr->mixChannel->setCoords1(chanX - xOffset, 2, MixChanWidth, mchanHeight);

            chanX += MixChanWidth + 1;
        }
    }

    confine();     // reset confinement

    scroller->setCoords1(0, getH() - base + 1, width - masterSectionWidth, 30);

    scroller->updateLimits((float)chanX, (float)(width - masterSectionWidth), (float)xOffset);

    chanX = width - masterSectionWidth + 10;

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        sendChannel[sc]->setCoords1(chanX, 2, MixChanWidth, getH() - 2);

        chanX += MixChanWidth + 1;
    }

    chanX += 10;

    masterChannel->setCoords1(chanX, 2, MixChanWidth, getH() - 2);

    chanX += MixChanWidth + 30;
    */
}

void Mixer::drawself(Graphics& g)
{
    fill(g, .1f);
/*
    gSetMonoColor(g, 0.35f);
    gLineHorizontal(g, y1, x1, x2);

    gSetMonoColor(g, 0.29f);
    gLineHorizontal(g, y1 + 1, x1, x2);

    gSetMonoColor(g, 0.25f);

    gFillRect(g, getX1(), getY1() + 2, getX2(), getY2());

    int xBound = getX1() + getW() - MixChanWidth - 35;

    g.drawVerticalLine(xBound + 1, (float)getY1(), (float)getY2());*/
}

int Mixer::getInstrChannelsRange()
{
    return width - masterSectionWidth;
}

void Mixer::updateChannelIndexes()
{
    int idx = 0;

    for(Instrument* instr : MInstrPanel->instrs)
    {
        instr->mixChannel->setIndex(idx++);
    }

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        sendChannel[sc]->setIndex(idx++);
    }

    masterChannel->setIndex(idx++);
}

MixChannel* Mixer::addMixChannel(Instrument * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel = new MixChannel(instr);

    mixChannel->mchanout = (MMixer->masterChannel);

    addObject(mixChannel, "");

    addObject(instr->butt = new Button36(true), "");

    ReleaseMutex(MixerMutex);

    return mixChannel;
}

