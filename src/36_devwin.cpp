

#include "36_devwin.h"
#include "36_draw.h"
#include "36_knob.h"
#include "36_listbox.h"
#include "36_params.h"
#include "36_parambox.h"
#include "36_vstinstr.h"



DevParamObject::DevParamObject(Device36* dev)
{
    device = dev;

    placeControls();
}

void DevParamObject::placeControls()
{
    //int border = 12;
    int boxWidth = 130;
    int boxHeight = 22;

    //int x = 10;
    //int y = border;

    setObjSpacing(2);
    setBorder(4);

    putStart(border, border);

    std::list<Parameter*>  showParams;

    VstInstr*  vsti = dynamic_cast<VstInstr*>(device);

    if (vsti)
    {
        showParams = vsti->getParams();
    }
    else
    {
        showParams = device->getParams();
    }

    for(Parameter* prm : showParams)
    {
        Gobj* obj = NULL;

        if (prm)
        {
            /*
            else if (prm->getType() == Param_Vol)
            {
                obj = new Knob(prm);
                obj->setWH(15, 15);
            }
            else if (prm->getType() == Param_Pan)
            {
                obj = new Knob(prm);
                obj->setWH(15, 15);
            }*/
            if (prm->getType() == Param_Radio)
            {
                obj = new RadioBox(prm);

                obj->setWH(100, obj->getH());
            }
            else if (prm->getType() == Param_Selector)
            {
                obj = new SelectorBox(prm);

                obj->setWH(100, obj->getH());
            }
            else if (prm->getType() == Param_Toggle)
            {
                obj = new ToggleBox(prm);

                obj->setWH(100, obj->getH());
            }
            else
            {
/*
                ParamBox* box = new ParamBox(prm);

                box->setCoords1(0, 0, boxWidth, boxHeight);
                box->setSliderOnly(false);

                obj = box;
*/
                obj = new Knob(prm);

                obj->setWH(100, 30);
            }

        }

        if (obj)
        {
            WinObject::addObject(obj);

            putBelow(obj, obj->getW(), obj->getH());
        }
    }

    WinObject::addObject(presetBox = new ListBoxx("Presets"));

    returnUp();

    spaceRight();
    spaceRight();

    if (device->getNumPresets() > 0)
    {
        putRight(presetBox, 150, wndH - border);
    }

    presetBox->setList(device->getPresetList());

    finalizePuts();

    setWidthHeight(wndW + border, wndH + border);

    WinObject::setName(String(device->getObjName().data()));
}

void DevParamObject::drawSelf(Graphics& g)
{
    WinObject::fill(g, 0.3f);
}

void DevParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    if (obj == presetBox && !ev.clickDown)
    {
        device->setPresetByName(presetBox->getCurrentName());

        WinObject::redraw();
    }
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

