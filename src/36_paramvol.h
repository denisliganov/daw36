
#pragma once

#include "36_globals.h"
#include "36_paramnum.h"
#include "36_config.h"



class ParamVol : public Parameter
{
public:
            ParamVol(std::string param_name) : Parameter(param_name, 0.f, DAW_VOL_RANGE, 1.f, Units_dB) {}

protected:
            std::string         calcValStr(float uv);
            float               calcOutputValue(float val);
};


