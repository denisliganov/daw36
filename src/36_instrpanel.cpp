


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
#include "36_parambox.h"





//// Libsndfile library
SNDFILE*                    sf_open(const char *path, int mode, SF_INFO *sfinfo);
sf_count_t                  sf_readf_float(SNDFILE *sndfile, float *ptr, sf_count_t frames);



class InstrHighlight : public Gobj
{
public:

        InstrHighlight() 
        {
            settouchable(false);
        }

        void updPos()
        {
            Instrument* instr = MInstrPanel->getCurrInstr();

            if (instr && instr->isshown())
            {
                setCoordsAbs(0, instr->getY1() - 1, instr->getX2() + 5, instr->getY2() + 1);
                //setCoords2(0, instr->getY1() - 1, instr->getX2() + 5, instr->getY2() + 1);
            }
            else
            {
                setVis(false);
            }
        }

protected:

        void drawSelf(Graphics& g)
        {
            //gSetMonoColor(g, 1, 0.7f);
            //setc(g, 0xffFFC030, 1, .5f);

            //rect(g, 0xa0FFC030);
/*
            setc(g, 0xb0FFA000);
            lineH(g, 0, 0, width-1);
            lineH(g, height-1, 0, width-1);
            fillx(g, width-3, 4, 1, height-8);
*/
            setc(g, 0xe0FF9930);
            gDrawRect(g,  x1, y1, x2, y2);
        }
};


InstrPanel::InstrPanel(Mixer* mixer)
{
    fxShowing = false;

    currInstr = instrs.begin();

    currMixChannel = NULL;

    int buttw = 25;

    addObject(btShowFX = new Button36(false), "bt.showbrw");
    addObject(btHideFX = new Button36(false), "bt.hidebrw");

    addObject(mixr = mixer);

    //addParamWithControl(masterVolume, "sl.vol", masterVolBox = new ParamBox(masterVolume));

    masterVolume = new Parameter("Master", Param_Vol, 1.f, 0.f, DAW_VOL_RANGE, Units_dB);

    addObject(masterVolBox = new ParamBox(masterVolume));

    masterVolBox->setSliderOnly(true);

    //addObject(masterVolKnob = new Knob(masterVolume));
    //masterVolume->addControl(masterVolKnob);

    addHighlight(instrHighlight = new InstrHighlight());
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

        if(MMixer->isshown())
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

void InstrPanel::colorizeInstruments()
{
    int num = getNumInstrs();

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

void InstrPanel::drawSelf(Graphics& g)
{
    fill(g, 0.1f);
    setc(g, .25f);
    fillx(g, 0, 0, width, MainLineHeight);

    //setc(g, 0.1f);
    //gLineHorizontal(g, y1 + MainLineHeight - 2, x1, x2 + 1);
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

int InstrPanel::getNumInstrs()
{
    int num = 0;

    for(Instrument* instr : instrs)
    {
        if (instr->previewOnly == false)
            num++;
    }

    return num;
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

void InstrPanel::generateAll(long num_frames, long mixbuffframe)
{
    for(Instrument* instr : instrs)
    {
        instr->generateData(num_frames, mixbuffframe);
    }
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
            //drag.dropHighlightHorizontal->setCoords2(o2->getX1(), o2->getY1() - 2 - 4, o2->getX2() + 1, o2->getY1() - 2 + 4);
            drag.dropHighlightHorizontal->setCoordsUn(o2->getX1(), o2->getY1() - 2 - 4, o2->getX2() + 1, o2->getY1() - 2 + 4);
        }
        else if(o1 != NULL)
        {
            //drag.dropHighlightHorizontal->setCoords2(o1->getX1(), o1->getY2() + 2 - 4, o1->getX2() + 1, o1->getY2() + 2 + 4);
            drag.dropHighlightHorizontal->setCoordsUn(o1->getX1(), o1->getY2() + 2 - 4, o1->getX2() + 1, o1->getY2() + 2 + 4);
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

        updateInstrIndexes();

        return true;
    }

    return false;
}

void InstrPanel::handleChildEvent(Gobj* obj, InputEvent& ev)
{
    if(ev.leftClick)
    {
        if(obj == btShowFX)
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
}

void InstrPanel::handleMouseWheel(InputEvent& ev)
{
    //
}

void InstrPanel::hideFX()
{
    MObject->setMainX1(InstrControlWidth);

    fxShowing = false;

    remapAndRedraw();
}

void InstrPanel::updateInstrIndexes()
{
    int idx = 0;

    for(Instrument* instr : instrs)
    {
        instr->setIndex(idx++);
    }
}

Instrument* InstrPanel::loadInstrFromBrowser(BrwEntry * be)
{
    if (getNumInstrs() >= 36)
    {
        MWindow->showAlertBox("Can't load more than 36 instruments");

        return NULL;
    }

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

    if (!vst->isLoaded())
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

    colorizeInstruments();

    MInstrPanel->remapAndRedraw();

    MMixer->remapAndRedraw();
}

void InstrPanel::resetAll()
{
    for(Instrument* instr : instrs)
    {
        instr->reset();
    }
}

void InstrPanel::remap()
{
    confine();

    masterVolBox->setCoords1(width - 120, 6, -1, 16);

    int instrListY = MainLineHeight + 1;
    int instrListHeight = height - (instrListY + BottomPadHeight);

    confine(0, instrListY, width, instrListY + instrListHeight - 1);

    int num = getNumInstrs();

    int ih = 33;

    while (num*(ih + 1) > instrListHeight)
    {
        ih--;
    }

    InstrHeight = ih;

    int yoffs =0;

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

            yoffs += InstrHeight + 1;
        }
    }

    confine();

    if (fxShowing)
    {
        btHideFX->setCoords1(FxPanelMaxWidth, 0, 28, 28);

        confine(10, instrListY, FxPanelMaxWidth, height);

        mixr->setCoords1(10, instrListY, FxPanelMaxWidth - 10, instrListHeight);
    }
    else
    {
        btShowFX->setCoords1(0, 0, 28, 28);

        mixr->setVis(false);
    }

    confine(0, instrListY-1, width, instrListY + instrListHeight - 1);

    instrHighlight->updPos();
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

    /*
    if(oldInstr != NULL && oldInstr->isWindowVisible())
    {
        oldInstr->showWindow(false);
        oldInstr->previewButton->release();
    }*/

    instr = *currInstr;

    redraw();

    instrHighlight->updPos();
}

void InstrPanel::showFX()
{
    MObject->setMainX1(FxPanelMaxWidth + InstrControlWidth);

    fxShowing = true;

    remapAndRedraw();
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


//};

