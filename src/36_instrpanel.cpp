


#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_instrpanel.h"
#include "36_vstinstr.h"
#include "36_sampleinstr.h"
#include "36_scroller.h"
#include "36_vu.h"
#include "36_mixer.h"
#include "36_browser.h"
#include "36_project.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_button.h"
#include "36_brwentry.h"
#include "36_audio_dev.h"
#include "36_dragndrop.h"
#include "36.h"
#include "36_text.h"
#include "36_grid.h"
#include "36_edit.h"
#include "36_keyboard.h"
#include "36_params.h"
#include "36_slider.h"
#include "36_knob.h"
#include "36_numbox.h"





//// Libsndfile library
SNDFILE*                    sf_open(const char *path, int mode, SF_INFO *sfinfo);
sf_count_t                  sf_readf_float(SNDFILE *sndfile, float *ptr, sf_count_t frames);



//namespace M {

InstrPanel::InstrPanel(Mixer* mixer)
{
    instrOffset = 0;

    fxShowing = false;

    currInstr = instrs.begin();

    currMixChannel = NULL;

    int buttw = 25;

    addObject(scroller = new Scroller(true));

    masterVolume = new Parameter("Master", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB);

    addParamWithControl(masterVolume, "sl.vol", masterVolBox = new ParamBox(masterVolume));

    addObject(masterFX = new Button36(false), "bt.mfx");
    addObject(send1FX = new Button36(false), "s1.mfx");
    addObject(send2FX = new Button36(false), "s2.mfx");
    addObject(send3FX = new Button36(false), "s3.mfx");

    addObject(btShowFX = new Button36(false), "bt.showbrw");
    addObject(btHideFX = new Button36(false), "bt.hidebrw");

    addObject(mixr = mixer);

    addObject(masterVolKnob = new Knob(masterVolume));

    masterVolume->addControl(masterVolKnob);
}

void InstrPanel::editAutopattern(Instrument * instr)
{
    // (was here) Place cursor at the previously edited pattern
}

int InstrPanel::getFullHeight()
{
    int fulllen = 20;
    int instroffs;

    for(Instrument* i : instrs)
    {
        if(i->previewOnly == false)
        {
            instroffs = i->getH();

            fulllen += instroffs;
            fulllen += 1;
        }
    }

    return fulllen;
}

void InstrPanel::deleteInstrument(Instrument* i)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    if(currMixChannel != NULL)
    {
        currMixChannel->setEnable(false);

        removeObject(currMixChannel);

        currMixChannel = NULL;
    }

    if(getCurrInstr() == i)
    {
        currInstr++;

        if (currInstr == instrs.end())
        {
            currInstr--;

            if (currInstr != instrs.begin())
            {
                currInstr--;
            }
        }

        if (*currInstr != i)
        {
            //currMixChannel = (*currInstr)->mixChannel;
            //addObject(currMixChannel);
            //currMixChannel->setEnable(true);
        }
    }

    instrs.remove(i);

    updateInstrIndexes();

    if(i->previewOnly == false)
    {
        deleteObject(i);

        MProject.setChange();

        remapAndRedraw();

        //MGrid->syncToInstruments();

        MEdit->remapAndRedraw();

        MMixer->remapAndRedraw();
    }

    colorizeInstruments();

    ReleaseMutex(AudioMutex);
}

void InstrPanel::cloneInstrument(Instrument* i)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Instrument* ni = i->clone();

    // Place right after current instrument

    instrs.remove(ni);
    currInstr++;
    instrs.insert(currInstr, ni);
    currInstr--;

    updateInstrIndexes();

    // And set it current

    setCurrInstr(i);

    colorizeInstruments();

    // redraw all

    remapAndRedraw();

    MEdit->remapAndRedraw();

    ReleaseMutex(AudioMutex);
}

Instrument* InstrPanel::getInstrByIndex(int index)
{
    for(Instrument* instr : instrs)
    {
        if(instr->devIdx == index)
        {
            return instr;
        }
    }

    return NULL;
}

Instrument* InstrPanel::loadInstrFromBrowser(BrwEntry * be)
{
    Instrument* ni = NULL;

    if(be->ftype == FType_Wave)
    {
        ni = (Instrument*)addSample(be->path.data());
    }
    else 
    {
        VstInstr* vstgen = addVst(be->path.data(), NULL);

        ni = (Instrument*)vstgen;
    }

    if(ni)
    {
        setCurrInstr(ni);
    }

    return ni;
}

void InstrPanel::colorizeInstruments()
{
    int num = getInstrNum();

    float periodAngle = float(PI * 2 * .75f/num);

    static uint8 leadColor0 = 0;
    static uint8 leadColor1 = 0;
    static uint8 leadColor2 = 0;

    uint8 constantPart = 155;
    uint8 variablePart = 100;

    uint8 r = constantPart;
    uint8 g = constantPart;
    uint8 b = constantPart;

    uint8 alpha = 255;
    int idx = 0;

    float   hueOffset = 0.6f;
    float   hueSpan = .9f;

    for(Instrument* instr : instrs)
    {
        float angle = periodAngle*(idx%num);

        r = uint8(((sin(angle))*variablePart) + constantPart);
        b = uint8((sin(angle + PI/2)*variablePart) + constantPart);

        float hue = float(idx*hueSpan)/(float(num)) + hueOffset;

        while (hue > 1)
        {
            hue -= 1;
        }

        instr->defineHueColor(hue, 0.4f);

        idx++;
    }

    updateWaves();

    MGrid->redraw(false, false);
}

void InstrPanel::addInstrument(Instrument * i, Instrument * objAfter)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    instrs.push_back(i);

    if(instrs.size() == 1)
    {
        // If at least 1 instrument present, init iterator for current

        currInstr = instrs.begin();
    }

    //i->setColor();

    if(!MProject.isLoading())
    {
        updateInstrIndexes();
    }

    if(i->previewOnly == false)
    {
        i->addMixChannel();

        addObject(i, "instr");

        remapAndRedraw();

        MProject.setChange();

        if(MMixer->isShown())
        {
            i->mixChannel->setEnable(true);

            MMixer->remapAndRedraw();
        }

        //MGrid->syncToInstruments();

        MEdit->remapAndRedraw();
    }

    colorizeInstruments();

    ReleaseMutex(AudioMutex);
}
 
void InstrPanel::handleChildEvent(Gobj* obj, InputEvent& ev)
{
    if(ev.leftClick)
    {
        if (obj == scroller)
        {
            setOffset((int)scroller->getoffs());
        }
        else if(obj == btShowFX)
        {
            if (ev.clickDown)
            {
                obj->setEnable(false);
                btHideFX->setEnable(true);
                showFX();
            }
        }
        else if(obj == btHideFX)
        {
            if (ev.clickDown)
            {
                obj->setEnable(false);
                btShowFX->setEnable(true);
                hideFX();
            }
        }
    }
    else if (obj == scroller)
    {
        if(ev.wheelDelta != 0)
        {
            setOffset((int)(instrOffset - ev.wheelDelta*32));
        }
    }
}

void InstrPanel::handleMouseWheel(InputEvent& ev)
{
    setOffset((int)(instrOffset - ev.wheelDelta*30));
}

void InstrPanel::setOffset(int offs)
{ 
    if(fullSpan > visibleSpan)
    {
        instrOffset = offs;

        if(instrOffset < 0)
        {
            instrOffset = 0;
        }

        if(instrOffset + visibleSpan > fullSpan)
        {
            instrOffset = int(fullSpan - visibleSpan);
        }
    }
    else if (instrOffset > 0)
    {
        instrOffset = 0;
    }

    remapAndRedraw();

    //MGrid->setVerticalOffset(instrOffset);
}

Instrument* InstrPanel::getCurrInstr()
{
    if (instrs.size() > 0 && currInstr != instrs.end())
    {
        return *currInstr;
    }
    else
    {
        return NULL;
    }
}

void InstrPanel::setCurrInstr(Instrument* instr)
{
    if(*currInstr == instr)
    {
        return;
    }

    Instrument* oldInstr = *currInstr;

    currInstr = instrs.begin();

    // NULL sets the first instrument as current

    if (instr != NULL)
    {
        while(*currInstr != instr)
        {
            currInstr++;
        }
    }

    if(oldInstr != NULL && oldInstr->isWindowVisible())
    {
        oldInstr->showWindow(false);

        oldInstr->guiButt->release();
    }

    instr = *currInstr;

    redraw();
}

// Make current instrument always visible on panel
//
void InstrPanel::adjustOffset()
{
    updateInstrIndexes();

    Instrument* instr = getCurrInstr();

    if(instr)
    {
        int instrY = (1 + instr->getIndex())*(InstrHeight + 1);

        if(instrY + InstrHeight > instrOffset + getH())
        {
            setOffset(instrY - getH() + InstrHeight);
        }

        if(instrY - InstrHeight*2 < instrOffset)
        {
            setOffset(instrY - InstrHeight - InstrHeight);
        }

        MMixer->updateChannelIndexes();

/*
        int chanX = (1 + instr->mixChannel->getIndex())*(MixChanWidth + 1);

        if (chanX + MixChanWidth > MMixer->getOffset() + MMixer->getInstrChannelsRange())
        {
            MMixer->setOffset(chanX - MMixer->getInstrChannelsRange() + MixChanWidth);
        }

        if (chanX - MixChanWidth*2 < MMixer->getOffset())
        {
            MMixer->setOffset(chanX - MixChanWidth - MixChanWidth);
        }*/

        window->refreshActiveObject();
    }
}

void InstrPanel::updateInstrIndexes()
{
    int idx = 0;

    for(Instrument* instr : instrs)
    {
        instr->setIndex(idx++);
    }
}

void InstrPanel::updateInstrNotePositions()
{
    int idx = 0;

    for(Instrument* instr : instrs)
    {
        instr->updNotePositions();
    }
}

void InstrPanel::resetAll()
{
    for(Instrument* instr : instrs)
    {
        instr->reset();
    }
}

void InstrPanel::generateAll(long num_frames, long mixbuffframe)
{
    for(Instrument* instr : instrs)
    {
        instr->generateData(num_frames, mixbuffframe);
    }
}

VstInstr* InstrPanel::loadVst(const char* path, VstInstr* otherVst)
{
    VstInstr* vst = NULL;

    if(path != NULL)
    {
        vst = new VstInstr((char*)path, NULL);
    }
    else
    {
        vst = new VstInstr(NULL, otherVst);
    }

    if (vst->vst2 == NULL)
    {
        delete vst;

        return NULL;
    }

    if(path != NULL)
    {
        vst->filePath = path;
    }
    else
    {
        vst->filePath = otherVst->filePath;
    }

    return vst;
}

VstInstr* InstrPanel::addVst(const char* path, VstInstr* otherVst)
{
    VstInstr* vst = loadVst(path, otherVst);

    if(vst != NULL)
    {
        addInstrument(vst);
    }
    else
    {
       // show error box
    }

    return vst;
}

Sample* InstrPanel::loadSample(const char* path)
{
    SF_INFO         sf_info;
    SNDFILE*        soundfile = NULL;
    unsigned int    memsize;

    memset(&sf_info, 0, sizeof(SF_INFO));

    soundfile = sf_open(path, SFM_READ, &sf_info);

    if(sf_error(soundfile) != 0)
    {
        return NULL;
    }

    memsize = sf_info.channels*(unsigned int)sf_info.frames;

    float* data = new float[memsize*sizeof(float)];

    sf_readf_float(soundfile, data, sf_info.frames);

    if(sf_error(soundfile) != 0)
    {
        return NULL;
    }

    sf_close(soundfile);

    return new Sample(data, (char*)path, sf_info);
}

Sample* InstrPanel::addSample(const char* path, bool temporaryForPreview)
{
    Sample* smp = loadSample(path);

    if (smp != NULL)
    {
        smp->previewOnly = temporaryForPreview;

        addInstrument(smp);
    }

    return smp;
}

/*
void Add_SoundFont(const char* path, const char* name, const char* alias)
{
    sfBankID bank_id = LoadSF2Bank((char*)path);
    SoundFont* sf = new SoundFont(bank_id);

    strcpy(sf->sf_file_path, path);
    strcpy(sf->sf_name, name);

    sf->alias = new TString((char*)alias, false, NULL);
    sf->alias->instr = sf;
    Add_PEdit(sf->alias);

    instr[num_instrs] = (Instrument*)sf;
    if(num_instrs == 0)
    {
        current_instr = (Instrument*)sf;
    }
    num_instrs++;
}
*/

int InstrPanel::getInstrNum()
{
    return instrs.size();
}

void InstrPanel::setSampleRate(float sampleRate)
{
    for(Instrument* instr : instrs)
    {
        instr->setSampleRate(sampleRate);
    }
}

void InstrPanel::setBufferSize(unsigned bufferSize)
{
    for(Instrument* instr : instrs)
    {
        instr->setBufferSize(bufferSize);
    }
}

void InstrPanel::placeBefore(Instrument* instr, Instrument* before)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    instrs.remove(instr);

    auto it = instrs.begin();

    if(before == NULL)      // Insert to end
    {
        it = instrs.end();
    }
    else
    {
        for(; it != instrs.end() && *it != before; it++);
    }

    instrs.insert(it, instr);

    it--;

    currInstr = it;

    ReleaseMutex(AudioMutex);

    adjustOffset();

    colorizeInstruments();

    MInstrPanel->remapAndRedraw();

    MMixer->remapAndRedraw();
}

Instrument* InstrPanel::getInstrByAlias(std::string alstr)
{
    std::string str = alstr;

    str = ToUpperCase(str);

    for(auto i : instrs)
    {
        if(str == i->instrAlias)
        {
            return i;
        }
    }

    return NULL;
}

bool InstrPanel::handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my)
{
    if(!fxShowing || fxShowing && mx > FxPanelMaxWidth)
    {
        Gobj* o1 = NULL;
        Gobj* o2 = NULL;

        Gobj* o = CheckNeighborObjectsY(objs, "instr", my, (Gobj**)&o1, (Gobj**)&o2);

        dropObj = o;

        if(o2 != NULL)
        {
            drag.dropHighlightHorizontal->setCoords2(o2->getX1(), o2->getY1() - 2 - 4, o2->getX2() + 1, o2->getY1() - 2 + 4);
        }
        else if(o1 != NULL)
        {
            drag.dropHighlightHorizontal->setCoords2(o1->getX1(), o1->getY2() + 2 - 4, o1->getX2() + 1, o1->getY2() + 2 + 4);
        }

        int tw = gGetTextWidth(FontSmall, obj->getObjName());
        int th = gGetTextHeight(FontSmall);

        drag.setCoords1(mx - tw/2, my - th/2, tw, th);

        return true;
    }
    else
    {
        return false;
    }
}

bool InstrPanel::handleObjDrop(Gobj * obj, int mx, int my, unsigned int flags)
{
    BrwEntry* be = dynamic_cast<BrwEntry*>(obj);

    Instrument* i = NULL;

    if(be)
    {
        i = loadInstrFromBrowser(be);
    }
    else
    {
        i = getCurrInstr();

        if (flags & kbd_shift)
        {
            i = getCurrInstr()->clone();
        }

        if (i == dropObj)
        {
            i = NULL;
        }
    }

    if(i != NULL)
    {
        placeBefore(i, (Instrument*)dropObj);

        //MGrid->syncToInstruments();

        return true;
    }

    return false;
}

void InstrPanel::remap()
{
    confine();

    //masterVolSlider->setCoords1(width - 90, 6, 80, 16);
    //masterVolKnob->setCoords1(width - InstrControlWidth + 2, 2, 24, 24);
    masterVolBox->setCoords1(width - 120, 6, -1, 16);

    int instrListY = MainLineHeight - 1;
    int instrListHeight = (height - instrListY - BottomPadHeight - 1);
    int yoffs = -instrOffset;

    confine(0, instrListY, width, instrListY + instrListHeight - 1);

    fullSpan = 0;

    for (Instrument* i : instrs)
    {
        if(i->previewOnly == false)
        {
            if((yoffs + i->getH()) >= 0 && yoffs <= instrListHeight)
            {
                i->setCoords1(width - InstrControlWidth + 1, instrListY + yoffs, InstrControlWidth - 11, InstrHeight);
            }
            else
            {
                i->setVisible(false);
            }

            fullSpan += InstrHeight + 1;

            yoffs += InstrHeight + 1;
        }
    }

    confine();

    if (fxShowing)
    {
        btHideFX->setCoords1(FxPanelMaxWidth, 0, 28, 28);

        confine(0, instrListY, FxPanelMaxWidth, height);

        mixr->setCoords1(0, instrListY, FxPanelMaxWidth, instrListHeight);

        int xOffs = 4;
        int bh = MainLineHeight - 6;
        masterFX->setCoords1(xOffs, 2, 30, bh);
        xOffs += 30 + 1;
        send1FX->setCoords1(xOffs, 2, 30, bh);
        xOffs += 30 + 1;
        send2FX->setCoords1(xOffs, 2, 30, bh);
        xOffs += 30 + 1;
        send3FX->setCoords1(xOffs, 2, 30, bh);
    }
    else
    {
        btShowFX->setCoords1(0, 0, 28, 28);

        mixr->setVisible(false);

        masterFX->setVisible(false);

        send1FX->setVisible(false);
        send2FX->setVisible(false);
        send3FX->setVisible(false);
    }

    fullSpan += InstrHeight*3;
    visibleSpan = float(height);// - MainLineHeight);

    //MEdit->verticalGridScroller->updateLimits(fullSpan, visibleSpan, (float)instrOffset);
}

void InstrPanel::drawself(Graphics& g)
{
    fill(g, 0.1f);

    setc(g, .25f);
    fillx(g, 0, 0, width, MainLineHeight - 2);

    setc(g, 0.1f);
    gLineHorizontal(g, y1 + MainLineHeight - 2, x1, x2 + 1);

    gPanelRect(g, 0, y2 - BottomPadHeight + 1, x2, y2);
}

void InstrPanel::updateWaves()
{
    for(Instrument* instr : instrs)
    {
        Sample* smp = dynamic_cast<Sample*>(instr);

        if(smp != NULL)
        {
            if(smp->waveImage != NULL)
            {
                smp->updWaveImage();
            }
        }
    }
}

Instrument* InstrPanel::getInstrFromLine(int trkLine)
{
    if (trkLine < 0)
    {
        return instrs.front();
    }
    else if (trkLine > instrs.size())
    {
        return instrs.back();
    }

    int line = 0;

    for(Instrument* instr : instrs)
    {
        if (!instr->previewOnly && line == trkLine)
        {
            return instr;
        }

        line++;
    }

    return NULL;
}

void InstrPanel::setInstrHeight(int instr_height)
{
    
}

void InstrPanel::showFX()
{
    MObject->setMainX1(FxPanelMaxWidth + InstrControlWidth);

    fxShowing = true;

    remapAndRedraw();
}

void InstrPanel::hideFX()
{
    MObject->setMainX1(InstrControlWidth);

    fxShowing = false;

    remapAndRedraw();
}


//};

