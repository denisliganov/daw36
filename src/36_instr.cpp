

#include "36.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_vu.h"
#include "36_devwin.h"
#include "36_edit.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_browser.h"
#include "36_mixchannel.h"
#include "36_audio_dev.h"
#include "36_menu.h"
#include "36_draw.h"
#include "36_button.h"
#include "36_project.h"
#include "36_text.h"
#include "36_events_triggers.h"
#include "36_knob.h"
#include "36_keyboard.h"
#include "36_params.h"
#include "36_dragndrop.h"



extern Device36* devDummy;





class InstrVU : public ChanVU
{
public:

    InstrVU::InstrVU() : ChanVU(true)
    {
        
    }

protected:

    void    drawSelf(Graphics & g)
    {
        float val = (getL() + getR())/2.f;
        val = pow(val, 0.3f);
        val *= (1 - 0.45f);

        Instr* i = dynamic_cast<Instr*>(parent);

        int left = (int)(height*getL());
        int right = (int)(height*getR());

        fill(g, .2f);

        //setc(g, 0xffFFB040);
        setc(g, .4f);

        int yc = 0;

        while (yc < height)
        {
            fillx(g, 1, yc, width - 2, 1);
            yc += 2;
        }

        yc = 0;
        setc(g, .8f);

        while(yc < height)
        {
            if(yc < left)
            {
                fillx(g, 1, height - yc - 1, width - 2, 1);
            }
            if(yc < right)
            {
                fillx(g, 1, height - yc - 1, width - 2, 1);
            }
            yc += 2;
        }
    }

    void handleMouseDown(InputEvent& ev)
    {
        if (parent)
        {
            parent->handleChildEvent(this, ev);
        }
    }

    void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
};


class EnableButton : public SelectorBox
{
public:

        EnableButton(Parameter* ptg) : SelectorBox(ptg) {}

protected:

        void drawSelf(Graphics& g)
        {
            Instr* instr = (Instr*)parent;

            instr->setMyColor(g, .4f);
            fillx(g, 0, 0, width, height);

            if(param->getBoolValue())
            {
                instr->setMyColor(g, 1.f);
            }
            else
            {
                instr->setMyColor(g, .6f);
            }

            //txt(g, FontVis, "#", width/2 - 2, height/2 + gGetTextHeight(FontVis)/2 - 1);
            txt(g, FontVis, instr->getAlias(), width / 2 - 2, height / 2 + gGetTextHeight(FontVis) / 2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
};


class GuiButt : public Button36
{
public:

        GuiButt() : Button36(false) 
        {
            fontId = FontInst;
        }

protected:

        FontId      fontId;

        void drawSelf(Graphics& g)
        {
            Instr* instr = (Instr*)parent;

            if (instr->getDevice() != devDummy)
            {
                instr->setMyColor(g, .5f);
                fillx(g, 0, 0, width, height);
                instr->setMyColor(g, .65f);
                rectx(g, 0, 0, width, height);
            }
            else
            {
                instr->setMyColor(g, .3f);
                fillx(g, 0, 0, width, height);
                //instr->setMyColor(g, .45f);
                //rectx(g, 0, 0, width, height);
            }

            bool wVis = (instr->getDevice() && instr->getDevice()->isWindowVisible());

            if(wVis)
            {
                gGradRect(g, 0xffFF9930, x1, y1, x2, y2);
            }

            int tw = gGetTextWidth(fontId, instr->getAlias());
            int th = gGetTextHeight(fontId);

            if (instr->getDevice() != devDummy)
            {
                instr->setMyColor(g, 1.f);
            }
            else
            {
                instr->setMyColor(g, .6f);
            }

            txt(g, fontId, instr->getAlias(), width/2 - tw/2 + 1, height/2 + th/2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
};


Instr::Instr(Device36* dev)
{
    index = -1;
    master = false;

    device = devDummy;

    addObject(guiButton = new GuiButt());

    mixChannel = MInstrPanel->getMasterChannel();       // Default to master channel

    //panBox = volBox = NULL;

    addObject(volKnob = new Knob(device->vol, false));
    volKnob->setHasText(false);
    volKnob->setHint("Channel Pre-Volume");

    addObject(panKnob = new Knob(device->pan, true));
    panKnob->setHint("Channel Pre-Panning");
    panKnob->setEnable(false);

    addObject(muteButt = new EnableButton(device->enabled));

    addObject(ivu = new InstrVU(), ObjGroup_VU);

    setDevice(dev);
}

Instr::~Instr()
{
    WaitForSingleObject(MixerMutex, INFINITE);

    setEnable(false);

    if(this == SoloInstr)
    {
        SoloInstr = NULL;
    }

    if (device != devDummy)
    {
        //delete device;
    }

    MInstrPanel->removeMixChannel(this);

    ReleaseMutex(MixerMutex);
}

void Instr::setDevice(Device36* dev)
{
    if (device != NULL)
    {
        volKnob->removeParam(device->vol);
        panKnob->removeParam(device->pan);
        muteButt->removeParam(device->enabled);

        //if (device != devDummy)
        {
        //    device->setContainer(NULL);
        }

        setObjName("");
    }

    device = dev;

    if (device)
    {
        addObject(device);

        device->setEnable(false);

        //device->setContainer(this);

        //if (device != devDummy)
        {
            device->setVU(ivu);

            volKnob->setParam(device->vol);
            panKnob->setParam(device->pan);
            muteButt->setParam(device->enabled);

            setObjName(device->getObjName());
        }
    }

    redraw();
}

void Instr::activateMenuItem(std::string item)
{
    if(item == "Clone")
    {
        MInstrPanel->cloneInstrument(this);
    }
    else if(item == "Delete")
    {
        MInstrPanel->deleteInstrument(this);
    }
}

void Instr::addMixChannel()
{
    mixChannel = MInstrPanel->addMixChannel(this);
}

ContextMenu* Instr::createContextMenu()
{
    if (isMaster())
    {
        return NULL;
    }
    else
    {
        MInstrPanel->setCurrInstr(this);

        //Menu* menu = new Menu(Obj_MenuPopup);

        ContextMenu* menu = new ContextMenu(this);

        menu->addMenuItem("Clone");
        menu->addMenuItem("Delete");
        menu->addMenuItem("Edit Self-Pattern");

        return menu;
    }
}

Instr* Instr::clone()
{
    Instr* instr = NULL;
/*
    switch(type)
    {
        case Instr_Sample:
            // TODO: rework sample cloning to copy from memory
            instr = (Instrument*)MInstrPanel->addSample(filePath.data());
            break;

        case Instr_VstPlugin:
            instr = (Instrument*)MInstrPanel->addVst(NULL, (VstInstr*)this);
            break;
    }*/

    return instr;
}

void Instr::drawSelf(Graphics& g)
{
    int h = height - 1;
    float incr = 0.f;

    if (this == MInstrPanel->getCurrInstr())
        incr = 0.2f;

    if (isMaster())
    {
        fill(g, .2f + incr);
        rect(g, .3f + incr);

        setc(g, .7f + incr);
        txt(g, FontInst, "Master", 18, height/2 + 3);
    }
    else
    {
        if(device != devDummy)
        {
            Gobj::setMyColor(g, .54f + incr);
            //setc(g, 1.f);
            fillx(g, 0, 0, width, h);

            Gobj::setMyColor(g, .5f + incr);
            fillx(g, 0, 0, width, h/2);
        }
        else
        {
            Gobj::setMyColor(g, .4f + incr);

            fillx(g, 0, 0, width, h);
        }

        //setc(g, .0f);
        //Gobj::setMyColor(g, 1.f, 1.f);
        //txt(g, FontVis, instrAlias, 2, height - 3);

        //setc(g, 1.f);
        Gobj::setMyColor(g, 1.f, .4f);
        txtfit(g, FontSmall, getObjName(), guiButton->getW() + 4, 10, width - 4);


        //Colour clr = Colour(100, 110, 110);
        //Colour clr = Colour(Gobj::colorHue, Gobj::colorSat, 1, 1);

        //float s = .4f;
        //float b = .8f;
        //float a = 1;
        //Colour clr = Colour(colorHue, s, b, a);
        //drawGlassRect1(g, (float)x1, (float)y1, width, height, clr, 1, 8, false, false, false, false);

        if(0 && device != devDummy)
        {
            float s = .4f;
            float b = .7f;
            float a = 1;

            Colour clr = Colour(colorHue, s, b, a);
            drawGlassRound(g, x2 - 70, y1 + 4, (h*0.65f), clr, 1);
            drawGlassRound(g, x2 - 47, y1 + 2, (h*0.8f), clr, 1);
        }
    }
}

void Instr::drawOverChildren(Graphics & g)
{
    //setMonoColor(.9f);
    //gTextFit(g, FontSmall, instrAlias, x1 + 6, y2 - 3, width - (width/2));
}

void Instr::deleteDevice()
{
    removeObject(device);
 
    if (device != NULL && device != devDummy)
    {
        delete device;
        device = NULL;
    }
}

std::list <Element*> Instr::getNotesFromRange(float offset, float lastVisibleTick)
{
    std::list <Element*> noteList;

    for(auto note : device->notes)
    {
        if(note->getEndTick() < offset || note == device->selfNote)
        {
            continue;
        }
        else if (note->getTick() > lastVisibleTick)
        {
            break;
        }
        else if (!note->isdel())
        {
            noteList.push_back((Element*)note);
        }
    }

    return noteList;
}

void Instr::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if(obj == ivu)
    {
        if (ev.clickDown)
        {
            MInstrPanel->setCurrInstr(this);

            preview(); 
        }
    }
    else if (obj == guiButton)
    {
        if (ev.clickDown && device != devDummy)
        {
            MInstrPanel->setCurrInstr(this);

            if (device)
            {
                device->showWindow(!device->isWindowVisible());
            }
        }
    }

    redraw();
}

void Instr::handleMouseDown(InputEvent& ev)
{
    if(ev.leftClick)
    {
        MInstrPanel->setCurrInstr(this);

        if(ev.keyFlags & kbd_ctrl)
        {
            preview();
        }
        else if (device != devDummy)
        {
            if(ev.keyFlags == 0 && ev.doubleClick)
            {
                device->showWindow(true);
            }
        }
    }
}

void Instr::handleMouseUp(InputEvent& ev)
{
    MAudio->releaseAllPreviews();

    MInstrPanel->setCurrInstr(this);
}

void Instr::handleMouseDrag(InputEvent& ev)
{
    if(device != devDummy && MDragDrop->canDrag())
    {
        MDragDrop->start(this, ev.mouseX, ev.mouseY);

        guiButton->release();
    }
}

void Instr::handleMouseWheel(InputEvent& ev)
{
    parent->handleMouseWheel(ev);
}

void Instr::load(XmlElement * instrNode)
{
    /*
    devIdx = instrNode->getIntAttribute(T("InstrIndex"), -1);

    XmlElement* xmlParam = NULL;

    forEachXmlChildElementWithTagName(*instrNode, xmlParam, T("Parameter"))
    {
        int idx = xmlParam->getIntAttribute(T("index"), -1);

        if(idx == vol->getIndex())
        {
            vol->load(xmlParam);
        }
        else if(idx == pan->getIndex())
        {
            pan->load(xmlParam);
        }
    }

    muteparam = (instrNode->getIntAttribute(T("mute")) == 1);

    soloparam = (instrNode->getIntAttribute(T("Solo")) == 1);

    if(soloparam)
    {
        SoloInstr = this;
    }
    */
}

void Instr::preview(int note)
{
    if (device && device->selfNote)
    {
        device->selfNote->setNote(note);

        device->selfNote->preview(note);
    }
}

void Instr::remap()
{
    int h = height - 1;

    if (!isMaster())
    {
        //guiButton->setCoords1(0, 0, h/1.5f, h);

        int bw = 16;

        if (device != devDummy)
        {
            //guiButton->setTouchable(true);

            /*
            int slH = 4;

            if (panBox)
            {
                panBox->setCoords1(width - 190, h - slH, 70, slH);
                panBox->setVis(false);
            }

            if (volBox)
            {
                volBox->setCoords1(width - 110, h - slH, 80, slH);
                volBox->setVis(false);
            }
            */

            int kH = h/1.5f;
            if (kH % 2)
            {
                kH--;
            }

            //volKnob->setCoords1(width - 80 - bw - bw - 2, (h - kH)/2, 80, int(kH));
            volKnob->setCoords1(width - 80 - bw - bw - 1, h - h/2+1, 80, h/2);

            //panKnob->setCoords1(width - 57, 0, int(kH-2), int(kH-2));

            muteButt->setCoords1(width - bw - bw, 0, bw, h);
        }
        else
        {
            //guiButton->setTouchable(false);

            //volBox->setVis(false);
            //panBox->setVis(false);

            volKnob->setVis(false);
            //panKnob->setVis(false);

            //soloButt->setVis(false);

            muteButt->setVis(false);
        }

        ivu->setCoords1(width - bw, 0, bw, h);
        
        if(gGetTextWidth(FontSmall, objName) > width - 38 - 50 - 10)
        {
            setHint(objName);
        }
        else
        {
            setHint("");
        }
    }
}

void Instr::setAlias(std::string alias)
{
    instrAlias = alias;
}

void Instr::setIndex(int idx)
{
    index = idx;
    device->setIndex(idx);

    int num = idx + 1;

    if(num == 10)
        num = 0;

    char c;

    if(num < 10)
    {
        c = num + 0x30; // ASCII offset for numbers
    }
    else
    {
        c = num + 0x36; // ASCII offset for uppercase letters minus 0xB
        //c = num + 0x56; // ASCII offset for lowercase letters minus 0xB
    }

    instrAlias = c;
}

void Instr::setBufferSize(unsigned bufferSize)
{
    device->setBufferSize(bufferSize);

    mixChannel->setBufferSize(bufferSize);
}

void Instr::save(XmlElement * instrNode)
{
    /*
    instrNode->setAttribute(T("InstrIndex"), devIdx);
    instrNode->setAttribute(T("InstrType"), int(type));
    instrNode->setAttribute(T("InstrName"), String(objName.data()));
    instrNode->setAttribute(T("InstrPath"), String(filePath.data()));

    instrNode->addChildElement(vol->save());
    instrNode->addChildElement(pan->save());
    */
    /*
    instrNode->setAttribute(T("Mute"), int(muteparam->getOutVal()));
    instrNode->setAttribute(T("Solo"), int(soloparam->getOutVal()));
    */
}

void Instr::setSampleRate(float sampleRate)
{
    mixChannel->setSampleRate(sampleRate);
}



