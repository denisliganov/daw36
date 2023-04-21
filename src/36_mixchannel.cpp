


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
#include "36_parambox.h"
#include "36_draw.h"
#include "36_keyboard.h"
#include "36_audio_dev.h"
#include "36_dragndrop.h"
#include "36_text.h"
#include "36_knob.h"





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



class MixDropHighlight : public Gobj
{
public:

    MixDropHighlight() 
    {
        setTouchable(false);
    }

    void    drawSelf(Graphics& g)
    {
        uint8 a = 128;

        Rect36 drwRect = { (float)(x1), (float)y1, (float)width, (float)height };

        bool vert = drwRect.w < drwRect.h;

        int count = vert ? width / 2 : height / 2;

        for (int c = 0; c < count; c++)
        {
            float m = (float)c / count;

            m *= m;

            gSetColor(g, 255, 153, 48, uint8(m * 255));

            if (vert)
            {
                gFillRectWH(g, drwRect.x, drwRect.y, 1, drwRect.h);
                gFillRectWH(g, drwRect.x + drwRect.w - 1, drwRect.y, 1, drwRect.h);

                drwRect.x++;
                drwRect.w -= 2;
            }
            else
            {
                gFillRectWH(g, drwRect.x, drwRect.y, drwRect.w, 1);
                gFillRectWH(g, drwRect.x, drwRect.y + drwRect.h - 1, drwRect.w, 1);

                drwRect.y++;
                drwRect.h -= 2;
            }

            if (drwRect.h < 1 || drwRect.w < 1)
            {
                break;
            }

            a /= 2;
        }
    }
};


class ChanOutToggle : public ToggleBox
{
public:

    ChanOutToggle(MixChannel* chan, MixChannel* out_chan) : ToggleBox(new Parameter("out", Param_Toggle))
    {
        channel = chan;
        outChannel = out_chan;
        param->setModule(channel);
    }

    MixChannel* getOutChannel()
    {
        return outChannel;
    }

    void setValue(bool val)
    {
        param->setBoolValue(val);

        redraw();
    }

private:

    void drawSelf(Graphics& g)
    {
        if (true)
        {
            Instr* instr = outChannel->getInstr();

            float hue = instr->isMaster() ? 1.f : instr->getColorHue();

            float s = 0; //.4f;
            float b = 1.0f;
            float a = 1;

            Colour clr = Colour(hue, s, b, a);

            if (param->getBoolValue())
                drawGlassRound(g, x1+1, y1+1, width-2, clr, 1);
            else
                drawGlassRound(g, x1+1, y1+1, width-2, clr.withBrightness(.32f), 1);
        }
        else
        {
            //Colour clr = Colour(0.f, 0.f, 0.f, 1.f);

            drawGlassRound(g, x1+1, y1+1, width-2, Colours::black, 1);
        }
    }

    /*
    std::string ChanOutToggle::getHint()
    {
        std::string hint = param->getName().data();

        hint += ": ";
        hint += param->getValString();

        return hint;
    }*/

    void handleMouseDown(InputEvent & ev) 
    {
        if (active)
        {
            param->setBoolValue(!param->getBoolValue());
            redraw();
        }
    }
    void handleMouseWheel(InputEvent& ev) { parent->handleMouseWheel(ev); }

    MixChannel*     channel;
    MixChannel*     outChannel;
};


class SendKnob : public Knob
{
public:

    SendKnob(MixChannel* chan, MixChannel* out_chan, std::string nm) : Knob(new Parameter(nm, Param_Default), true)
    {
        channel = chan;
        outChannel = out_chan;

        param->setModule(channel);

        setDimOnZero(true);
    }

    MixChannel* getOutChannel()
    {
        return outChannel;
    }

    void drawKnob(Graphics & g, float b1, float b2)
    {
        if (1)
        {
            Knob::drawKnob(g);
        }
        else
        {
            int w = height - 4;// *0.8f;
            int h = height - 4;// *0.8f;
            int x = x1 + 2;
            int y = y1 + 2;

            drawGlassRound(g, x, y, w, Colours::black, 1);
        }
    }

    float getValue()
    {
        return param->getOutVal();
    }

private:
/*
    std::string getHint()
    {
        std::string hint = param->getName().data();

        hint += ": ";
        hint += param->getValString();

        return hint;
    }*/

    MixChannel*     channel;
    MixChannel*     outChannel;
};





MixChannel::MixChannel()
{
    init(NULL);
}

MixChannel::MixChannel(Instr* i)
{
    init(i);
}

MixChannel::~MixChannel()
{
    //
}

void MixChannel::addSend(MixChannel* mchan)
{
    SendKnob* sk;
    addObject(sk = new SendKnob(this, mchan, "snd"), "snd");

    sk->setHint("Send to " + mchan->getInstr()->getAlias());

    ChanOutToggle* c;
    addObject(c = new ChanOutToggle(this, mchan), "out");

    c->setHint("Route to " + mchan->getInstr()->getAlias());

    if (mchan == mchanout)
    {
        outTg = c;
        outTg->setValue(true);
    }

    updateSends();
}

void MixChannel::delSend(MixChannel* mchan)
{
    SendKnob* k = NULL;
    ChanOutToggle* c = NULL;

    for (Gobj* o : objs)
    {
        if (!k && o->getObjId() == "snd")
        {
            k = dynamic_cast<SendKnob*>(o);

            if (k && k->getOutChannel() != mchan)
                k = NULL;
        }

        if (!c && o->getObjId() == "out")
        {
            c = dynamic_cast<ChanOutToggle*>(o);

            if (c && c->getOutChannel() != mchan)
                c = NULL;
        }
    }

    if (k)
        deleteObject(k);

    if (c)
        deleteObject(c);
}

void MixChannel::init(Instr* ins)
{
    objId = "mixchan";

    mixCount = 0;

    instr = ins;

    mchanout = NULL;
    mutetoggle = NULL;
    volKnob = NULL;
    panKnob = NULL;
    vu = NULL;
    outTg = NULL;

    //addParam(volParam = new Parameter("Volume", Param_Vol, 0.f, DAW_VOL_RANGE, 1.f, Units_Percent));
    //addParam(panParam = new Parameter("Panning", Param_Pan));

    addBasicParamSet();

    addObject(volKnob = new Knob(vol));
    addObject(panKnob = new Knob(pan));

    addObject(vu = new ChanVU(false), ObjGroup_VU);
    addObject(vscr = new Scroller(true));

    addHighlight(dropHighlight = new MixDropHighlight());
}

void MixChannel::remap()
{
    if (MixViewSingle)
    {
        int sendPanelHeight = 0;

        int xeff = FxPanelScrollerWidth + 1;
        int yeff = 0;
        int totalHeight = 0;
        int visibleHeight = height - FxPanelBottomHeight - 2;
        int gap = 5;

        confine(xeff, 1, width - 80, visibleHeight);

        for (Eff* eff : effs)
        {
            eff->showDevice(true);

            totalHeight += eff->getH() + gap;
        }

        vscr->updBounds(totalHeight, visibleHeight, vscr->getOffset());

        for (Eff* eff : effs)
        {
            eff->setCoords1(xeff, 1 + yeff - int(vscr->getOffset()), eff->getW(), eff->getH());

            yeff += eff->getH() + gap;
        }

        confine();

        vscr->setCoords1(0, 0, FxPanelScrollerWidth, visibleHeight);

        int ySendControls = height - FxPanelBottomHeight;
        int yControls = height - FxPanelBottomHeight + sendPanelHeight;

        volKnob->setCoords1(FxPanelScrollerWidth, yControls + 5, 150, 22);
        panKnob->setCoords1(FxPanelScrollerWidth, yControls + 32, 150, 22);

        vu->setCoords1(FxPanelScrollerWidth, height - 22, 150, 20);

        for (Gobj* o : objs)
        {
            if (o->getObjId() == "snd")
            {
                SendKnob* k = dynamic_cast<SendKnob*>(o);

                Instr* ins = k->getOutChannel()->getInstr();

                k->setCoords1(width - 40, ins->getY1() - parent->getY1() + 2, 20, 20);
            }

            if (o->getObjId() == "out")
            {
                ChanOutToggle* t = dynamic_cast<ChanOutToggle*>(o);

                Instr* ins = t->getOutChannel()->getInstr();

                t->setCoords1(width - 17, ins->getY1() - parent->getY1() + 5, 12, 12);
            }
        }
    }
    else
    {
        confine(0, 0, width - 10, height);

        int xeff = 0;

        for(Eff* eff : effs)
        {
            eff->showDevice(false);

            eff->setCoords1(xeff, 1, 32, height - 2);

            xeff += eff->getW() + 1;
        }

        vscr->setVis(false);
        volKnob->setVis(false);
        panKnob->setVis(false);
        vu->setVis(false);

        for (Gobj* o : objs)
        {
            o->setVis(false);
        }
    }
}

void MixChannel::drawSelf(Graphics& g)
{
    //gSetColorSettings(instr->getColorHue(), .2f);

    fill(g, .1f);

    int w = width - 80; //width - 64;

    if (MixViewSingle)
    {
        int sendPanelHeight = 0;

        setc(g, .4f);
        fillx(g, 0, height - FxPanelBottomHeight, w, FxPanelBottomHeight - InstrHeight);

        setc(g, .36f);
        fillx(g, 0, height - FxPanelBottomHeight, w, sendPanelHeight);

        setc(g, .46f);
        rectx(g, 0, height - FxPanelBottomHeight, w, FxPanelBottomHeight - InstrHeight);
    }
    else
    {
        setc(g, .2f);
        fillx(g, 0, 0, w, height);
    }
}


void MixChannel::drawOverChildren(Graphics& g)
{
    /*
    for (Gobj* o : objs)
    {
        if (o->getObjId() == "out")
        {
            ChanOutToggle* t = dynamic_cast<ChanOutToggle*>(o);

            if (t->getBoolValue())
            {
                setc(g, .6f);
                gLine(g, x1, y1 + height/2, t->getX1() + t->getW()/2, t->getY1() + t->getH()/2);
            }
        }
    }
    */

/*
    setc(g, .6f);

    for (SendKnob* sk : sendsk)
    {
        gLine(g, x1, y1 + height/2, sk->getX1() + sk->getW()/2, sk->getY1() + sk->getH()/2);
    }

    if (out != NULL)
        gLine(g, x1, y1 + height/2, out->getX1() + out->getW()/2, out->getY1() + out->getH()/2);
*/
    //gResetColorSettings();
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
        Vst2Plugin*  vsteff = new Vst2Plugin(de->getPath());

        eff = new Eff(vsteff);
    }
    else if(de->getType() == Entry_Native)
    {
        eff = CreateEffect(de->getPath());
    }

    if(eff != NULL)
    {
        addEffect(eff);
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

void MixChannel::save(XmlElement * xmlChanNode)
{
    //xmlChanNode->setAttribute(T("Index"), instr->instrAlias);

    //xmlChanNode->addChildElement(volParam->save());
    //xmlChanNode->addChildElement(panParam->save());

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
    //soloparam = xmlNode->getBoolAttribute(T("Solo"));
    //if (soloparam)
    //    SoloMixChannel = this;

    XmlElement* xmlParam = NULL;

    forEachXmlChildElementWithTagName(*xmlNode, xmlParam, T("Parameter"))
    {
        String sname = xmlParam->getStringAttribute(T("devName"));

        //if(sname == T("Vol"))
        //    volParam->load(xmlParam);
        //else if(sname == T("Pan"))
        //    panParam->load(xmlParam);

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

bool MixChannel::canAcceptInputFrom(MixChannel * other_chan)
{
    if (instr->isMaster())
    {
        return true;
    }

    if (other_chan == this)
    {
        return false;
    }

    if (outTg)
    {
        if (!outTg->getOutChannel()->canAcceptInputFrom(other_chan))
        {
            return false;
        }
    }

    for (SendKnob* sk : sendsActive)
    {
        if (!sk->getOutChannel()->canAcceptInputFrom(other_chan))
        {
            return false;
        }
    }

    return true;
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

        ReleaseMutex(MixerMutex);
    }

    redraw();
}

void MixChannel::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    if (obj == vscr)
    {
        remap();
        redraw();
    }
}

void MixChannel::handleMouseWheel(InputEvent& ev)
{
    vscr->setOffset(vscr->getOffset() - ev.wheelDelta * 15);
}

void MixChannel::handleMouseDown(InputEvent& ev)
{
    if(instr != NULL)
    {
        MInstrPanel->setCurrInstr(instr);
    }
}

void MixChannel::handleMouseUp(InputEvent& ev)
{
    MAudio->releaseAllPreviews();

    if(!MixViewSingle)
    {
        if(instr != NULL)
        {
            MInstrPanel->setCurrInstr(instr);
        }
    }
}

void MixChannel::handleObjDrag(bool reset, Gobj * obj,int mx,int my)
{
    if (reset)
    {
        dropHighlight->setVis(false);
        dropObj = NULL;
        return;
    }

    if (MixViewSingle)
    {
        Gobj* uper = NULL;
        Gobj* lower = NULL;

        dropObj = CheckNeighborObjectsY(objs, "eff", my, (Gobj**)&uper, (Gobj**)&lower);

        int hW = FxPanelMaxWidth - 80 - FxPanelScrollerWidth - 1;

        if(uper != NULL)
        {
            dropHighlight->setCoords1(uper->getX(), uper->getY() + uper->getH() - 3, hW, 8);
;       }
        else
        {
            dropHighlight->setCoords1(getX() + 1, getY() - 3, hW, 8);
        }
    }
    else
    {
        Gobj* left = NULL;
        Gobj* right = NULL;

        dropObj = CheckNeighborObjectsX(objs, "eff", mx, (Gobj**)&left, (Gobj**)&right);

        if(left != NULL)
        {
            dropHighlight->setCoords1(left->getX() + left->getW() - 3, left->getY(), 8, left->getH());
        }
        else
        {
            dropHighlight->setCoords1(getX() - 3, getY(), 8, height);
        }

        redraw();
    }
}

bool MixChannel::handleObjDrop(Gobj * obj,int mx,int my,unsigned flags)
{
    dropHighlight->setVis(false);

    BrwListEntry* ble = dynamic_cast<BrwListEntry*>(obj);
    Eff* eff = dynamic_cast<Eff*>(obj);
    
    if(ble)
    {
        if (ble->getType() == Entry_Native || ble->getType() == Entry_DLL)
        {
            eff = addEffectFromBrowser((BrwListEntry*)obj);

            if (eff != NULL)
            {
                placeEffectBefore(eff, (Eff*)dropObj);
            }
            else
            {
                MWindow->showAlertBox("Can't load effect");
            }
        }
    }
    else if (eff)
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

void MixChannel::handleParamUpdate(Parameter * param)
{
    if (param->getName() == "snd")
    {
        WaitForSingleObject(MixerMutex, INFINITE);

        SendKnob* sk = dynamic_cast<SendKnob*>(param->getControl());

        sk->delSnap();

        if (sk)
        {
            if (sk->getParam()->getNormalizedValue() == 0)
            {
                for (SendKnob* s : sendsActive)
                {
                    if (s == sk)
                    {
                        redraw();
                        break;
                    }
                }

                sendsActive.remove(sk);
            }
            else
            {
                sendsActive.push_back(sk);
                sendsActive.unique();

                // Disable conflicting routing

                if (outTg && outTg->getOutChannel() == sk->getOutChannel())
                {
                    outTg->setValue(false);
                    outTg = NULL;
                }

                redraw();
            }

            sk->getOutChannel()->updateSends();
        }

        ReleaseMutex(MixerMutex);
    }
    else if (param->getName() == "out")
    {
        WaitForSingleObject(MixerMutex, INFINITE);

        ChanOutToggle* t = dynamic_cast<ChanOutToggle*>(param->getControl());

        if (t->getBoolValue() == false)
        {
            if (t == outTg)
            {
                // Disable output completely

                outTg = NULL;
            }
        }
        else
        {
            mchanout = t->getOutChannel();

            if (outTg && outTg != t)
            {
                // Switch output to other channel

                outTg->setValue(false);
            }

            outTg = t;

            // Disable possible conflicting send

            for (SendKnob* s : sendsActive)
            {
                if (s->getOutChannel() == t->getOutChannel())
                {
                    s->getParam()->setNormalizedValue(0);
                    sendsActive.remove(s);
                    break;
                }
            }
        }

        t->getOutChannel()->updateSends();

        ReleaseMutex(MixerMutex);
    }
}

void MixChannel::updateSends()
{
    for (Gobj* o : objs)
    {
        if (o->getObjId() == "snd")
        {
            SendKnob* k = dynamic_cast<SendKnob*>(o);
            k->setActive(k->getOutChannel()->canAcceptInputFrom(this));
        }

        if (o->getObjId() == "out")
        {
            ChanOutToggle* t = dynamic_cast<ChanOutToggle*>(o);
            t->setActive(t->getOutChannel()->canAcceptInputFrom(this));
        }
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

    ReleaseMutex(MixerMutex);
}

void MixChannel::prepareForMixing()
{
    if (outTg)
    {
        outTg->getOutChannel()->increaseMixCounter();
    }

    for (SendKnob* sk : sendsActive)
    {
        sk->getOutChannel()->increaseMixCounter();
    }

    processed = false;
}

void MixChannel::doSend(float* sendbuff, float amount, int num_frames)
{
    float outL, outR;

    int i = 0;
    int fc = 0;

    while (i < num_frames)
    {
        outL = outBuff[fc];
        outR = outBuff[fc + 1];

        sendbuff[fc++] += outL * amount;
        sendbuff[fc++] += outR * amount;

        i++;
    }
}

void MixChannel::processChannel(int num_frames)
{
    process(num_frames);

    for (SendKnob* sk : sendsActive)
    {
        doSend(sk->getOutChannel()->tempBuff, sk->getValue(), num_frames);

        sk->getOutChannel()->decreaseMixCounter();
    }

    if (outTg)
    {

        doSend(outTg->getOutChannel()->tempBuff, 1, num_frames);

        outTg->getOutChannel()->decreaseMixCounter();
    }

    processed = true;
}

void MixChannel::process(int num_frames)
{
    if(effs.size() > 0)
    {
        for(Eff* eff : effs)
        {
            eff->getDevice()->generateData(tempBuff, outBuff, num_frames);

            memcpy(tempBuff, outBuff, sizeof(float)*num_frames*2);
        }
    }
    else
    {
        memcpy(outBuff, tempBuff, sizeof(float)*num_frames*2);
    }

    fillOutputBuffer(outBuff, num_frames, 0, 0);

#if 0
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
                /*
                if(volParam->envelopes != NULL)
                {
                    //venv = (Envelope*)((Command*)volParam->envelopes->el)->paramedit;
                }

                if(panParam->envelopes != NULL)
                {
                    ///penv = (Envelope*)((Command*)panParam->envelopes->el)->paramedit;
                }*/

                volv = volParam->getOutVal();

                if (volv < 1)
                {
                    int a = 1;
                }

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
                    mR = (volL - volR)*outbuff[tc + 1];
                }
                else
                {
                    mR = 0;
                }

                if(volR > volL)
                {
                    mL = (volR - volL)*outbuff[tc];
                }
                else
                {
                    mL = 0;
                }

                outL = outbuff[tc]*volL + mR;
                outR = outbuff[tc + 1]*volR + mL;

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
                    outbuff[tc++] = outL;
                    outbuff[tc++] = outR;
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
#endif
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

void MixChannel::setInstrument(Instr* i)
{
    instr = i;
}

void MixChannel::setOutChannel(MixChannel * mc)
{
    mchanout = mc;
}


