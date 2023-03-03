

#include "36.h"
#include "36_instr.h"
#include "36_instrpanel.h"
#include "36_vu.h"
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
#include "36_parambox.h"
#include "36_paramvol.h"
#include "36_parampan.h"





class EnableButton : public Button36
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

            txt(g, FontVis, "M", width/2 - 2, height/2 + gGetTextHeight(FontVis)/2 - 1);
        }

        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }

public:

        EnableButton() : Button36(true) {}
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

        GuiButt() : Button36(false) {}

protected:

        void drawSelf(Graphics& g)
        {
            Instrument* instr = (Instrument*)parent;

            instr->setMyColor(g, .5f);

            fillx(g, 0, 0, width, height);

            //instr->setMyColor(g, .8f);
            //rectx(g, 0, 0, width, height);

            bool wVis = instr->isWindowVisible();

            if(wVis)
            {
                gGradRect(g, 0xffFF9930, x1, y1, x2, y2);
            }

            int tw = gGetTextWidth(FontBold, instr->getAlias());
            int th = gGetTextHeight(FontBold);

            if (wVis)
                //setc(g, (uint32)0xffFF9930);
                setc(g, 1.f);
            else
                instr->setMyColor(g, 1.f);

            txt(g, FontBold, instr->getAlias(), width/2 - tw/2, height/2 + th/2 - 1);
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

            instr->setMyColor(g, .3f);

            int yc = 0;

            while (yc < height)
            {
                rectx(g, 0, yc, width, 1);
                yc += 2;
            }
        }

        void handleMouseEnter(InputEvent & ev)  { redraw(); }
        void handleMouseLeave(InputEvent & ev)  { redraw(); }
        void handleMouseDrag(InputEvent & ev)   { parent->handleMouseDrag(ev); }
        void handleMouseWheel(InputEvent & ev)   { parent->handleMouseWheel(ev); }
        //void handleMouseUp(InputEvent & ev)     { redraw();  parent->redraw();  }

public:

        PreviewButt() : Button36(true) {}
};


Instrument::Instrument()
{
    selfPattern = NULL;
    selfNote = NULL;

    addObject(volBox = new ParamBox(vol));
    volBox->setSliderOnly(true);

    addObject(panBox = new ParamBox(pan));
    panBox->setSliderOnly(true);

    mixChannel = MMixer->masterChannel;       // Default to master channel

    addObject(previewButton = new PreviewButt());
    addObject(guiButton = new GuiButt());
    addObject(muteButt = new EnableButton());
    addObject(soloButt = new SoloButton());

    addObject(ivu = new InstrVU(), ObjGroup_VU);

    ivu->setEnable(false);
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

    removeElements();

    ReleaseMutex(MixerMutex);
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

// Create self-pattern + note, for previewing
//
void Instrument::createSelfPattern()
{
    if(selfPattern == NULL)
    {
        selfPattern = new Pattern("self", 0.0f, 16.0f, 0, 0, true);
//        selfPattern->setBasePattern(selfPattern);
        selfPattern->setPattern(selfPattern);
        selfPattern->addInstance(selfPattern);

        MProject.patternList.push_front(selfPattern);
    }

    if(selfNote == NULL)
    {
        selfNote = CreateNote(0, 0, this, BaseNote, 4, 1, 0, selfPattern);

        selfNote->propagateTriggers(selfPattern);
    }

    selfPattern->recalc();
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

    switch(type)
    {
        case Instr_Sample:
            // TODO: rework sample cloning to copy from memory
            instr = (Instrument*)MInstrPanel->addSample(filePath.data());
            break;

        case Instr_VstPlugin:
            instr = (Instrument*)MInstrPanel->addVst(NULL, (VstInstr*)this);
            break;
    }

    return instr;
}

void Instrument::drawSelf(Graphics& g)
{
    //Gobj::fill(g, .35f);

    Gobj::setMyColor(g, .7f);
    fillx(g, 0, 0, width, height);

   // setc(g, .31f);
    Gobj::setMyColor(g, .6f);
    fillx(g, 0, 0, width, height/2);

    //setc(g, .0f);
    Gobj::setMyColor(g, .1f);
    txtfit(g, FontSmall, objName, height+2, 9, width - (height+2));

    //setc(g, 1.f);
    Gobj::setMyColor(g, 1.f);
    txtfit(g, FontSmall, objName, height+2, 8, width - (height+2));



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

    for(auto note : notes)
    {
        if(note->getendtick() < offset || note == selfNote)
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
    if(obj == previewButton)
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

            showWindow(!isWindowVisible());
        }
    }

    redraw();
}

void Instrument::handleMouseDown(InputEvent& ev)
{
    if(ev.leftClick)
    {
        MInstrPanel->setCurrInstr(this);

        //if(ev.keyFlags & kbd_ctrl)
        {
          //  preview();
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
}

void Instrument::preview(int note)
{
    selfNote->setNote(note);

    selfNote->preview(note);
}

void Instrument::remap()
{
    volBox->setCoords1(width - 90, height - 10, 50, 10);
    panBox->setCoords1(width - 150, height - 10, 50, 10);

    previewButton->setCoords1(height + 2, height - 10, 20, 10);
    volBox->setCoords1(width - 84, height - 10, 50, 10);
    panBox->setCoords1(width - 149, height - 10, 50, 10);

    int bw = 11;

    soloButt->setCoords1(width - bw*2 - 8, height - bw, bw, bw);
    muteButt->setCoords1(width - bw - 8, height - bw, bw, bw);

    guiButton->setCoords1(0, 0, height, height-1);

    ivu->setCoords1(width - 8, 0, 8, height);

    if(gGetTextWidth(FontSmall, objName) > width - 38 - 50 - 10)
    {
        setHint(objName);
    }
    else
    {
        setHint("");
    }
}

void Instrument::setIndex(int idx)
{
    devIdx = idx;

    int num = devIdx;

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
    instrNode->setAttribute(T("InstrIndex"), devIdx);
    instrNode->setAttribute(T("InstrType"), int(type));
    instrNode->setAttribute(T("InstrName"), String(objName.data()));
    instrNode->setAttribute(T("InstrPath"), String(filePath.data()));

    instrNode->addChildElement(vol->save());
    instrNode->addChildElement(pan->save());

    /*
    instrNode->setAttribute(T("Mute"), int(muteparam->getOutVal()));
    instrNode->setAttribute(T("Solo"), int(soloparam->getOutVal()));
    */
}

void Instrument::setSampleRate(float sampleRate)
{
    mixChannel->setSampleRate(sampleRate);
}



