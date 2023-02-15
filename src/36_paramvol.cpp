

#include "rosic/rosic.h"


#include "36_paramvol.h"
#include "36_utils.h"

#include "stdio.h"


std::string ParamVol::calcValStr(float val)
{
    std::string valStr;

    if(outVal == 0)
    {
        valStr = "INF";
    }
    else
    {
        char str[100] = {};

        double pval = (amp2dB(val));

        sprintf(str, "%.2f", pval);

        valStr = str;
    }

    return valStr;
}


float ParamVol::calcOutputValue(float val)
{
    return GetVolOutput(val);
}


