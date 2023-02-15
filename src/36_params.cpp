


#include "36_params.h"
#include "36_controls.h"


Param::Param()
{
    index = 0;
    envdirect = true;
    module = NULL;
    ctrlUpdatingFrom = NULL;
}

void Param::addControl(Control* ct)
{
    controls.push_back(ct);

    ct->addParam(this);
}

void Param::removeControl(Control* ctrl)
{
    controls.remove(ctrl);
}

bool Param::getEnvDirect()
{
    return envdirect;
}

void Param::setEnvDirect(bool envdir)
{
    envdirect = envdir;
}



