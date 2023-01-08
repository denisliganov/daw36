
#include "36_params.h"
#include "36_effwin.h"
#include "36_draw.h"
#include "36_numbox.h"



EffParamObject::EffParamObject(Eff* eff)
{
    int x = 6;
    int y = 8;

    ParamBox* box = NULL;

    int txs1=0, txs2=0, txs3=0, txs4=0;
    int tx1=0, tx2=0, tx3=0, tx4=0;

    for(Parameter* param : eff->params)
    {
        if (param->type != Param_Bool)
        {

            addObject(box = new ParamBox(param), x, y, 100, 14, "eff.param");

            box->getTextCoords(&tx1,&tx2,&tx3,&tx4);

            if(tx1 > txs1)
                txs1 = tx1;
            if(tx2 > txs2)
                txs2 = tx2;
            if(tx3 > txs3)
                txs3 = tx3;
            if(tx4 > txs4)
                txs4 = tx4;

            y += 16;
        }
    }

    for(Gobj* obj : objs)
    {
        if(obj->getObjId() == "eff.param")
        {
            box = (ParamBox*)obj;
            box->adjustTx2(txs2);

            box->getTextCoords(&tx1,&tx2,&tx3,&tx4);

            if(tx2 > txs2)
                txs2 = tx2;
            if(tx3 > txs3)
                txs3 = tx3;
            if(tx4 > txs4)
                txs4 = tx4;
        }
    }

    for(Gobj* obj : objs)
    {
        if(obj->getObjId() == "eff.param")
        {
            box = (ParamBox*)obj;
            box->adjustTx3(txs3);
            box->getTextCoords(&tx1,&tx2,&tx3,&tx4);

            if(tx3 > txs3)
                txs3 = tx3;
            if(tx4 > txs4)
                txs4 = tx4;
        }
    }

    for(Gobj* obj : objs)
    {
        if(obj->getObjId() == "eff.param")
        {
            box = (ParamBox*)obj;
            box->adjustTx4(txs4);

            if(tx4 > txs4)
                txs4 = tx4;
        }
    }

    int w = 150;

    if (txs4 + 40 > w)
        w = txs4 + 40;

    setWidthHeight(w, y + 6);
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



