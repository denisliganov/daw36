
#include "36_configwin.h"
#include "36_slider.h"
#include "36_params.h"
#include "36_paramswin.h"
#include "36_draw.h"




ParamWin::ParamWin()
{

    setWidthHeight(300, 300);
}

void ParamWin::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void ParamWin::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}



