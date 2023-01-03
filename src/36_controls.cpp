



#include "36_controls.h"




Control::Control()
{
    param = NULL;
    dev = NULL;
    parentImage = NULL;

    setHint("");
}

void Control::addParam(Parameter* p)
{
    param = p;
    dev = NULL;

    updPosition();
}

