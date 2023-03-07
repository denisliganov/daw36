

#include "36.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_vu.h"
#include "36_devwin.h"
#include "36_edit.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_browser.h"
#include "36_mixer.h"
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
#include "36_parambox.h"
#include "36_params.h"


extern Device36* devDummy;


class EnableButton : public ToggleBox
{
protected:

        void drawSelf(Graphics& g)
        {
            Instrument* instr = (Instrument*)parent;

            instr->setMyColor(g, .4f);
            fillx(g, 0, 0, width, height);

            if(prmToggle->getValue())
            {
                instr->setMyColor(g, 1.f);
            }
            else
            {
                instr->setMyColor(g, .6f);
            }

            txt(g, FontVis, "#", width/2 - 2, height/2 + gGetTextHeight(FontVis)/2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }

public:

        EnableButton(ParamToggle* ptg) : ToggleBox(ptg) {}
};

class SoloButton : public Button36
{
protected:

        void drawSelf(Graphics& g)
        {
            Instrument* instr = (Instrument*)parent;

            instr->setMyColor(g, .4f);
            fillx(g, 0, 0, width, height);

            if(pressed)
            {
                instr->setMyColor(g, 1.f);
            }
            else
            {
                instr->setMyColor(g, .6f);
            }

            txt(g, FontVis, "S", width/2 - 2, height/2 + gGetTextHeight(FontVis)/2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }

public:

        SoloButton() : Button36(true) {}
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
            Instrument* instr = (Instrument*)parent;

            instr->setMyColor(g, .5f);

            fillx(g, 0, 0, width, height);

            instr->setMyColor(g, .65f);
            
            rectx(g, 0, 0, width, height);

            bool wVis = (instr->getDevice() && instr->getDevice()->isWindowVisible());

            if(wVis)
            {
                gGradRect(g, 0xffFF9930, x1, y1, x2, y2);
            }

            int tw = gGetTextWidth(fontId, instr->getAlias());
            int th = gGetTextHeight(fontId);

            if (wVis)
                //setc(g, (uint32)0xffFF9930);
                setc(g, 1.f);
            else
                instr->setMyColor(g, 1.f);

            txt(g, fontId, instr->getAlias(), width/2 - tw/2, height/2 + th/2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
};

class PreviewButt : public Button36
{
protected:

        void drawSelf(Graphics& g)
        {
            Instrument* instr = (Instrument*)parent;
            instr->setMyColor(g, .8f);

            gTriangle(g, x1, y1, x2, y1 + height/2, x1, y2);

            /*
            Instrument* instr = (Instrument*)parent;
            instr->setMyColor(g, .3f);
            int yc = 0;
            while (yc < height)
            {
                rectx(g, 0, yc, width, 1);
                yc += 2;
            }*/
        }

/*
        void handleMouseEnter(InputEvent & ev)  { redraw(); }
        void handleMouseLeave(InputEvent & ev)  { redraw(); }
        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
        //void handleMouseUp(InputEvent & ev)     { redraw();  parent->redraw();  }*/

public:

        PreviewButt() : Button36(true) {}
};


Instrument::Instrument(Device36* dev)
{
    device = dev;

    volBox = NULL;
    panBox = NULL;

    addObject(guiButton = new GuiButt());
    
    mixChannel = MMixer->masterChannel;       // Default to master channel

    addObject(volBox = new ParamBox(device->vol));
    volBox->setSliderOnly(true);

    addObject(panBox = new ParamBox(device->pan));
    panBox->setSliderOnly(true);

    addObject(muteButt = new EnableButton(device->enabled));
    
    //addObject(soloButt = new SoloButton());

    addObject(previewButton = new PreviewButt());

    addObject(ivu = new InstrVU(), ObjGroup_VU);

    ivu->setEnable(false);

    setDevice(dev);
}

Instrument::~Instrument()
{
    WaitForSingleObject(MixerMutex, INFINITE);

    setEnable(false);

    if(mixChannel != MMixer->masterChannel)
    {
        MMixer->deleteObject(mixChannel);
    }

    if(this == SoloInstr)
    {
        SoloInstr = NULL;
    }

    device->removeElements();

    ReleaseMutex(MixerMutex);
}

void Instrument::setDevice(Device36* dev)
{
    //if (device != devDummy)
    {
        volBox->removeParam(device->vol);
        panBox->removeParam(device->pan);
        muteButt->removeParam(device->enabled);

        if (device != devDummy)
        {
        //    device->setContainer(NULL);
        }

        setObjName("");
    }

    device = dev;

    if (device)
    {
        device->setContainer(this);

        //if (device != devDummy)
        {
            device->setVU(ivu);

            volBox->addParam(device->vol);
            panBox->addParam(device->pan);
            muteButt->addParam(device->enabled);

            setObjName(dev->getObjName());
        }
    }
}

void Instrument::activateMenuItem(std::string item)
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

void Instrument::addMixChannel()
{
    mixChannel = MMixer->addMixChannel(this);
}

ContextMenu* Instrument::createContextMenu()
{
    MInstrPanel->setCurrInstr(this);

    //Menu* menu = new Menu(Obj_MenuPopup);

    ContextMenu* menu = new ContextMenu(this);

    menu->addMenuItem("Clone");
    menu->addMenuItem("Delete");
    menu->addMenuItem("Edit Self-Pattern");

    return menu;
}

Instrument* Instrument::clone()
{
    Instrument* instr = NULL;
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

void Instrument::drawSelf(Graphics& g)
{
    //Gobj::fill(g, .35f);

    if(device != devDummy)
    {
        Gobj::setMyColor(g, .7f);

        fillx(g, 0, 0, width, height);

       // setc(g, .31f);
        Gobj::setMyColor(g, .5f);
        fillx(g, 0, 0, width, height/2);
    }
    else
    {
        Gobj::setMyColor(g, .4f);

        fillx(g, 0, 0, width, height);
    }

    //setc(g, .0f);
    Gobj::setMyColor(g, .9f);
    txtfit(g, FontSmall, getObjName(), guiButton->getW() + 4, 9, width - (height+4));

    //setc(g, 1.f);
    Gobj::setMyColor(g, 1.f);
    txtfit(g, FontSmall, getObjName(), guiButton->getW() + 4, 8, width - (height+4));


    //Colour clr = Colour(100, 110, 110);

    //Colour clr = Colour(Gobj::colorHue, Gobj::colorSat, 1, 1);

    //float s = .4f;
    //float b = .8f;
    //float a = 1;
    //Colour clr = Colour(colorHue, s, b, a);
    //drawGlassRect1(g, (float)x1, (float)y1, width, height, clr, 1, 8, false, false, false, false);

//    lineH(0, 0, width - 1);
}

void Instrument::drawOverChildren(Graphics & g)
{
    //setMonoColor(.9f);
    //gTextFit(g, FontSmall, instrAlias, x1 + 6, y2 - 3, width - (width/2));
}

std::list <Element*> Instrument::getNotesFromRange(float offset, float lastVisibleTick)
{
    std::list <Element*> noteList;

    for(auto note : device->notes)
    {
        if(note->getendtick() < offset || note == device->selfNote)
        {
            continue;
        }
        else if (note->gettick() > lastVisibleTick)
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

void Instrument::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    if(obj == previewButton || obj == ivu)
    {
        if (ev.clickDown)
        {
            MInstrPanel->setCurrInstr(this);

            preview(); 
        }
    }
    else if (obj == guiButton)
    {
        if (ev.clickDown)
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

void Instrument::handleMouseDown(InputEvent& ev)
{
    if(ev.leftClick)
    {
        MInstrPanel->setCurrInstr(this);

        if(ev.keyFlags & kbd_ctrl)
        {
            preview();
        }
        /*
        else
        {
            if(ev.keyFlags == 0 && ev.doubleClick)
            {
                showWindow(true);
            }
        }
        */
    }
}

void Instrument::handleMouseUp(InputEvent& ev)
{
    MAudio->releaseAllPreviews();

    MInstrPanel->setCurrInstr(this);
}

void Instrument::handleMouseDrag(InputEvent& ev)
{
    if(MObject->canDrag(this))
    {
        MObject->dragAdd(this, ev.mouseX, ev.mouseY);

        guiButton->release();
    }
}

void Instrument::handleMouseWheel(InputEvent& ev)
{
    parent->handleMouseWheel(ev);
}

void Instrument::load(XmlElement * instrNode)
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

void Instrument::preview(int note)
{
    if (device && device->selfNote)
    {
        device->selfNote->setNote(note);

        device->selfNote->preview(note);
    }
}

void Instrument::remap()
{
    guiButton->setCoords1(0, 0, height/1.5f, height);

    int bw = 12;

    if (device != devDummy)
    {
        guiButton->setTouchable(true);

        int slH = 4;

        if (panBox)
            panBox->setCoords1(width - 190, height - slH, 70, slH);

        if (volBox)
            volBox->setCoords1(width - 110, height - slH, 80, slH);

        //soloButt->setCoords1(width - bw*2 - 8, height - bw, bw, bw);

        muteButt->setCoords1(width - bw - bw, 0, bw, height);

        //previewButton->setCoords1(height, height/2, height/2, height/2);
    }
    else
    {
        guiButton->setTouchable(false);

        if (volBox)
            volBox->setVis(false);

        if (panBox)
            panBox->setVis(false);

        //soloButt->setVis(false);

        muteButt->setVis(false);

        previewButton->setVis(false);
    }

    ivu->setCoords1(width - bw, 0, bw, height);
    
    if(gGetTextWidth(FontSmall, objName) > width - 38 - 50 - 10)
    {
        setHint(objName);
    }
    else
    {
        setHint("");
    }
}

bool Instrument::isDummy()
{
    return (device == devDummy);
}

void Instrument::setIndex(int idx)
{
    if (device)
        device->devIdx = idx;

    int num = idx;

    //if(num == 10)
    //    num = 0;

    instrAlias = "a";

    char c;

    if(num < 10)
    {
        c = num + 0x30; // ASCII offset for numbers
    }
    else
    {
        c = num + 0x37; // ASCII offset for uppercase letters minus 0xB
        //c = num + 0x56; // ASCII offset for lowercase letters minus 0xB
    }

    instrAlias = c;
}

void Instrument::setBufferSize(unsigned bufferSize)
{
    mixChannel->setBufferSize(bufferSize);
}

void Instrument::save(XmlElement * instrNode)
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

void Instrument::setSampleRate(float sampleRate)
{
    mixChannel->setSampleRate(sampleRate);
}



