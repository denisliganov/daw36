
#include "36_params.h"
#include "36_effwin.h"
#include "36_draw.h"
#include "36_numbox.h"



EffParamObject::EffParamObject(Eff* eff)
{
    int border = 20;
    int boxWidth = 150;
    int boxHeight = border;

    int x = border;
    int y = border;

    ParamBox* box = NULL;

    int txs1=0, txs2=0, txs3=0;
    int tx1=0, tx2=0, tx3=0;

    for(Parameter* param : eff->params)
    {
        if (param->type != Param_Bool)
        {
            addObject(box = new ParamBox(param), x, y, boxWidth, boxHeight, "eff.param");
            y += boxHeight + 2;
        }
    }

    setWidthHeight(x + boxWidth + border + 5, y + border - 2);

    WinObject::setName(String(eff->getObjName().data()));
}

void EffParamObject::drawSelf(Graphics& g)
{
    fillWithMonoColor(0.35f);
}

void EffParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}



