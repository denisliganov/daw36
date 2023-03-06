
#include "36_controls.h"
#include "36_params.h"



Control::Control()
{
    param = NULL;
    dev = NULL;
    parentImage = NULL;
    headerOn = true;

    setHint("");
}

void Control::addParam(Param* p)
{
    if (p != NULL)
    {
        param = p;

        param->addControl(this);
    }

    remap();
}

void Control::removeParam(Param* p)
{
    if (param == p)
    {
        param->removeControl(this);

        param = NULL;
    }

    remap();
}

void Control::setFontId(FontId fId)
{
    fontId = fId;

    textHeight = gGetTextHeight(fontId);

    headerHeight = textHeight + 3;
}

