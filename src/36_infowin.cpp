
#include "36_infowin.h"
#include "36_draw.h"




InfoWin::InfoWin()
{
    setWidthHeight(300, 300);
}

void InfoWin::drawSelf(Graphics& g)
{
    fill(g, 0.4f);
}

void InfoWin::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    //
}



