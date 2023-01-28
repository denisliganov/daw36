
#include "36_params.h"
#include "36_effwin.h"
#include "36_draw.h"
#include "36_numbox.h"
#include "36_vstinstr.h"
#include "36_vsteff.h"



DevParamObject::DevParamObject(Device36* dev)
{
    device = dev;

    initAll();
}

void DevParamObject::initAll()
{
    int border = 16;
    int boxWidth = 130;
    int boxHeight = border;

    int x = 4;
    int y = border;

    ParamBox* box = NULL;

    int txs1=0, txs2=0, txs3=0;
    int tx1=0, tx2=0, tx3=0;

    std::list<Parameter*>  showParams;

    VstInstr*  vsti = dynamic_cast<VstInstr*>(device);
    VstEffect* vste = dynamic_cast<VstEffect*>(device);

    if (vsti)
    {
        showParams = vsti->vst2->params;
    }
    else if (vste)
    {
        showParams = vste->vst2->params;
    }
    else
    {
        showParams = device->params;
    }

    for(Parameter* param : showParams)
    {
        if (param->type != Param_Bool)
        {
            addObject(box = new ParamBox(param), x, y, boxWidth, boxHeight, "eff.param");
            y += boxHeight + 2;
        }
    }

    setWidthHeight(x + boxWidth + border + 5, y + border - 2);

    WinObject::setName(String(device->getObjName().data()));
}

void DevParamObject::drawSelf(Graphics& g)
{
    fill(g, 0.35f);
}

void DevParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

