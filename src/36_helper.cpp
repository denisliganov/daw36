

#include "36_helper.h"
#include "36_draw.h"





HelperPanel::HelperPanel()
{
    
}

void HelperPanel::drawself(Graphics & g)
{
    gPanelRect(g, x1, y1, x2, y2);
}


