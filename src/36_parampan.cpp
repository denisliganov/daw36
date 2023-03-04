

#include "36_parampan.h"
#include "36_utils.h"




std::string ParamPan::calcValStr(float val)
{
    char str[100] = {};

    float absVal = (val);

    int pval = abs(int(absVal*100));

    std::string valStr = String(pval);

    if(value < 0)
    {
        valStr = "<" + valStr;
    }
    else if(value > 0)
    {
        valStr = valStr + ">";
    }

    return valStr;
}

float ParamPan::adjustForEditor(float val)
{
    return range - (val - offset) + offset;
}

float ParamPan::getEditorValue()
{
    return 1.f - getValueNormalized();
}


