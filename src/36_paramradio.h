
#pragma once

#include "36_params.h"


class ParamRadio : public Param
{
public:

            ParamRadio(std::string name) { prmName = name; currentOption = 0; }

            void                addOption(std::string opt) { options.push_back(opt); }
            int                 getNumOptions()     { return options.size(); }
       std::list<std::string>&  getOptions()     { return options; }
            int                 getCurrent()     { return currentOption; }
            void                setCurrent(int curr) { currentOption = curr;}

protected:

            int                 currentOption;

            std::list<std::string>  options;
};


