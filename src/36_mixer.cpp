


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
#include "36_dragndrop.h"
#include "36_text.h"
#include "36_instrpanel.h"
#include "36_instr.h"



bool            MixViewSingle = true;


Mixer::Mixer()
{
    init();
}

Mixer::~Mixer()
{
    ///
}

void Mixer::init()
{
    objId = "mixer";

    addObject(masterChannel = new MixChannel(), "mchan.master");
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

void Mixer::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    ///
}

void Mixer::remap()
{
    if (MixViewSingle)
    {
        for (Instrument* instr : MInstrPanel->getInstrs())
        {
            if (instr != MInstrPanel->getCurrInstr())
            {
                instr->getMixChannel()->setVis(false);
            }
            else
            {
                instr->getMixChannel()->setCoords1(0, 0, width, height);
            }
        }
    }
    else
    {
        int yCh = 0;

        for(Instrument* instr : MInstrPanel->getInstrs())
        {
            if((yCh + InstrHeight > 0) && yCh < getH())
            {
                instr->mixChannel->setCoords1(0, yCh, width, instr->getH());
            }
            else if(instr->mixChannel->isShown())
            {
                instr->mixChannel->setVis(false);
            }

            yCh += InstrHeight;
        }
    }
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
    g.drawVerticalLine(xBound + 1, (float)getY1(), (float)getY2());
*/
}

MixChannel* Mixer::addMixChannel(Instrument * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel;

    if (instr->isMaster())
    {
        mixChannel = (masterChannel);
        mixChannel->instr = instr;
    }
    else
    {
        mixChannel = new MixChannel(instr);
        mixChannel->mchanout = (masterChannel);
        addObject(mixChannel, "");
    }

    ReleaseMutex(MixerMutex);

    return mixChannel;
}

void Mixer::removeMixChannel(Instrument * instr)
{
    WaitForSingleObject(MixerMutex, INFINITE);

    MixChannel* mixChannel = instr->getMixChannel();

    if (mixChannel != masterChannel)
    {
        deleteObject(mixChannel);
    }

    ReleaseMutex(MixerMutex);
}

void Mixer::updateMixingQueue()
{
    for (Instrument* instr : MInstrPanel->getInstrs())
    {
        MixChannel* mc = instr->getMixChannel();
    }
}



