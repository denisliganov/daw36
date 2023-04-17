

#include "36_devwin.h"
#include "36_draw.h"
#include "36_knob.h"
#include "36_listbox.h"
#include "36_params.h"
#include "36_parambox.h"
#include "36_device.h"




DevParamObject::DevParamObject(Device36* dev)
{
    device = dev;

    placeControls();
}

void DevParamObject::placeControls()
{
    int boxWidth = 130;
    int boxHeight = 22;

    setObjSpacing(2);
    setBorder(4);

    putStart(border, border);

    std::list<Parameter*>  showParams;

    showParams = device->getParams();

    int numIndexed = showParams.size() - 3;
    int xnum = 1;
    int ynum = numIndexed;

    while (numIndexed > 3 && ynum*(2 + 30) > 2*xnum*(2 + 100))
    {
        xnum ++;
        ynum /= 2;
    }

    int numPut = 0;

    for(Parameter* prm : showParams)
    {
        Gobj* obj = NULL;

        if (prm->getIndex() != -1)
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

            numPut++;

            if (numPut % ynum == 0 && numPut < numIndexed)
            {
                returnUp();
            }
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
        device->setPreset(presetBox->getCurrentName());

        WinObject::redraw();
    }
}

void DevParamObject::handleClose()
{
    device->handleWindowClosed();
}

