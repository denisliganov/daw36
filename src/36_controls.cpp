
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
    if (param == NULL)
    {
        param = p;

        param->addControl(this);
    }

    dev = NULL;

    updPosition();
}

void Control::setFontId(FontId fId)
{
    fontId = fId;

    textHeight = gGetTextHeight(fontId);

    headerHeight = textHeight + 3;
}
