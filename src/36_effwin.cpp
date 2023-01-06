
#include "36_params.h"
#include "36_effwin.h"
#include "36_draw.h"
#include "36_numbox.h"



EffParamObject::EffParamObject(Eff* eff)
{
    int x = 0;
    int y = 4;
    ParamBox* box = NULL;

    for(Parameter* param : eff->params)
    {
        addObject(box = new ParamBox(param), x, y, 100, 14);
        y += 16;
    }

    setWidthHeight(140, y);
}

void EffParamObject::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void EffParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}



