// 
// 
//


#include "36_note.h"
#include "36_paramnum.h"
#include "36_paramvol.h"
#include "36_parampan.h"
#include "36_keyboard.h"
#include "36_textinput.h"
#include "36_events_triggers.h"
#include "36_pattern.h"
#include "36_instr.h"
#include "36_sampleinstr.h"
#include "36_edit.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_instrpanel.h"
#include "36_grid.h"
#include "36_audio_dev.h"
#include "36_transport.h"
#include "36_text.h"
#include "36_ctrlpanel.h"




Note::Note()
{
    instr = NULL;

    vol = NULL;
    pan = NULL;
}

Note::Note(Instrument* i, int note_val)
{
    type = El_GenNote;

    instr = i;

    dev = (Device36*)instr;

    vol = new ParamVol("Volume");
    pan = new ParamPan("Panning");

    setNote(note_val);

    ticklen = (float)MTransp->getTicksPerBeat();

    tick2 = tick1 + ticklen;

    instr->addNote(this);
}

Note::~Note()
{
    deleteAllTriggers();

    patt->removeElement(this);

    MGrid->removeElement(this);

    instr->removeNote(this);

    delete vol;
    delete pan;
}

Note* Note::clone(Instrument* new_instr)
{
    return AddNote(tick1, line, new_instr == NULL ? instr : new_instr, noteValue, ticklen, vol->getValue(), pan->getValue(), patt);
}

Element* Note::clone()
{
    return (Element*)clone(NULL);
}

Parameter* Note::getParamByDisplayMode(GridDisplayMode mode)
{
    switch(mode)
    {
        case GridDisplayMode_Volumes:
            return vol;
        case GridDisplayMode_Pans:
            return pan;
        default:
            return vol;
    }
}

void Note::move(float dtick, int dtrack)
{
    Element::setPos(tick1 + dtick, line + dtrack);

    instr->reinsertNote(this);
}

void Note::load(XmlElement * xmlNode)
{
    Element::load(xmlNode);

    vol->setValue((float)xmlNode->getDoubleAttribute(T("Volume")));
    pan->setValue((float)xmlNode->getDoubleAttribute(T("Panning")));
}

void Note::recalc()
{
    if(!isdel())
    {
        calcFreq();

        calcframes();

        calculated = true;

        if (patt)
        {
            relocateTriggers();
        }
    }
}

void Note::preview(int note, bool update_instr)
{
    int noteval = (note != -1 ? note : noteValue);

    if (MAudio->isNotePlayingOnPreview(noteval))
    {
        return;
    }

    Trigger* tg = new Trigger(this, patt, NULL);

    tg->noteVal = noteval;

    tg->freq = NoteToFreq(noteval);

    addTrigger(tg);

    tg->previewTrigger = true;

    MAudio->addPreviewTrigger(tg);

    if(update_instr)
    {
        instr->lastNoteLength = getticklen();
        instr->lastNoteVol = vol->getValue();
        instr->lastNotePan = pan->getValue();
        instr->lastNoteVal = noteValue;
    }
}

void Note::calcFreq()
{
    freq = NoteToFreq(noteValue);
}

void Note::calcForGrid(Grid* grid)
{
     x1 = grid->getXfromTick(tick1);
     yBase = grid->getYfromLine(line)- 1;

     float lHeight = float(grid->getLineHeight() - 1);

     if (grid->getDisplayMode() == GridDisplayMode_Bars)
     {
         Parameter* param = getParamByDisplayMode(grid->getDisplayMode());

         float mul = param == NULL ? DAW_INVERTED_VOL_RANGE : param->getEditorValue();

         height = int(lHeight*mul); 

         y1 = yBase - height;
     }
     else if (grid->getDisplayMode() == GridDisplayMode_Volumes || grid->getDisplayMode() == GridDisplayMode_Pans)
     {
         Parameter* param = getParamByDisplayMode(grid->getDisplayMode());

         int start = int(lHeight*((0.f - param->getOffset())/param->getRange()));

         barDraw = int((lHeight - 1)*param->getEditorValue()) - start;
         barStart = yBase - start;
     }

     width = (int)(ticklen*grid->getPixelsPerTick());

     y2 = y1 + height - 1;
     x2 = x1 + width - 1;

    setDrawAreaDirectly(x1, y1, x2, y2);
}

void Note::drawOnGrid(Graphics& g, Grid* grid)
{
    if (grid->getDisplayMode() == GridDisplayMode_Bars)
    {
        FontId fnt = FontSmall;

        instr->setMyColor(g, .6f);
        gFillRect(g, x1, y1, x2, y2);

        instr->setMyColor(g, 1.f);
        gFillRect(g, x1, y1, x1, y2);

        //fillx(g, 1.f, .3f);

        //setc(g, 1.f, .8f);
        //lineH(g, 0, 0, width);

        //setc(g, .0f, .25f);
        //fillx(g, 0, 1, 7, 8);

        //setc(g, 0.f);
        //txt(g,fnt, instr->getAlias(), 1, height-1);
        //setc(g, 1.f);
        //txt(g,fnt, instr->getAlias(), 0, height-2);

        if(issel())
        {
            //fill(g, 1.f, .2f);
            instr->setMyColor(g, 1.f);
            rectx(g, x1, y1, x2, y2);
        }
    }
    else if (grid->getDisplayMode() == GridDisplayMode_Volumes || grid->getDisplayMode() == GridDisplayMode_Pans)
    {
        if(issel())
        {
            setc(g, 0xffFFF020, .9f);
        }
        else
        {
            instr->setMyColor(g, .9f);
        }

        int yTop = barStart;
        int yBot = yTop;
        int yCap = yTop;

        if (barDraw >= 0)
        {
            yTop -= barDraw;
            yCap = yTop;
        }
        else
        {
            yBot -= barDraw;
            yCap = yBot;
        }

        gFillRect(g, x1, yTop, x1, yBot);
        gFillRect(g, x1, yCap, x1 + 2, yCap);
    }

    //g.setColour(Colour(instr->color).withAlpha(0.8f));
    //gTextFit(g, FontSmall, instr->objName.data(), x1, y2 - 1, width);
}

void Note::handleMouseUp(InputEvent& ev)
{
    Element::handleMouseUp(ev);

    MAudio->releaseAllPreviews();
}

void Note::handleMouseDown(InputEvent & ev)
{
    Element::handleMouseDown(ev);

    if(ev.keyFlags & kbd_ctrl)
    {
        preview();
    }

    MInstrPanel->setCurrInstr(instr);
}

void Note::propagateTriggers(Pattern* sonPatt)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Trigger* tgStart = new Trigger(this, sonPatt, NULL);

    addTrigger(tgStart);

    Trigger* tgEnd = new Trigger(this, sonPatt, tgStart);

    addTrigger(tgEnd);

    if(calculated)
    {
        tgStart->locate();
        tgEnd->locate();
    }

    ReleaseMutex(AudioMutex);
}

void Note::releasePreview()
{
    MAudio->releasePreviewByElement(this);
}

void Note::setNote(int note_value)
{
    noteValue = note_value;
}

void Note::setPos(float tick,int line)
{
    Element::setPos(tick, line);

    //instr->reinsertNote(this);
}

void Note::save(XmlElement * xmlNode)
{
    Element::save(xmlNode);

    xmlNode->setAttribute(T("InstrIndex"), instr->getIndex());
    xmlNode->setAttribute(T("Volume"), vol->getValue());
    xmlNode->setAttribute(T("Panning"), pan->getValue());
}

void Note::unpropagateTriggers(Pattern* sonPatt)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Element::unpropagateTriggers(sonPatt);

    ReleaseMutex(AudioMutex);
}


SampleNote::SampleNote(Sample* smp, int note_val) : Note(smp, note_val)
{
    type = El_SampleNote;

    instr = sample = smp;
    sampleFrameLength = (long)sample->sample_info.frames;

    setTickLength(-1);

    reversed = false;
}

SampleNote* SampleNote::clone(Instrument* new_instr)
{
    SampleNote* clone = (SampleNote*)Note::clone(new_instr);

    clone->reversed = reversed;

    return clone;
}

void SampleNote::drawOnGrid(Graphics& g, Grid* grid)
{
    if(sample->waveImage == NULL)
    {
        sample->updWaveImage();
    }

    if (grid->getDisplayMode() == GridDisplayMode_Bars && MCtrllPanel->wavesAreVisible() && sample->waveImage != NULL)
    {
        setc(g, 1.f, 0.4f);

        g.saveState();
        g.reduceClipRegion(x1, y1, width - 1, height - 1);
        g.drawImageWithin(sample->waveImage, x1, y1, width, height, RectanglePlacement::stretchToFit);
        g.restoreState();
    }

    Note::drawOnGrid(g, grid);

    //g.setColour(Colour(instr->getColor()).withBrightness(1.f).withAlpha(0.8f));
    //gText(g, FontSmall, instr->instrAlias, x1 + 2, y1 + 13);

    //g.setColour(Colour(instr->getColor()).withBrightness(.6f));
    //gFillRect(g, x1, y1, x1 + 5, y1 + 10);

    //g.setColour(Colour(instr->getColor()).withAlpha(1.f));
    //std::string al = instr->alias;
    //gTextFit(g, FontSmall, instr->alias, x2 - 5, y1 + 8, width);
}

bool SampleNote::isOutOfBounds(double* cursor)
{
    if(*cursor > rightmostFrame || *cursor < leftmostFrame)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool SampleNote::initCursor(double* cursor)
{
    if(reversed == false)
    {
       *cursor = leftmostFrame;
    }
    else if(reversed == true)
    {
       *cursor = rightmostFrame;
    }

    if(patt->isBounded() && (patt != MPattern)&&((frame1 + *cursor) >= patt->getendframe()))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void SampleNote::calcFreq()
{
    int val = noteValue - BaseNote;

    dataStep = sample->calcSampleFreqIncrement(val);

    ticklen = MTransp->getTickFromFrame(sampleFrameLength)*sample->rateUp/CalcFreqRatio(val);

    tick2 = tick1 + ticklen;

    updateSampleBounds();
}

void SampleNote::load(XmlElement * xmlNode)
{
    Note::load(xmlNode);

    reversed = xmlNode->getBoolAttribute(T("Reversed"));
}

void SampleNote::recalc()
{
    Note::recalc();
}

void SampleNote::setTickLength(float tick_length)
{
    if(tick_length == -1)
    {
        // default length, based on the number of frames in the sample
        Element::setTickLength(MTransp->getTickFromFrame(sampleFrameLength)*sample->rateUp/CalcFreqRatio(noteValue - BaseNote));
    }
    else
    {
        Element::setTickLength(tick_length);
    }
}

void SampleNote::save(XmlElement * xmlNode)
{
    Note::save(xmlNode);

    xmlNode->setAttribute(T("Reversed"), int(reversed));
}

void SampleNote::setTickDelta(float tick_delta)
{
    
}

void SampleNote::updateSampleBounds()
{
    if(sample != NULL)
    {
        if(reversed == false)
        {
            /*
            if(ed_offs->value != 0)
            {
                leftmostFrame = (long)((float)(sampleFrameLength - 1)/100 * ed_offs->value);
            }
            else if(sample->params->offs->val != 0)
            {
                leftmostFrame = (long)((sampleFrameLength - 1) * sample->params->offs->val);
            }
            else*/
            {
                leftmostFrame = 0;
            }

            /*
            if(ed_len->value != 100)
            {
                rightmostFrame = (long)((float)(sampleFrameLength - 1)/ 100 * ed_len->value);
            }
            else if(sample->params->len->val != 1)
            {
                rightmostFrame = (long)((sampleFrameLength - 1) * sample->params->len->val);
            }
            else*/
            {
                rightmostFrame = (sampleFrameLength - 1);
            }

            //rightmostFrame = long(rightmostFrame*ksize);
        }
        else // rev == true
        {
            /*
            if(ed_offs->value != 0)
            {
                rightmostFrame = (sampleFrameLength - 1)/100 * (1 - ed_offs->value);
            }
            else if(sample->params->offs->val != 0)
            {
            	rightmostFrame = (long)((sampleFrameLength - 1) * (1 - sample->params->offs->val));
            }
            else*/
            {
                rightmostFrame = (sampleFrameLength - 1);
            }

            /*
            if(ed_len->value != 100)
            {
            	leftmostFrame = (sampleFrameLength - 1) * (1 - ed_len->value);
            }
            else if(sample->params->len->val != 1)
            {
            	leftmostFrame = (long)((sampleFrameLength - 1)*(1 - sample->params->len->val));
            }
            else*/
            {
                leftmostFrame = 0;
            }

            //leftmostFrame = long(leftmostFrame*ksize);
        }

        if(patt && patt->isBounded() == true)
        {
            long p_offs = 0;
            long p_cut = 0;

            if((patt != NULL)&&(patt != MPattern))
            {
                if(frame1 < 0)
                {
                    p_offs = (long)(abs(frame1)*sample->rateDown);
                }
                else
                {
                    p_offs = 0;
                }

                if(patt->getFrameLength() < frame2)
                {
                    p_cut = (long)(abs(frame2 - patt->getFrameLength())*(sample->rateDown));
                }
                else
                {
                    p_cut = 0;
                }
            }

            if(reversed == false)
            {
                if(p_offs > leftmostFrame)
                {
                    leftmostFrame = p_offs;
                }

                if(p_cut > ((sampleFrameLength - 1) - rightmostFrame))
                {
                    rightmostFrame = (sampleFrameLength - 1) - p_cut;
                }
            }
            else if(reversed == true)
            {
                if(p_offs > ((sampleFrameLength - 1) - rightmostFrame))
                {
                    rightmostFrame = (sampleFrameLength - 1) - p_offs;
                }

                if(p_cut > leftmostFrame)
                {
                    leftmostFrame = p_cut;
                }
            }
        }
    }
}


