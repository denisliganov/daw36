

#include "36_devwin.h"
#include "36_draw.h"
#include "36_listbox.h"
#include "36_params.h"
#include "36_parambox.h"
#include "36_paramnum.h"
#include "36_paramradio.h"
#include "36_paramselector.h"
#include "36_paramtoggle.h"
#include "36_vstinstr.h"
#include "36_vsteff.h"



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
        showParams = vsti->getParams();
    }
    else if (vste)
    {
        showParams = vste->getParams();
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
            ParamBox* box = new ParamBox(prm);

            box->setCoords1(0, 0, boxWidth, boxHeight);
            box->setSliderOnly(false);

            obj = box;
        }
        else if (prmRad)
        {
            obj = new RadioBox(prmRad);
        }
        else if (prmSel)
        {
            obj = new SelectorBox(prmSel);
        }
        else if (prmTg)
        {
            obj = new ToggleBox(prmTg);
        }

        addObject(obj);

        if (obj)
        {
            putBelow(obj, obj->getW(), obj->getH());
        }
    }

    addObject(presetBox = new ListBoxx("Presets"));

    goTop();

    spaceRight();
    spaceRight();

    if (device->getNumPresets() > 0)
    {
        putRight(presetBox, 150, wndH - border);
    }

    presetBox->setList(device->getList());

    finalizePuts();

//    setWidthHeight(x + boxWidth + border + 5, y + border - 2);

    WinObject::setName(String(device->getObjName().data()));
}

void DevParamObject::drawSelf(Graphics& g)
{
    Gobj::fill(g, 0.4f);
}

void DevParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;

    if (obj == presetBox && !ev.clickDown)
    {
        device->setPresetByName(presetBox->getCurrentName());

        redraw();
    }
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

