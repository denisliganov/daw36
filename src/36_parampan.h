
#pragma once

#include "36_globals.h"
#include "36_paramnum.h"
#include "36_config.h"



class ParamPan : public Parameter
{
public:
            ParamPan(std::string param_name) : Parameter(param_name, -1.f, 1.f, 0.f, Units_Percent) {}
            float               adjustForEditor(float val);
            float               getEditorValue();

protected:
            std::string         calcValStr(float uv);
};


