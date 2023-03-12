


#include "36_mixer.h"
#include "36_mixchannel.h"
#include "36_params.h"
#include "36_vu.h"
#include "36_browser.h"
#include "36_browserlist.h"
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
#include "36_knob.h"
#include "36_vstinstr.h"





Eff* CreateEffect(std::string effalias)
{
    Device36* dev = NULL;

    if(effalias == "eff.eq1")
    {
        dev = new EQ1();
    }
    else if(effalias == "eff.eq3")
    {
        dev = new EQ3();
    }
    else if(effalias == "eff.grapheq")
    {
        dev = new GraphicEQ();
    }
    else if(effalias == "eff.delay")
    {
        dev = new XDelay();
    }
    else if(effalias == "eff.reverb")
    {
        dev = new CReverb();
    }
    else if(effalias == "eff.tremolo")
    {
        dev = new CTremolo();
    }
    else if(effalias == "eff.comp")
    {
        dev = new Compressor();
    }
    else if(effalias == "eff.chorus")
    {
        dev = new CChorus();
    }
    else if(effalias == "eff.flanger")
    {
        dev = new CFlanger();
    }
    else if(effalias == "eff.phaser")
    {
        dev = new CPhaser();
    }
    else if(effalias == "eff.wah")
    {
        dev = new CWahWah();
    }
    else if(effalias == "eff.dist")
    {
        dev = new CDistort();
    }
    else if(effalias == "eff.bitcrush")
    {
        dev = new CBitCrusher();
    }
    else if(effalias == "eff.stereo")
    {
        dev = new CStereo();
    }
    else if(effalias == "eff.filter1")
    {
        dev = new Filter1();
    }

    return new Eff(dev);
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

    muteCount = 0;

    if(ins != NULL)
    {
        instr = ins;

        int slLen = MixChanWidth - 10;

        addParam(volParam = new Parameter("Volume", Param_Vol));
        addParam(panParam = new Parameter("Panning", Param_Pan));

        //addObject(volKnob = new Knob(volParam));
        //addObject(panKnob = new Knob(panParam));

        //muteparam = NULL; // instr->muteparam;
        //soloparam = NULL; // instr->soloparam;
    }
    else    // send or master
    {
        instr = NULL;
        mchanout = NULL;
        //muteparam = NULL;
        soloparam = NULL;
        mutetoggle = NULL;
        solotoggle = NULL;
        volKnob = NULL;
        volKnob = NULL;

        addParam(volParam = new Parameter("Volume", Param_Vol));
        addParam(panParam = new Parameter("Panning", Param_Pan));
    }

    addObject(vu = new ChanVU(true), ObjGroup_VU);
}

void MixChannel::remap()
{
    if (MixViewSingle)
    {
        //mixViewUpdate();

        confine(FxPanelScrollerWidth, 0, width-1, height - FxPanelBottomHeight);

        int xeff = FxPanelScrollerWidth;
        int yeff = 0;

        for (Eff* eff : effs)
        {
            eff->getDevice()->setEnable(true);
            eff->getDevice()->setVis(true);
            eff->setCoords1(xeff, yeff, eff->getDevice()->getW(), eff->getDevice()->getH() + 14);

            yeff += eff->getH() + 1;
        }

        //if(volslider)
        //    volslider->setCoords1(width - 30, 1, 10, height - 2);

        //if(volKnob)
        //    volKnob->setCoords1(width - 30, 0, 22, 22);
        //if(panKnob)
        //    panKnob->setCoords1(width - 30, 22, 22, 22);
        //if(panslider)
        //    panslider->setCoords1(width - 18, 1, 10, height - 2);

        //if(vu)
        //    vu->setCoords1(1, 1, 10, height - 2);
    }
    else
    {
        confine(0, 0, width - 1, height);

        int xeff = 0;

        for(Eff* eff : effs)
        {
            eff->getDevice()->setEnable(false);
            eff->getDevice()->setVis(false);

            eff->setCoords1(xeff, 1, 32, height - 2);

            xeff += eff->getW() + 1;
        }
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

void MixChannel::drawSelf(Graphics& g)
{
    int baseheight = MixChannelPadHeight;

    fill(g, .2f);

    //setc(g, 1.f);
    //gTextFit(g, FontSmall, instr->getObjName(), 0, 5, width);
    //gText(g, FontSmall, "123", 0, 5);

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

    addObject(eff, "eff");

    remapAndRedraw();
    
    ReleaseMutex(MixerMutex);
}

Eff* MixChannel::addEffectFromBrowser(BrwListEntry * de)
{
    Eff* eff = NULL;

    if(de->getType() == Entry_DLL)
    {
        //VstEffect* vsteff = new VstEffect((char*)de->getPath().data());
        VstInstr*  vsteff = new VstInstr((char*)de->getPath().data(), NULL);

        eff = new Eff(vsteff);
    }
    else if(de->getType() == Entry_Native)
    {
        eff = CreateEffect(de->getPath());
    }

    if(eff != NULL)
    {
        addEffect(eff);

        MMixer->setCurrentEffect(eff);
    }

    return eff;
}

void MixChannel::deleteEffect(Eff* eff)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    effs.remove(eff);

    deleteObject(eff);

    remapAndRedraw();

    MProject.setChange();

    ReleaseMutex(MixerMutex);
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

    //xmlChanNode->setAttribute(T("Mute"), muteparam);
    //xmlChanNode->setAttribute(T("Solo"), soloparam);

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
    //muteparam = xmlNode->getBoolAttribute(T("Mute"));
    soloparam = xmlNode->getBoolAttribute(T("Solo"));

    if (soloparam)
    {
        SoloMixChannel = this;
    }

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

ContextMenu* MixChannel::createContextMenu()
{
    ContextMenu* menu = new ContextMenu(this);

    menu->addMenuItem("1-band Equalizer");
    menu->addMenuItem("3-band Equalizer");
    menu->addMenuItem("Graphic Equalizer");
    menu->addMenuItem("BitCrusher");
    menu->addMenuItem("Compressor");
    menu->addMenuItem("Delay");
    menu->addMenuItem("Distortion");
    menu->addMenuItem("Flanger");
    menu->addMenuItem("Filter");
    menu->addMenuItem("Phaser");
    menu->addMenuItem("Reverb");
    menu->addMenuItem("Stereo Expander");
    menu->addMenuItem("WahWah");

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

void MixChannel::activateMenuItem(std::string mi)
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
    else if(mi == "BitCrusher")
    {
        eff = CreateEffect("eff.bitcrush");
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

void MixChannel::handleMouseWheel(InputEvent& ev)
{
    //
}

void MixChannel::handleMouseDown(InputEvent& ev)
{
    if(instr != NULL)
    {
        MInstrPanel->setCurrInstr(instr);

        //if(ev.keyFlags & kbd_ctrl && ev.mouseY < y1 + MixerTopHeight)
        {
        //    instr->preview();
        }
    }
}

void MixChannel::handleMouseUp(InputEvent& ev)
{
    MAudio->releaseAllPreviews();

    //if(ev.mouseY < y1 + MixerTopHeight)
    {
        if(instr != NULL)
        {
            //_MInstrPanel->setCurrentInstrument(instr);
        }
    }
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

   // BrwEntry* be = dynamic_cast<BrwEntry*>(obj);
    BrwListEntry* ble = dynamic_cast<BrwListEntry*>(obj);

/*
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
    else */
    if(ble)
    {
        eff = addEffectFromBrowser((BrwListEntry*)obj);
        
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
                eff->getMixChannel()->removeEffect(eff);
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

void MixChannel::process(int num_frames, float* out_buff)
{
    if(effs.size() > 0)
    {
        for(Eff* eff : effs)
        {
            eff->getDevice()->generateData(inbuff, outbuff, num_frames);

            if(eff->getDevice()->getBypass() == false)
            {
                // Copy output back to input for the next effect to process

                if(eff->getDevice()->muteCount > 0)
                {
                    long tc = 0;
                    float aa;

                    while(tc < num_frames)
                    {
                        aa = float(DECLICK_COUNT - eff->getDevice()->muteCount)/DECLICK_COUNT;

                        inbuff[tc*2] = inbuff[tc*2]*(1.f - aa) + outbuff[tc*2]*aa;
                        inbuff[tc*2 + 1] = inbuff[tc*2 + 1]*(1.f - aa) + outbuff[tc*2 + 1]*aa;

                        tc++;

                        if(eff->getDevice()->muteCount > 0)
                        {
                            eff->getDevice()->muteCount--;
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
                if(eff->getDevice()->muteCount < DECLICK_COUNT)
                {
                    long tc = 0;
                    float aa;

                    while(tc < num_frames && eff->getDevice()->muteCount < DECLICK_COUNT)
                    {
                        aa = float(DECLICK_COUNT - eff->getDevice()->muteCount)/DECLICK_COUNT;

                        inbuff[tc*2] = inbuff[tc*2]*(1.f - aa) + outbuff[tc*2]*aa;
                        inbuff[tc*2 + 1] = inbuff[tc*2 + 1]*(1.f - aa) + outbuff[tc*2 + 1]*aa;

                        tc++;

                        eff->getDevice()->muteCount++;
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

        /*
        if(muteparam != NULL)
        {
            if(!muteparam || (SoloMixChannel != NULL && SoloMixChannel != this))
            {
                off = true;
            }
        }*/

        bool fill;

        if(off == false || muteCount < DECLICK_COUNT)
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

                volv = volParam->getOutVal();

                if(volParam->lastValue == -1)
                {
                    volParam->setLastVal(volParam->getOutVal());
                }
                else if(volParam->lastValue != volParam->getOutVal())
                {
                    if(volParam->declickCount == 0)
                    {
                        volParam->declickCoeff = float(volParam->getOutVal() - volParam->lastValue)/DECLICK_COUNT;

                        volv = volParam->lastValue;

                        volParam->declickCount = DECLICK_COUNT;
                    }
                    else
                    {
                        volv = volParam->lastValue + (DECLICK_COUNT - volParam->declickCount)*volParam->declickCoeff;
                    }
                }
                else if(volParam->declickCount > 0) // (params->volparam->lastval == params->volparam->outval)
                {
                    volParam->declickCount = 0;
                    volParam->declickCoeff = 0;
                }

                panv = panParam->getOutVal();
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
                        volParam->setLastVal(volParam->getOutVal());
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

                    if(muteCount < DECLICK_COUNT)
                    {
                        aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                        muteCount++;
                    }
                    else
                    {
                        aa = 0;
                    }
                }
                else
                {
                    // Fadein case

                    if(muteCount > 0)
                    {
                        aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                        muteCount--;
                    }
                    else if(muteCount == 0)
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

void MixChannel::removeEffect(Eff* eff)
{
    effs.remove(eff);

    eff->setMixChannel(NULL);

    removeObject(eff);

    remapAndRedraw();
}

void MixChannel::setBufferSize(unsigned int bufferSize)
{
    for(Eff* eff : effs)
    {
        eff->getDevice()->setBufferSize(bufferSize);
    }
}

void MixChannel::setSampleRate(float sampleRate)
{
    for(Eff* eff : effs)
    {
        eff->getDevice()->setSampleRate(sampleRate);
    }
}

void MixChannel::reset()
{
    for(Eff* eff : effs)
    {
        eff->getDevice()->reset();
    }
}




