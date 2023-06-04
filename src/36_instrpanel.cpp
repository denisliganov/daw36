


#include "rosic/rosic.h"

#include "36_browserlist.h"
#include "36_globals.h"
#include "36_instrpanel.h"
#include "36_instr.h"
#include "36_vst.h"
#include "36_sampleinstr.h"
#include "36_scroller.h"
#include "36_vu.h"
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



bool            MixViewSingle = true;




//// Libsndfile library
SNDFILE*                    sf_open(const char *path, int mode, SF_INFO *sfinfo);
sf_count_t                  sf_readf_float(SNDFILE *sndfile, float *ptr, sf_count_t frames);


/*
class DropHighlight : public Gobj
{
public:

    DropHighlight()
    {
        setTouchable(false);
    }

    void    drawSelf(Graphics& g)
    {
        if (1)
        {


            setc(g, 1.f,.3f);
            fillx(g,  0, 0, width, height);
        }
        else
        {
            //fill(g, 1.f);

            uint8 a = 128;

            Rect36 drwRect = { (float)(x1), (float)y1, (float)width, (float)height };

            bool vert = drwRect.w < drwRect.h;

            int count = vert ? width / 2 : height / 2;

            for (int c = 0; c < count; c++)
            {
                float m = (float)c / count;

                m *= m;

                gSetColor(g, 255, 153, 48, uint8(m * 255));

                //gDrawRect(g, x1, y1, width, height);
                //gFillRectWH(g, drwRect.x, drwRect.y, drwRect.w, drwRect.h);

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
    }
};
*/

class DropHighlight : public Gobj
{
public:

    DropHighlight()
    {
        setTouchable(false);
    }

    void    drawSelf(Graphics& g)
    {
        //gSetColor(g, 255, 200, 48, 255);
        //fillx(g, 0, 0, width, height);
        //return;
        
        bool vert = width < height;
        int count = vert ? width / 2 : height / 2;
        uint8 a = 255;
        int y = 0;
        for (int c = 0; c < count; c++)
        {
            gSetColor(g, 255, 200, 48, a);

            if (vert)
            {
                fillx(g, width / 2 + y, 0, 1, height);
                fillx(g, width / 2 - y, 0, 1, height);
            }
            else
            {
                fillx(g, 0, height / 2 + y, width, 1);
                fillx(g, 0, height / 2 - y, width, 1);

            }

            y++;

            a /= 2;
        }
    }
};



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
                setCoordsAbs(instr->getX1(), instr->getY1(), instr->getX2(), instr->getY2());
                //setCoords2(0, instr->getY1() - 1, instr->getX2() + 5, instr->getY2() + 1);
            }
            else
            {
                setVis(false);
            }
        }

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
                setc(g, 1.f,.32f);

            gFillRect(g,  x1, y1, x2, y2);
        }

        Instr*     instr;
};

Device36*       devDummy;
extern Sample*  prevSample;

InstrPanel::InstrPanel()
{
    fxShowing = false;

    curr = NULL;
    dropObj = NULL;

    devDummy = new Device36();
    devDummy->setTouchable(false);
    devDummy->addBasicParamSet();

    addObject(masterChannel = new MixChannel(), "mchan.master");

    addObject(btShowFX = new Button36(false), "bt.showbrw");
    addObject(btHideFX = new Button36(false), "bt.hidebrw");
    addObject(allChannelsView = new Button36(true), "bt.channels");

    //addObject(masterVolKnob = new Knob(NULL));
    //masterVolKnob->setHasText(true, true);

    addHighlight(instrHighlight = new InstrHighlight());
    addHighlight(dropHighlight = new DropHighlight());

    //showFX();
}

InstrPanel::~InstrPanel()
{
    //delete devDummy;
}


void InstrPanel::init()
{
    objId = "mixer";
}

void InstrPanel::cleanBuffers(int num_frames)
{
    for(Instr* instr : instrs)
    {
        memset(instr->getMixChannel()->tempBuff, 0, sizeof(float)*num_frames*2);
    }
}

void InstrPanel::mixAll(int num_frames)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    for(Instr* instr : instrs)
    {
        instr->getMixChannel()->prepareForMixing();
    }

    bool incomplete;

    do
    {
        incomplete = false;

        for(Instr* instr : instrs)
        {
            MixChannel* mc = instr->getMixChannel();

            if (mc->getMixCounter() > 0)
            {
                incomplete = true;
            }
            else if (!mc->isProcessed())
            {
                mc->processChannel(num_frames);
            }
        }
    }while (incomplete);

    ReleaseMutex(MixerMutex);
}

void InstrPanel::resetAll()
{
    for(Instr* instr : instrs)
    {
        instr->getMixChannel()->reset();
        instr->device->reset();
    }
}

/*
void Mixer::remap()
{
    if (MixViewSingle)
    {
        for (Instr* instr : MInstrPanel->getInstrs())
        {
            if (instr == MInstrPanel->getCurrInstr())
            {
                instr->getMixChannel()->setCoords1(0, 0, width, height);
            }
            else
            {
                if (instr->getMixChannel()->isShown())
                    instr->getMixChannel()->setVis(false);
            }
        }
    }
    else
    {
        int yCh = 0;

        for(Instr* instr : MInstrPanel->getInstrs())
        {
            if((yCh + InstrHeight > 0) && yCh < getH())
            {
                instr->getMixChannel()->setCoords1(0, yCh, width, instr->getH());
            }
            else if(instr->getMixChannel()->isShown())
            {
                instr->getMixChannel()->setVis(false);
            }

            yCh += InstrHeight;
        }
    }
}
*/

MixChannel* InstrPanel::addMixChannel(Instr * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel;

    if (instr->isMaster())
    {
        masterChannel->setInstrument(instr);

        mixChannel = masterChannel;
    }
    else
    {
        addObject(mixChannel = new MixChannel(instr), "");

        mixChannel->setOutChannel(masterChannel);
    }

    ReleaseMutex(MixerMutex);

    return mixChannel;
}

void InstrPanel::removeMixChannel(Instr * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel = instr->getMixChannel();

    if (mixChannel != masterChannel)
    {
        deleteObject(mixChannel);
    }

    ReleaseMutex(MixerMutex);
}




Vst2Plugin* InstrPanel::addVst(const char* path, Vst2Plugin* otherVst)
{
    Vst2Plugin* vst = loadVst(path, otherVst);

    vst->addBasicParamSet();
    vst->createSelfPattern();

    addInstrument(vst);

    return vst;
}

Sample* InstrPanel::addSample(const char* path)
{
    Sample* smp = loadSample(path);

    smp->addBasicParamSet();
    smp->createSelfPattern();

    addInstrument(smp);

    return smp;
}

Instr* InstrPanel::addInstrument(Device36 * dev, bool master)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Device36* device = dev != NULL ? dev : devDummy;

    Instr* i = new Instr(device);

    auto it = instrs.end();

    if (instrs.size() > 0)
    {
        Instr* last = instrs.back();

        if (last != NULL && last->isMaster())
        {
            it--;
        }
    }

    instrs.insert(it, i);

    ReIndexInstruments();

    if (master)
    {
        i->master = true;

        //masterVolKnob->setParam(i->getMixChannel()->vol);
    }

    i->addMixChannel();

    addObject(i, "instr");

    // Propagate sends

    for (Instr* instr : instrs)
    {
        if (!master)
        {
            i->getMixChannel()->addSendToChannel(instr->getMixChannel());
        }

        if (!instr->isMaster() && instr != i)
        {
            instr->getMixChannel()->addSendToChannel(i->getMixChannel());
        }
    }

    remapAndRedraw();

    MProject.setChange();

    MEdit->remapAndRedraw();

    setCurrInstr(i);

    colorizeInstruments();

    ReleaseMutex(AudioMutex);

    return i;
}

void InstrPanel::cloneInstrument(Instr* i)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    Instr* ni = i->clone();

    // Place right after current instrument
    //instrs.remove(ni);
    //currInstr++;
    //instrs.insert(currInstr, ni);
    //currInstr--;
    //ReIndexInstruments();

    // And set it current
    // setCurrInstr(i);

    // colorizeInstruments();

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

    uint8 constantPart = 100;
    uint8 variablePart = 155;

    uint8 r = constantPart;
    uint8 g = constantPart;
    uint8 b = constantPart;

    uint8 alpha = 255;
    int idx = 0;

    float   hueOffset = 0.6f;
    float   hueSpan = .9f;

    for(Instr* instr : instrs)
    {
        float angle = periodAngle*(idx%num);

        r = uint8(((sin(angle))*variablePart) + constantPart);
        b = uint8((sin(angle + PI/2)*variablePart) + constantPart);

        float hue = float(idx*hueSpan)/(float(num)) + hueOffset;

        while (hue > 1)
        {
            hue -= 1;
        }

        instr->defineHueColor(hue, 0.5f);

        idx++;
    }

    updateWaves();

    MGrid->redraw(false, false);
}

void InstrPanel::deleteInstrument(Instr* i)
{
    WaitForSingleObject(AudioMutex, INFINITE);
    WaitForSingleObject(MixerMutex, INFINITE);

    instrs.erase(instrs.begin() + i->getIndex());

    if (i == curr)
    {
        int newIdx = i->getIndex() - 1;

        if (newIdx < 0) 
            newIdx = 0;

        setCurrInstr(instrs[newIdx]);
    }

    for (Instr* ins : instrs)
    {
        //if (ins == i || ins->isMaster()) 
        //    continue;

        ins->getMixChannel()->delSend(i->getMixChannel());
    }

    ReIndexInstruments();

    colorizeInstruments();

    deleteObject(i);

    remapAndRedraw();

    MGrid->redraw(true, true);

    ReleaseMutex(MixerMutex);
    ReleaseMutex(AudioMutex);
}

void InstrPanel::drawSelf(Graphics& g)
{
    fill(g, 0.1f);
    setc(g, .25f);
    fillx(g, 0, 0, width, MainLineHeight);
}

Instr* InstrPanel::getInstrByIndex(int index)
{
    for(Instr* instr : instrs)
    {
        if(instr->getIndex() == index)
        {
            return instr;
        }
    }

    return NULL;
}

Instr* InstrPanel::getCurrInstr()
{
    return curr;
}

Instr* InstrPanel::getInstrByAlias(std::string alstr)
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

    for(Instr* instr : instrs)
    {
        num++;
    }

    return num;
}

Instr* InstrPanel::getInstrFromLine(int trkLine)
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

    for(Instr* instr : instrs)
    {
        if (line == trkLine)
        {
            return instr;
        }

        line++;
    }

    return NULL;
}


void InstrPanel::generateAll(long num_frames, long mixbuffframe)
{
    for(Instr* instr : instrs)
    {
        MixChannel* mchan = instr->getMixChannel();

        if (instr->getDevice())
        {
            instr->device->generateData(NULL, mchan->tempBuff, num_frames, mixbuffframe);
        }
    }

    if (prevSample)
    {
        prevSample->generateData(NULL, MInstrPanel->getMasterChannel()->tempBuff, num_frames, mixbuffframe);
    }
}

void InstrPanel::handleObjDrag(bool reset, Gobj * obj,int mx,int my)
{
    if (reset)
    {
        dropHighlight->setVis(false);
        dropObj = NULL;
        return;
    }

    //if(!fxShowing || fxShowing && mx > FxPanelMaxWidth)
    {
        Gobj* uper = NULL;
        Gobj* lower = NULL;

        dropObj = CheckNeighborObjectsY(objs, "instr", my, (Gobj**)&uper, (Gobj**)&lower);

        //dropHighlight->setVis(true);

        Instr* i = (Instr*)uper;

        if (i && i->isMaster())
        {
            dropObj = NULL;
        }
        else
        {
            if (uper != NULL)
            {
                dropHighlight->setCoords1(uper->getX(), uper->getY() + uper->getH() - 3, InstrControlWidth - 11, 8);
                //dropHighlight->setCoords1(uper->getX(), uper->getY(), InstrControlWidth - 11, uper->getH());

                 //dropHighlight->setCoords1(0, 0, 100, 100);
            }
            else
            {
                dropHighlight->setCoords1(getX() + FxPanelMaxWidth, MainLineHeight - 4, InstrControlWidth - 11, 8);
            }
        }

 /*
        Gobj* o1 = NULL;
        Gobj* o2 = NULL;

        dropObj = CheckDropObjectY(objs, "instr", my);

        if (dropObj)
        {
            dropHighlight->setCoords1(dropObj->getX(), dropObj->getY(), dropObj->getW(), dropObj->getH());
        }
 */
    }
}
void InstrPanel::placeBefore(Instr* i, Instr* before)
{
    WaitForSingleObject(AudioMutex, INFINITE);

    auto it = instrs.end();

    it--;

    for (; (*it) != i; it--);

    Instr* instr = *it;

    instrs.erase(it);

    if (before == NULL)      // Insert to end
    {
        it = instrs.end();
    }
    else
    {
        it = instrs.begin();

        for (; it != instrs.end() && *it != before; it++);
    }

    instrs.insert(it, instr);

    //it--;

    curr = instr;

    ReleaseMutex(AudioMutex);

    //adjustOffset();
    ReIndexInstruments();
    colorizeInstruments();
}

void InstrPanel::handleObjDrop(Gobj * obj, int mx, int my, unsigned int flags)
{
    dropHighlight->setVis(false);

    if (dropObj == NULL)
    {
        return;
    }

    BrwListEntry* ble = dynamic_cast<BrwListEntry*>(obj);
    Instr* i = NULL;

    if (ble)
    {
        addInstrFromNewBrowser(ble);
    } 

    {
        i = getCurrInstr();

        if (i == dropObj)
        {
            i = NULL;
        }
    }


    if (i)
    {
        placeBefore(i, (Instr*)dropObj);

        dropObj = NULL;

        ReIndexInstruments();

        MGrid->redraw(true, true);

        remapAndRedraw();
    }

    /*
    dropHighlight->setVis(false);

    Instr* iTo = dynamic_cast<Instr*>(dropObj);

    if (iTo && !iTo->isMaster())
    {
        BrwListEntry* ble = dynamic_cast<BrwListEntry*>(obj);
        Instr* iFrom = dynamic_cast<Instr*>(obj);

        if (ble)
        {
            // load from browser

            setInstrFromNewBrowser(ble, iTo);
        }
        else if (iFrom)
        {
            // move from other instr

            if (flags & kbd_shift)
            {
//                i = getCurrInstr()->clone();
            }

            Device36* devTo = iTo->getDevice();
            Device36* devFrom = iFrom->getDevice();

            iTo->setDevice(devFrom);
            iFrom->setDevice(devTo);

            setCurrInstr(iTo);

            //iTo->redraw();
            //iFrom->redraw();

            remapAndRedraw();
        }

        dropObj = NULL;

        ReIndexInstruments();

        MGrid->redraw(true, true);
    }
    */
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

void InstrPanel::handleMouseLeave(InputEvent& ev)
{
    dropHighlight->setVis(false);
}

void InstrPanel::hideFX()
{
    MObject->setMainX1(InstrControlWidth);

    fxShowing = false;

    remapAndRedraw();
}

void InstrPanel::ReIndexInstruments()
{
    int idx = 0;

    for(Instr* instr : instrs)
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

void InstrPanel::setInstrFromNewBrowser(BrwListEntry* ble, Instr* instr)
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


void InstrPanel::remap()
{
    confine();

    //masterVolKnob->setCoords1(width - 130, 1, 100, MainLineHeight - 2);

    int instrListY = MainLineHeight + 1;
    int instrListHeight = height - (instrListY + BottomPadHeight);

    confine(0, instrListY, width, instrListY + instrListHeight - 1);

    int num = getNumInstrs();

    int ih = 44;

    while (num*(ih) > instrListHeight)
    {
        ih--;
    }

    if (ih == 0) ih = 2;

    InstrHeight = ih;

    int yoffs =0;

    for (Instr* i : instrs)
    {
        if((yoffs + i->getH()) >= 0 && yoffs <= instrListHeight)
        {
            i->setCoords1(width - InstrControlWidth + 1, instrListY + yoffs, InstrControlWidth - 11, InstrHeight);
        }
        else
        {
            i->setVis(false);
        }

        yoffs += InstrHeight;
    }

    confine();

    int btW = 28;

    if (fxShowing)
    {
        //btHideFX->setCoords1(width - btW - 1, 0, btW, btW);
        //allChannelsView->setCoords1(2, 0, btW, btW);

        confine(0, instrListY, FxPanelMaxWidth-1, height);

        if (curr)
        {
            //curr->getMixChannel()->setCoords1(0, instrListY, FxPanelMaxWidth, instrListHeight);
            curr->getMixChannel()->setCoords1(0, instrListY, FxPanelMaxWidth, instrListHeight);
        }
    }
    else
    {
        //btShowFX->setCoords1(width - btW - 1, 0, btW, btW);
        //allChannelsView->setVis(false);

        if (curr)
        {
            curr->getMixChannel()->setVis(false);
        }
    }

    //confine(0, instrListY-1, width, instrListY + instrListHeight - 1);
    //instrHighlight->updPos();

    if (MGrid)
        MGrid->setLineHeight(InstrHeight);

    confine();
}

void InstrPanel::setSampleRate(float sampleRate)
{
    for(Instr* instr : instrs)
    {
        instr->setSampleRate(sampleRate);
    }
}

void InstrPanel::setBufferSize(unsigned bufferSize)
{
    for(Instr* instr : instrs)
    {
        instr->setBufferSize(bufferSize);
    }
}

void InstrPanel::setCurrInstr(Instr* instr)
{
    if (curr)
    {
        curr->getMixChannel()->setEnable(false);
    }

    curr = instr;

    if (curr)
    {
        curr->getMixChannel()->setEnable(true);
    }

    remapAndRedraw();

    //instrHighlight->updPos();
}

void InstrPanel::setCurrInstr(int instr_index)
{
    if (instrs[instr_index] != NULL && instrs[instr_index] != curr)
    {
        setCurrInstr(instrs[instr_index]);
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
    for(Instr* instr : instrs)
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


