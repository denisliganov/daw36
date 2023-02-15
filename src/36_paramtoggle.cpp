


#include "36_paramtoggle.h"
#include "36_paramobject.h"




void ParamToggle::toggle()
{
    value = !value; 

    module->handleParamUpdate(this);
}


