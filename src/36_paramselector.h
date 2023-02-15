
#pragma once

#include "36_params.h"
#include "36_config.h"


#include <vector>

class ParamSelector : public Param
{
public:

            ParamSelector(std::string name) { prmName = name; }
            void                addOption(std::string opt, bool val) { options.push_back(opt); optVals.push_back(val); }
            std::vector<std::string>&  getOptions()     { return options; }
            void                setValue(int optnum, bool new_val) { optVals[optnum] = new_val; lastChangedNum = optnum; }
            void                toggleValue(int optnum) { optVals[optnum] = !optVals[optnum]; lastChangedNum = optnum; }
            bool                getValue(int optnum)    { return optVals[optnum]; }
            int                 getLastChanged() { return lastChangedNum; }
            int                 getNumOptions()     { return options.size(); }

protected:

            int                 lastChangedNum;

            std::vector<std::string>  options;
            std::vector<bool>         optVals;
};



