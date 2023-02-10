
#include "36_params.h"
#include "36_devwin.h"
#include "36_draw.h"
#include "36_parambox.h"
#include "36_vstinstr.h"
#include "36_vsteff.h"



DevParamObject::DevParamObject(Device36* dev)
{
    device = dev;

    initAll();
}

void DevParamObject::initAll()
{
    int border = 12;
    int boxWidth = 130;
    int boxHeight = 22;

    int x = 10;
    int y = border;

    int txs1=0, txs2=0, txs3=0;
    int tx1=0, tx2=0, tx3=0;

    std::list<Param*>  showParams;

    VstInstr*  vsti = dynamic_cast<VstInstr*>(device);
    VstEffect* vste = dynamic_cast<VstEffect*>(device);

    if (vsti)
    {
        showParams = vsti->vst2->getParams();
    }
    else if (vste)
    {
        showParams = vste->vst2->getParams();
    }
    else
    {
        showParams = device->getParams();
    }

    for(Param* param : showParams)
    {
        Parameter*      prm = dynamic_cast<Parameter*>(param);
        ParamRadio*     prmRad = dynamic_cast<ParamRadio*>(param);
        ParamSelector*  prmSel = dynamic_cast<ParamSelector*>(param);
        ParamToggle*    prmTg = dynamic_cast<ParamToggle*>(param);

        if (prm)
        {
            ParamBox* box = NULL;
            Gobj::addObject(box = new ParamBox(prm), x, y, boxWidth, boxHeight);

            //box->setSliderOnly(false);

            y += boxHeight + 3;
        }
        else if (prmRad)
        {
            RadioBox* box = NULL;
            Gobj::addObject(box = new RadioBox(prmRad), x + 5, y, boxWidth - 5, box->getH());

            y += box->getH() + 3;
        }
        else if (prmSel)
        {
            SelectorBox* box = NULL;
            Gobj::addObject(box = new SelectorBox(prmSel), x + 5, y, boxWidth - 5, box->getH());

            y += box->getH() + 3;
        }
        else if (prmTg)
        {
            ToggleBox* box = NULL;
            Gobj::addObject(box = new ToggleBox(prmTg), x + 5, y, boxWidth - 5, box->getH());

            y += box->getH() + 3;
        }
    }

    setWidthHeight(x + boxWidth + border + 5, y + border - 2);

    WinObject::setName(String(device->getObjName().data()));
}

void DevParamObject::drawSelf(Graphics& g)
{
    Gobj::fill(g, 0.35f);
}

void DevParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

