



#include "36_controls.h"




Control::Control()
{
    param = NULL;
    dev = NULL;
    parentImage = NULL;

    setHint("");
}

void Control::addParam(Param* p)
{
    param = p;
    dev = NULL;

    updPosition();
}

