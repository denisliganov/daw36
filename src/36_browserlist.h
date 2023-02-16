
#pragma once

#include "36_globals.h"
#include "36_scroller.h"

#include <vector>



class BrowserList  : public ListBoxx
{
public:
            BrowserList(std::string name);

protected:
            void    drawSelf(Graphics& g);
};


