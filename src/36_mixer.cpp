


#include "36_mixer.h"
#include "36_mixchannel.h"
#include "36_params.h"
#include "36_vu.h"
#include "36_browser.h"
#include "36_utils.h"
#include "36_scroller.h"
#include "36_menu.h"
#include "36_project.h"
#include "36_draw.h"
#include "36_audio_dev.h"
#include "36_brwentry.h"
#include "36_dragndrop.h"
#include "36_text.h"
#include "36_instrpanel.h"
#include "36_instr.h"



bool            MixViewSingle = false;


Mixer::Mixer()
{
    init();
}

Mixer::~Mixer()
{
    int a = 1;
}

void Mixer::init()
{
    objId = "mixer";

    currentEffect = NULL;

    addObject(masterChannel = new MixChannel(), "mchan.master");

    masterChannel->master = true;
    masterChannel->chanTitle = "MASTER";
}


void Mixer::cleanBuffers(int num_frames)
{
    for(Instrument* instr : MInstrPanel->getInstrs())
    {
        memset(instr->mixChannel->inbuff, 0, sizeof(float)*num_frames*2);
    }

    memset(masterChannel->inbuff, 0, sizeof(float)*num_frames*2);
}

void Mixer::mixAll(int num_frames)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    for(Instrument* instr : MInstrPanel->getInstrs())
    {
        if (instr->mixChannel != masterChannel)
        {
            instr->mixChannel->process(num_frames, masterChannel->inbuff);

            for (int s = 0; s < NUM_SENDS; s++)
            {
                //float val = instr->mixChannel->sends[s].amount->getOutVal();
                //if (val > 0)
                //{
                //    instr->mixChannel->doSend(sendChannel[s]->inbuff, val, num_frames);
                //}
            }
        }
    }

    masterChannel->process(num_frames, NULL);

    ReleaseMutex(MixerMutex);
}

void Mixer::resetAll()
{
    for(Instrument* instr : MInstrPanel->getInstrs())
    {
        instr->mixChannel->reset();
    }

    masterChannel->reset();
}

void Mixer::setCurrentEffect(Eff * eff)
{
    WaitForSingleObject(MixerMutex, INFINITE);
    //std::unique_lock<std::mutex> lock(MixMutex);

    currentEffect = eff;

    redraw();

    ReleaseMutex(MixerMutex);
}

void Mixer::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    ///
}

void Mixer::remap()
{
    if (MixViewSingle == false)
    {
        int yCh = 0;

        for(Instrument* instr : MInstrPanel->getInstrs())
        {
            if (instr->device && instr->device->isPreviewOnly())
            {
                continue;
            }

            if((yCh + InstrHeight > 0) && yCh < getH())
            {
                instr->mixChannel->setCoords1(0, yCh, width, instr->getH());
            }
            else if(instr->mixChannel->isShown())
            {
                instr->mixChannel->setVis(false);
            }

            yCh += InstrHeight + 1;
        }
    }
    else
    {
        MInstrPanel->getCurrInstr()->getMixChannel()->setCoords1(0, 0, width, height);
    }

/*
    int base = 30;
    int chanX = 0 ;
    int mchanHeight = getH() - base - 2;

    masterSectionWidth = (NUM_SENDS + 1)*(MixChanWidth + 1) + 50;

    confine(0, 2, width - masterSectionWidth, 2 + mchanHeight);

    // Instrument mixchannels, ordered according to the instruments

    for(Instrument* instr : MInstrPanel->instrs)
    {
        if(!instr->previewOnly)
        {
            instr->mixChannel->setCoords1(chanX - xOffset, 2, MixChanWidth, mchanHeight);

            chanX += MixChanWidth + 1;
        }
    }

    confine();     // reset confinement

    scroller->setCoords1(0, getH() - base + 1, width - masterSectionWidth, 30);

    scroller->updateLimits((float)chanX, (float)(width - masterSectionWidth), (float)xOffset);

    chanX = width - masterSectionWidth + 10;

    for(int sc = 0; sc < NUM_SENDS; sc++)
    {
        sendChannel[sc]->setCoords1(chanX, 2, MixChanWidth, getH() - 2);

        chanX += MixChanWidth + 1;
    }

    chanX += 10;

    masterChannel->setCoords1(chanX, 2, MixChanWidth, getH() - 2);

    chanX += MixChanWidth + 30;
    */
}

void Mixer::drawSelf(Graphics& g)
{
    fill(g, .1f);
/*
    gSetMonoColor(g, 0.35f);
    gLineHorizontal(g, y1, x1, x2);

    gSetMonoColor(g, 0.29f);
    gLineHorizontal(g, y1 + 1, x1, x2);

    gSetMonoColor(g, 0.25f);

    gFillRect(g, getX1(), getY1() + 2, getX2(), getY2());

    int xBound = getX1() + getW() - MixChanWidth - 35;

    g.drawVerticalLine(xBound + 1, (float)getY1(), (float)getY2());*/
}

void Mixer::updateChannelIndexes()
{
    int idx = 0;

    for(Instrument* instr : MInstrPanel->getInstrs())
    {
        instr->mixChannel->setIndex(idx++);
    }

    masterChannel->setIndex(idx++);
}

MixChannel* Mixer::addMixChannel(Instrument * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel = new MixChannel(instr);

    mixChannel->mchanout = (MMixer->masterChannel);

    addObject(mixChannel, "");

    ReleaseMutex(MixerMutex);

    return mixChannel;
}

