
#include "36_controls.h"
#include "36_params.h"



Control::Control()
{
    param = NULL;
    dev = NULL;
    parentImage = NULL;
    headerOn = true;
    bgSaved = false;

    setHint("");
}

void Control::setParam(Parameter* p)
{
    if (p != NULL)
    {
        param = p;

        param->addControl(this);
    }

    updPosition();

    //remap();
    //redraw();
}

void Control::removeParam(Parameter* p)
{
    if (param == p)
    {
        param->removeControl(this);

        param = NULL;
    }

    //remap();
}

void Control::setFontId(FontId fId)
{
    fontId = fId;

    textHeight = gGetTextHeight(fontId);

    headerHeight = textHeight + 3;
}

void Control::handleMouseEnter(InputEvent& ev)
{
    //param->getModule();
}

void Control::handleMouseLeave(InputEvent& ev)
{
    //param->getModule();
}


