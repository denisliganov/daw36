
#include "36_params.h"
#include "36_devwin.h"
#include "36_draw.h"
#include "36_parambox.h"
#include "36_vstinstr.h"
#include "36_vsteff.h"
#include "36_listbox.h"



DevParamObject::DevParamObject(Device36* dev)
{
    device = dev;

    initAll();
}

void DevParamObject::initAll()
{
    //int border = 12;
    int boxWidth = 130;
    int boxHeight = 22;

    //int x = 10;
    //int y = border;

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

        Gobj* obj = NULL;

        if (prm)
        {
            ParamBox* box = box = new ParamBox(prm);

            box->setCoords1(0, 0, boxWidth, boxHeight);
            box->setSliderOnly(false);

            obj = box;
        }
        else if (prmRad)
        {
            RadioBox* box = new RadioBox(prmRad);
            obj = box;
        }
        else if (prmSel)
        {
            SelectorBox* box = new SelectorBox(prmSel);
            obj = box;
        }
        else if (prmTg)
        {
            ToggleBox* box = new ToggleBox(prmTg);
            obj = box;
        }

        if (obj)
        {
            putBelow(obj, obj->getW(), obj->getH());
        }
    }

    presetBox = new ListBoxx("Presets");

    goTop();

    spaceRight();
    spaceRight();

    putRight(presetBox, 150, wndH - border);

    presetBox->setList(device->getList());

    finalizePuts();

//    setWidthHeight(x + boxWidth + border + 5, y + border - 2);

    WinObject::setName(String(device->getObjName().data()));
}

void DevParamObject::drawSelf(Graphics& g)
{
    Gobj::fill(g, 0.36f);
}

void DevParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;

    if (obj == presetBox && !ev.clickDown)
    {
        device->setPresetByName(presetBox->getCurrentName());
    }
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

