


#include "rosic/rosic.h"

#include "36_browserlist.h"
#include "36_globals.h"
#include "36_instrpanel.h"
#include "36_instr.h"
#include "36_vst.h"
#include "36_sampleinstr.h"
#include "36_scroller.h"
#include "36_vu.h"
#include "36_mixer.h"
#include "36_mixchannel.h"
#include "36_browser.h"
#include "36_project.h"
#include "36_utils.h"
#include "36_draw.h"
#include "36_button.h"
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
            setTouchable(false);
            
            instr = NULL;
        }

        void updPos()
        {
            instr = MInstrPanel->getCurrInstr();

            if (instr && instr->isShown())
            {
                setCoordsAbs(instr->getX1()-2, instr->getY1() - 1, instr->getX2() + 2, instr->getY2() + 1);
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
            //setc(g, 0xe0FF9930);

            if (instr != NULL)
                instr->setMyColor(g, 1.f);
            else
                setc(g, 0xe0FF9930);

            gDrawRect(g,  x1, y1, x2, y2);
        }

        Instrument*     instr;
};

Device36*   devDummy;
extern Sample* prevSample;

InstrPanel::InstrPanel(Mixer* mixer)
{
    fxShowing = false;

    currInstr = instrs.begin();

    currMixChannel = NULL;

    devDummy = new Device36();
    devDummy->setTouchable(false);
    devDummy->addBasicParamSet();

    addObject(btShowFX = new Button36(false), "bt.showbrw");
    addObject(btHideFX = new Button36(false), "bt.hidebrw");
    addObject(allChannelsView = new Button36(true), "bt.channels");

    addObject(mixr = mixer);

    masterVolume = new Parameter("Master", Param_Vol);

    //addObject(masterVolBox = new ParamBox(masterVolume));
    //masterVolBox->setSliderOnly(true);

    addObject(masterVolKnob = new Knob(masterVolume));

    masterVolume->addControl(masterVolKnob);

    addHighlight(instrHighlight = new InstrHighlight());
}

InstrPanel::~InstrPanel()
{
    delete devDummy;
}

Instrument* InstrPanel::addVst(const char* path, Vst2Plugin* otherVst)
{
    Vst2Plugin* vst = loadVst(path, otherVst);

    vst->addBasicParamSet();
    vst->createSelfPattern();

    Instrument* i = NULL;

    if(vst != NULL)
    {
        i = addInstrument(vst);
    }
    else
    {
       // show error box
    }

    return i;
}

Sample* InstrPanel::addSample(const char* path, bool temporaryForPreview)
{
    Sample* smp = loadSample(path);

    smp->addBasicParamSet();
    smp->createSelfPattern();

    if (smp != NULL)
    {
        if (!temporaryForPreview)
        {
            addInstrument(smp);
        }
    }

    return smp;
}

Instrument* InstrPanel::addInstrument(Device36 * dev, Instrument * objAfter)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Device36* device = dev != NULL ? dev : devDummy;

    Instrument* i = new Instrument(device);

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

    if(!(device && device->previewOnly))
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

    return i;
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

    uint8 constantPart = 120;
    uint8 variablePart = 135;

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

    deleteObject(i);

    remapAndRedraw();

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
        if(instr->device->devIdx == index)
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
        if (instr->getDevice() && instr->getDevice()->previewOnly)
            continue;

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
        if (!instr->device->previewOnly && line == trkLine)
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
        MixChannel* mchan = instr->getMixChannel();

        if (instr->getDevice())
        {
            instr->device->generateData(NULL, mchan->inbuff, num_frames, mixbuffframe);
        }
    }

    if (prevSample)
        prevSample->generateData(NULL, MMixer->getMasterChannel()->inbuff, num_frames, mixbuffframe);
}

bool InstrPanel::handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my)
{
    if(!fxShowing || fxShowing && mx > FxPanelMaxWidth)
    {
        Gobj* o1 = NULL;
        Gobj* o2 = NULL;

        dropObj = CheckDropObjectY(objs, "instr", my);

        if (dropObj)
        {
            drag.setDropCoords(dropObj->getX1(), dropObj->getY1(), dropObj->getW(), dropObj->getH(), true);
        }

/*
        dropObj = CheckNeighborObjectsY(objs, "instr", my, (Gobj**)&o1, (Gobj**)&o2);

        if(o2 != NULL)
        {
            drag.dropHighlightHorizontal->setCoordsUn(o2->getX1(), o2->getY1() - 2 - 4, o2->getX2() + 1, o2->getY1() - 2 + 4);
        }
        else if(o1 != NULL)
        {
            drag.dropHighlightHorizontal->setCoordsUn(o1->getX1(), o1->getY2() + 2 - 4, o1->getX2() + 1, o1->getY2() + 2 + 4);
        }
*/

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
    if (dropObj)
    {
        BrwListEntry* ble = dynamic_cast<BrwListEntry*>(obj);

        if (ble)
        {
            // load from browser

            Instrument* i = dynamic_cast<Instrument*>(dropObj);

            setInstrFromNewBrowser(ble, i);
        }
        else
        {
            // move from other instr

            Instrument* iTo = (Instrument*)dropObj;
            Instrument* iFrom = (Instrument*)obj;

            Device36* devTo = iTo->getDevice();
            Device36* devFrom = iFrom->getDevice();

            iTo->setDevice(devFrom);
            iFrom->setDevice(devTo);

            setCurrInstr(iTo);

            //iTo->redraw();
            //iFrom->redraw();

            remapAndRedraw();
        }
    }
    else
    {
        
    }

/*
    if (ble)
    {
        i = loadInstrFromNewBrowser(ble);
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
    }*/

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
        else if(obj == allChannelsView)
        {
            if (!ev.clickDown)
            {
                MixViewSingle = !allChannelsView->isPressed();

                remap();
                redraw();
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

void InstrPanel::addInstrFromNewBrowser(BrwListEntry* ble)
{
    if (getNumInstrs() >= 37)
    {
        MWindow->showAlertBox("Can't load more than 36 instruments");

        return;
    }

    if (ble->getType() == Entry_Wave)
    {
        addSample(ble->getPath().data());
    }
    else if (ble->getType() == Entry_DLL)
    {
        addVst(ble->getPath().data(), NULL);
    }
}

void InstrPanel::setInstrFromNewBrowser(BrwListEntry* ble, Instrument* instr)
{
    Device36* dev = NULL;

    if (ble->getType() == Entry_Wave)
    {
        dev = loadSample(ble->getPath().data());
    }
    else if (ble->getType() == Entry_DLL)
    {
        dev = loadVst(ble->getPath().data(), NULL);
    }

    if (dev)
    {
        dev->addBasicParamSet();

        dev->createSelfPattern();

        instr->deleteDevice();

        instr->setDevice(dev);

        remapAndRedraw();
    }
}

Vst2Plugin* InstrPanel::loadVst(const char* path, Vst2Plugin* otherVst)
{
    Vst2Plugin* vst = NULL;

    std::string vstPath = path != NULL ? path : otherVst->getPath();

    vst = new Vst2Plugin((char*)path);

    if (!vst->isLoaded())
    {
        delete vst;

        return NULL;
    }

    vst->setPath(path);

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
        instr->device->reset();
    }
}

void InstrPanel::remap()
{
    confine();

    //masterVolBox->setCoords1(width - 120, 6, -1, 16);

    masterVolKnob->setCoords1(width - 130, 1, 100, MainLineHeight - 2);

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
        if (i->getDevice()->previewOnly)
        {
            continue;
        }

        if((yoffs + i->getH()) >= 0 && yoffs <= instrListHeight)
        {
            i->setCoords1(width - InstrControlWidth + 1, instrListY + yoffs, InstrControlWidth - 11, InstrHeight);
        }
        else
        {
            i->setVis(false);
        }

        yoffs += InstrHeight + 1;
    }

    confine();

    int btW = 28;

    if (fxShowing)
    {
        btHideFX->setCoords1(width - btW - 1, 0, btW, btW);

        allChannelsView->setCoords1(2, 0, btW, btW);

        confine(0, instrListY, FxPanelMaxWidth-1, height);

        mixr->setCoords1(0, instrListY, FxPanelMaxWidth, instrListHeight);
    }
    else
    {
        btShowFX->setCoords1(width - btW - 1, 0, btW, btW);

        mixr->setVis(false);

        allChannelsView->setVis(false);
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

    instr = *currInstr;

    redraw();

    instrHighlight->updPos();

    if (fxShowing)
    {
        MMixer->remapAndRedraw();
    }
}

void InstrPanel::showFX()
{
    fxShowing = true;

    MObject->setMainX1(FxPanelMaxWidth + InstrControlWidth);

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

