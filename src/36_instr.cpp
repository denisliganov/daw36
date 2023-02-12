

#include "36.h"
#include "36_instr.h"
#include "36_params.h"
#include "36_instrpanel.h"
#include "36_vu.h"
#include "36_edit.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_vst.h"
#include "36_textinput.h"
#include "36_browser.h"
#include "36_mixer.h"
#include "36_audio_dev.h"
#include "36_menu.h"
#include "36_draw.h"
#include "36_slider.h"
#include "36_button.h"
#include "36_project.h"
#include "36_text.h"
#include "36_events_triggers.h"
#include "36_knob.h"
#include "36_parambox.h"

//namespace M {

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

            instr->setMyColor(g, .6f);

            fillx(g, 0, 0, width, height);

            instr->setMyColor(g, .8f);

            rectx(g, 0, 0, width, height);

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

            //int gap = 3;
            //gTriangle(g, x1 + gap,y1 + gap, x2 - gap, y1 + height/2, x1 + gap, y2 - gap, clr1, clr1);

            //gTriangle(g, x1,y1, x2, y1 + height/2, x1, y2);

            //if(instr == _MInstrPanel->getCurrInstr())
            //    gSetColor(g, 0xffFFDD60);
            //else
            //    instr->setDrawColor(g, 1);
            //std::string str = String::formatted(T("%s"), instr->instrAlias.data());
            //gText(g, FontSmall, str, x1 + 12, y1 + height/2 + 4);
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
    envVol = NULL;
    mixChannel = NULL;
    selfPattern = NULL;
    selfNote = NULL;
    devIdx = -1;
    rampCounterV = 0;
    cfsV = 0;
    rampCounterP = 0;
    cfsP = 0;

    lastNoteLength = 4;
    lastNoteVol = 1;
    lastNotePan = 0;
    lastNoteVal = BaseNote;

    addParam(vol = new Parameter("VOL", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB));
    addParam(pan = new Parameter("PAN", Param_Pan, 0.f, -1.f, 2.f, Units_Percent));
    
    addObject(volBox = new ParamBox(vol));
    volBox->setSliderOnly(true);
    addObject(panBox = new ParamBox(pan));
    panBox->setSliderOnly(true);

    //addParamWithControl(muteparam = new BoolParam(false), "tg.mute", mute = new Button36(true));

    muteparam = soloparam = NULL;

    mixChannel = MMixer->masterChannel;       // Default to master channel

    addObject(previewButton = new PreviewButt());
    addObject(guiButton = new GuiButt());
    addObject(muteButt = new EnableButton());
    addObject(soloButt = new SoloButton());

    addObject(ivu = new InstrVU(), ObjGroup_VU);
    ivu->setEnable(false);

    rampCount = 512;
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

void Instrument::activateTrigger(Trigger* tg)
{
    Note* note = (Note*)tg->el;

    tg->outsync = false;
    tg->tgworking = true;
    tg->muted = false;
    tg->broken = false;
    tg->framePhase = 0;
    tg->auCount = 0;
    tg->volBase = note->vol->getOutVal();
    tg->panBase = note->pan->getOutVal();

    tg->setState(TS_Sustain);

    tg->noteVal = note->getNoteValue();
    tg->freq = note->freq;

    tg->wt_pos = 0;

    activeTriggers.remove(tg);
    activeTriggers.push_back(tg);
}

void Instrument::addNote(Note * note)
{
    std::list<Note*>::iterator it = notes.begin();

    while(1)
    {
        if (it == notes.end())
        {
            notes.insert(it, note);
            break;
        }

        Note* n = *it;

        if(n == selfNote)
        {
            it++;
            continue;
        }

        if(n->gettick() > note->gettick())
        {
            notes.insert(it, note);
            break;
        }
        else if (n->gettick() == note->gettick())
        {
            if(n->getNoteValue() < note->getNoteValue())
            {
                notes.insert(it, note);
                break;
            }
        }

        it++;
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

    makeClone(instr);

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

//    lineH(0, 0, width - 1);
}

void Instrument::drawover(Graphics & g)
{
    //setMonoColor(.9f);
    //gTextFit(g, FontSmall, instrAlias, x1 + 6, y2 - 3, width - (width/2));
}

void Instrument::deactivateTrigger(Trigger* tg)
{
    activeTriggers.remove(tg);

    tg->tgworking = false;
}

// Perform fadeout or fadein antialiasing
//
// [IN]     stuff
//
void Instrument::deClick(Trigger* tg, long num_frames, long buffframe, long mixbuffframe, long buff_remaining)
{
    long tc0;

    float mul;

    if(tg->aaOUT)
    {
        int nc = 0;

        tc0 = buffframe*2;

        while(nc < num_frames)
        {
            if(tg->aaCount > 0)
            {
                mul = (float)tg->aaCount/DECLICK_COUNT;

                inBuff[tc0] *= mul;
                inBuff[tc0 + 1] *= mul;

                tc0++; tc0++;

                tg->aaCount--;
            }
            else
            {
                inBuff[tc0] = 0;
                inBuff[tc0 + 1] = 0;

                tc0++; tc0++;
            }

            nc++;
        }

        if(tg->aaCount == 0)
        {
            tg->aaOUT = false;
        }
    }
    else if(tg->aaIN)
    {
        int nc = 0;

        tc0 = buffframe*2;

        while(nc < num_frames && tg->aaCount > 0)
        {
            mul = (float)(DECLICK_COUNT - tg->aaCount)/DECLICK_COUNT;

            inBuff[tc0] *= mul;
            inBuff[tc0 + 1] *= mul;

            tc0++;
            tc0++;
            nc++;

            tg->aaCount--;
        }

        if(tg->aaCount == 0)
        {
            tg->aaIN = false;
        }
    }

    // Check if finished

    bool aaU = false;
    int aaStart;

    if(tg->tgState == TS_SoftFinish)
    {
        // Finished case

        aaU = true;

        if(tg->auCount == 0)
        {
            aaStart = endframe - DECLICK_COUNT;

            if(aaStart < 0)
            {
                aaStart = 0;
            }
        }
        else
        {
            tg->setState(TS_Finished);

            aaStart = 0;
        }
    }

    // Finish declick processing

    if(aaU == true)
    {
        jassert(aaStart >= 0);

        if(aaStart >= 0)
        {
            tc0 = buffframe*2 + aaStart*2;

            while(aaStart < num_frames)
            {
                if(tg->auCount < DECLICK_COUNT)
                {
                    tg->auCount++;

                    mul = float(DECLICK_COUNT - tg->auCount)/DECLICK_COUNT;

                    inBuff[tc0] *= mul;
                    inBuff[tc0 + 1] *= mul;

                    tc0++;
                    tc0++;
                }
                else
                {
                    inBuff[tc0] = 0;
                    inBuff[tc0 + 1] = 0;

                    tc0++;
                    tc0++;
                }

                aaStart++;
            }
        }
    }
}

void Instrument::forceStop()
{
restart:

    for(Trigger* tg :activeTriggers)
    {
        tg->stop();

        goto restart;
    }
}

void Instrument::fillMixChannel(long num_frames, long buffframe, long mixbuffframe)
{
    float volval = vol->getOutVal();

    if(vol->lastValue == -1)
    {
        vol->setLastVal(vol->getOutVal());
    }
    else if(vol->lastValue != vol->getOutVal())
    {
        if(rampCounterV == 0)
        {
            cfsV = float(vol->getOutVal() - vol->lastValue)/DECLICK_COUNT;

            volval = vol->lastValue;

            rampCounterV = DECLICK_COUNT;
        }
        else
        {
            volval = vol->lastValue + (DECLICK_COUNT - rampCounterV)*cfsV;
        }
    }
    else if(rampCounterV > 0) // (paramSet->vol->lastval == paramSet->vol->outval)
    {
        rampCounterV = 0;
        cfsV = 0;
    }

    float lMax, rMax, outL, outR;
    lMax = rMax = 0;

    long tc0 = buffframe*2;
    long tc = mixbuffframe*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        if(rampCounterV > 0)
        {
            volval += cfsV;

            rampCounterV--;

            if(rampCounterV == 0)
            {
                vol->setLastVal(vol->getOutVal());
            }
        }

        outL = outBuff[tc0++]*volval;
        outR = outBuff[tc0++]*volval;

        if (mixChannel != NULL)
        {
            mixChannel->inbuff[tc++] += outL;
            mixChannel->inbuff[tc++] += outR;
        }

        if(c_abs(outL) > lMax)
        {
            lMax = outL;
        }

        if(c_abs(outR) > rMax)
        {
            rMax = outR;
        }
    }

    // Update VU

    ivu->setValues(lMax, rMax);
}

// This function helps to declick when we're forcing note removal due to lack of free voice slots
//
void Instrument::flowTriggers(Trigger* tgfrom, Trigger* tgto)
{
    memset(outBuff, 0, rampCount*sizeof(float)*2);
    memset(inBuff, 0, rampCount*sizeof(float)*2);
    memset(tgto->auxbuff, 0, rampCount*sizeof(float)*2);

    long actual = workTrigger(tgfrom, rampCount, rampCount, 0, 0);

    for(int ic = 0; ic < actual; ic++)
    {
        outBuff[ic*2] *= float(actual - ic)/actual;
        outBuff[ic*2 + 1] *= float(actual - ic)/actual;
    }

    memcpy(tgto->auxbuff, outBuff, actual*sizeof(float)*2);

    tgto->lcount = (float)rampCount;
}

void Instrument::generateData(long num_frames, long mixbuffframe)
{
    long        frames_remaining;
    long        frames_to_process;
    long        actual;
    long        buffframe;
    long        mbframe;
    Trigger*    tgenv;

    memset(outBuff, 0, num_frames*sizeof(float)*2);

    frames_remaining = num_frames;
    buffframe = 0;
    mbframe = mixbuffframe;

    while(frames_remaining > 0)
    {
        if(frames_remaining > BUFF_CHUNK_SIZE)
        {
            frames_to_process = BUFF_CHUNK_SIZE;
        }
        else
        {
            frames_to_process = frames_remaining;
        }

        // Process envelopes for this instrument
        tgenv = envelopes;

        // Rewind to the very first, to provide correct envelopes overriding
        while(tgenv != NULL && tgenv->group_prev != NULL) 
        {
            tgenv = tgenv->group_prev;
        }

        // Now process them all

        while(tgenv != NULL)
        {
            /*
            env = (Envelope*)tgenv->el;
            if(env->newbuff && buffframe >= env->last_buffframe)
            {
                param = env->param;
                param->SetValueFromEnvelope(env->buff[mixbuffframe + buffframe], env);
            }*/

            tgenv = tgenv->group_next;
        }

        /*
        // Process activeTriggers for the current chunk
        tg = tg_first;
        while(tg != NULL)
        {
            tgnext = tg->loc_act_next;
            // Clean inBuff to ensure obsolete data won't be used
            memset(inBuff, 0, num_frames*sizeof(float)*2);
            actual = workTrigger(tg, frames_to_process, frames_remaining, buffframe, mbframe);
            tg = tgnext;
        }
        */

        for(auto itr = activeTriggers.begin(); itr != activeTriggers.end(); )
        {
            // Clean inBuff to ensure obsolete data won't be used

            memset(inBuff, 0, num_frames*sizeof(float)*2);

            Trigger* tg = *itr;

            itr++;

            actual = workTrigger(tg, frames_to_process, frames_remaining, buffframe, mbframe);
        }

        frames_remaining -= frames_to_process;
        buffframe += frames_to_process;
        mbframe += frames_to_process;
    }

    // Send data to assigned mixer channel

    fillMixChannel(num_frames, 0, mixbuffframe);
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
    //MInstrPanel->setCurrInstr(this);

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

    bool mute = (instrNode->getIntAttribute(T("mute")) == 1);
    muteparam->SetBoolValue(mute);

    bool solo = (instrNode->getIntAttribute(T("Solo")) == 1);
    soloparam->SetBoolValue(solo);

    if(solo)
    {
        SoloInstr = this;
    }
}

Instrument* Instrument::makeClone(Instrument * instr)
{
    auto itr1 = params.begin();
    auto itr2 = instr->params.begin();

    for(; itr1 != params.end(); itr1++, itr2++)
    {
        ((Parameter*)(*itr2))->setValue(((Parameter*)(*itr1))->getValue());
    }

    return instr;
}

void Instrument::preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe)
{
    if(tg->broken)
    {
        // Per break or mute we could leave filling for two or more times, if there're not enough num_frames
        // to cover ANTIALIASING_FRAMES number

        if(tg->aaFilledCount >= DECLICK_COUNT)
        {
           *fill = false;
           *skip = true;

            tg->setState(TS_Finished);
        }
        else
        {
            tg->aaFilledCount += num_frames;
        }
    }

    // Check conditions for muting
    if((muteparam != NULL && muteparam->getOutVal()) || !(SoloInstr == NULL || SoloInstr == this))
    {
        // If note just begun then there's nothing to declick. Set aaFilledCount to full for immediate muting

        if(tg->framePhase == 0)
        {
            tg->muted = true;
            tg->aaFilledCount = DECLICK_COUNT;
        }

        if(tg->muted == false)
        {
            // If note was already playig, then need to continue until DECLICK_COUNT number of frames are filled

            tg->muted = true;
            tg->aaOUT = true;
            tg->aaCount = DECLICK_COUNT;
            tg->aaFilledCount = num_frames;
        }
        else
        {
            if(tg->aaFilledCount >= DECLICK_COUNT)
            {
               *fill = false;
            }
            else
            {
                tg->aaFilledCount += num_frames;
            }
        }
    }
    else if(*fill == true)
    {
        if(tg->muted)
        {
            tg->muted = false;
            tg->aaIN = true;
            tg->aaCount = DECLICK_COUNT;
        }
    }
}

void Instrument::preview(int note)
{
    selfNote->setNote(note);

    selfNote->preview(note);
}

// Post-process data, generated per trigger. 
// Apply all high-level params, envelopes and fill corresponding mixcell.
//
// [IN]
//    tg            - trigger being processed.
//    num_frames    - number of frames to process.
//    curr_frame    - global timing frame number.
//    buffframe     - global buffering offset.
//
void Instrument::postProcessTrigger(Trigger* tg, long num_frames, long buffframe, long mixbuffframe, long buff_remaining)
{
    float       vol, pan;
    float       volL, volR;
    long        tc, tc0;


    deClick(tg, num_frames, buffframe, mixbuffframe, buff_remaining);

    vol = tg->vol_val*volbase;

    pan0 = tg->pan_val;

    tc = mixbuffframe*2;
    tc0 = buffframe*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        // pan0 is unchanged
        // then to pattern/env

        // pan2 is unchanged
        // then to instrument/env

        pan = (((pan0*(1 - c_abs(pan1)) + pan1)*(1 - c_abs(pan2)) + pan2)*(1 - c_abs(pan3)) + pan3);

        //tg->lastpan = pan;
        ////PanLinearRule(pan, &volL, &volR);
        //pan = 0;
        //volL = volR = 1;
        //if(pan > 0)
        //    volL -= pan;
        //else if(pan < 0)
        //    volR += pan;
        //PanConstantRule(pan, &volL, &volR);

        int ai = int((PI_F*(pan + 1)/4)/wt_angletoindex);

        volL = wt_cosine[ai];
        volR = wt_sine[ai];

        outBuff[tc0] += inBuff[tc0]*vol*volL;
        outBuff[tc0 + 1] += inBuff[tc0 + 1]*vol*volR;

        tc0++;
        tc0++;
    }
}

void Instrument::removeNote(Note * note)
{
    notes.remove(note);
}

void Instrument::reinsertNote(Note * note)
{
    notes.remove(note);

    addNote(note);
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

// Here initialized the stuff that does not change throughout the whole session
void Instrument::staticInit(Trigger* tg, long num_frames)
{
    pan0 = pan1 = pan2 = pan3 = 0;

    volbase = 0;
    venvphase = 0;
    endframe = 0;

    /// Init volume base. Volume base consist of volumes, that don't change during the whole filling session.
    /// it's then multiplied to dynamic tg->vol_val value in postProcessTrigger.

    volbase = 1; // tg->tgPatt->vol->outval; // Pattern static paramSet

    volbase *= DAW_INVERTED_VOL_RANGE;

//    if(type == Instr_Generator)  
//        volbase *= DAW_INVERTED_VOL_RANGE;

    // Init pans
    pan1 = 0;
    pan2 = 0; // tg->tgPatt->pan->outval;     // Pattern's local panning
    pan3 = pan->getOutVal();     // Instrument's panning
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

void Instrument::setLastParams(float last_length,float last_vol,float last_pan, int last_val)
{
    lastNoteLength = last_length;
    lastNoteVol = last_vol;
    lastNotePan = last_pan;
    lastNoteVal = last_val;
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

long Instrument::workTrigger(Trigger * tg, long num_frames, long remaining, long buffframe, long mixbuffframe)
{
    staticInit(tg, num_frames);

    long actual_num_frames = processTrigger(tg, num_frames, buffframe);

    if(actual_num_frames > 0)
    {
        postProcessTrigger(tg, actual_num_frames, buffframe, mixbuffframe, remaining - actual_num_frames);
    }

    if(tg->lcount > 0)
    {
        for(int ic = 0; ic < actual_num_frames; ic++)
        {
            outBuff[(buffframe + ic)*2] += tg->auxbuff[int(rampCount - tg->lcount)*2];
            outBuff[(buffframe + ic)*2 + 1] += tg->auxbuff[int(rampCount - tg->lcount)*2 + 1];

            tg->lcount--;

            if(tg->lcount == 0)
            {
                break;
            }
        }
    }

    // If trigger was finished during processing, deactivate it here

    if(tg->tgState == TS_Finished)
    {
        tg->stop();
    }

    return actual_num_frames;
}


//}

