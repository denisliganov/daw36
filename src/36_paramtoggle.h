
#pragma once

#include "36_params.h"


class ParamToggle : public Param
{
public:

            ParamToggle(std::string name, bool def_val) {prmName = name; value = def_val;}

            bool                getValue()  { return value; }
            void                toggle();

protected:

            bool                value;
};



