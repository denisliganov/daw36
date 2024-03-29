
#include "36_paramobject.h"
#include "36_params.h"
#include "36_controls.h"
#include "36_events_triggers.h"
#include "36_knob.h"




ParamObject::ParamObject()
{
    border = 10;

    putStart(border, border);

    groupSpacing = 8;
    objSpacing = 4;

    wndW = border;
    wndH = border;

    paramLocked = false;

    invokeButton = NULL;
}

ParamObject::~ParamObject()
{
    while (params.size() > 0)
    {
        Parameter* p = params.front();
        params.remove(p);
        delete p;
    }
}

void ParamObject::addParam(Parameter* param)
{
    param->setModule(this);
    param->setEnvDirect(false);

    params.push_back(param);

    handleParamUpdate(param);
}

Parameter* ParamObject::getParamByName(char *param_name)
{
    for(Parameter* param : params)
    {
        if(_stricmp(param->getName().data(), param_name) == 0)
        {
            return param;
        }
    }

    return NULL;
}

Parameter* ParamObject::getParamByIndex(int index)
{
    for(Parameter* param : params)
    {
        if(param->getIndex() == index)
        {
            return param;
        }
    }

    return NULL;
}

void ParamObject::removeParam(Parameter* param)
{
    params.remove(param);

    delete param;
}

void ParamObject::enqueueParamEnvelope(Trigger* tg)
{
    tg->tgworking = true;

    if(envelopes != NULL)  envelopes->group_next = tg;

    tg->group_prev = envelopes;
    tg->group_next = NULL;

    envelopes = tg;

    Parameter* prm = ((Envelope*)tg->el)->param;
    tg->prev_value = prm->getNormalizedValue();

    // New envelopes unblock the param ability to be changed by envelope
    prm->unblockEnvAffect();
}

void ParamObject::dequeueParamEnvelope(Trigger* tg)
{
    if(envelopes == tg)  envelopes = tg->group_prev;
    
    if(tg->group_prev != NULL)  tg->group_prev->group_next = tg->group_next;
    if(tg->group_next != NULL)  tg->group_next->group_prev = tg->group_prev;

    tg->group_prev = NULL;
    tg->group_next = NULL;
}

void ParamObject::handleClose()
{
    if (invokeButton != NULL)
    {
        invokeButton->release();
    }
}

void ParamObject::placeControls2(int maxWidth, int maxH)
{
    int boxWidth = 130;
    int boxHeight = 22;
    int rowNum = 0;
    int numPut = 0;
    int defaultWidth = maxWidth - 4;
    int defaultH = 14;

    setObjSpacing(1);
    setBorder(8);

    putStart(0, border);

    for(Parameter* prm : params)
    {
        Gobj* obj = NULL;

        if (prm && prm->getAutoPlaced())
        {
            if (prm->getType() == Param_Radio)
            {
                obj = new SelectorBox(prm);
            }
            else if (prm->getType() == Param_Selector)
            {
                obj = new SelectorBox(prm);
            }
            else if (prm->getType() == Param_Toggle)
            {
                obj = new ToggleBox(prm);
            }
            else 
            {
                obj = new Knob(prm, false);
                obj->setObjId("fx");
            }

        }

        if (obj)
        {
            obj->setWH(defaultWidth, defaultH);

            //obj->setHint(prm->getName());

            addObject(obj);

            putBelow(obj, obj->getW(), obj->getH());

            numPut++;
        }
    }

    if (numPut)
    {
        setWH(wndW + border, wndH + border);
    }
    else
    {
        setWH(wndW + border, border*2);
    }
}

void ParamObject::placeControls1(int maxWidth, int maxH)
{
    int boxWidth = 130;
    int boxHeight = 22;
    int rowNum = 0;
    int numPut = 0;
    int defaultWidth = 24;

    setObjSpacing(4);
    setBorder(8);

    putStart(border, border);

    for(Parameter* prm : params)
    {
        Gobj* obj = NULL;

        if (prm && prm->getAutoPlaced())
        {
            if (prm->getType() == Param_Radio)
            {
                obj = new SelectorBox(prm);
                obj->setWH(defaultWidth, obj->getH());
            }
            else if (prm->getType() == Param_Selector)
            {
                obj = new SelectorBox(prm);
                obj->setWH(defaultWidth, obj->getH());
            }
            else if (prm->getType() == Param_Toggle)
            {
                obj = new ToggleBox(prm);
                obj->setWH(defaultWidth, defaultWidth);
            }
            else 
            {
                obj->setWH(defaultWidth, defaultWidth);
                obj->setObjId("fx");
            }

        }

        if (obj)
        {
            addObject(obj);

            putRight(obj, obj->getW(), obj->getH());

            rowNum++;

            if (xstart1 + 4*(defaultWidth + objSpacing) > maxWidth)
            {
                rowNum = 0;

                returnLeft();
            }

            numPut++;
        }
    }

    if (numPut)
    {
        setWH(wndW + border, wndH + border);
    }
    else
    {
        setWH(wndW + border, border*2);
    }
}


void ParamObject::putStart(int x, int y)
{
    xstart1 = xstart2 = x;
    ystart1 = ystart2 = y;
}

void ParamObject::putRight(Gobj* obj, int ow, int oh)
{
    //Gobj::addObject(obj, xstart1, ystart1, ow, oh);
    int w = ow > 0 ? ow : obj->getW();
    int h = oh > 0 ? oh : obj->getH();

    obj->setVis(true);
    obj->setCoords1(xstart1, ystart1, w, h);

    xstart2 = xstart1;
    ystart2 = ystart1 + h + objSpacing;

    xstart1 += w + objSpacing;

    if (xstart1 > wndW)
    {
        wndW = xstart1 - objSpacing;
    }

    if (ystart1 + h + objSpacing > wndH)
    {
        wndH = ystart1 + h;
    }
}

void ParamObject::putBelow(Gobj* obj, int ow, int oh)
{
    //Gobj::addObject(obj, xstart2, ystart2, ow, oh);
    int w = ow > 0 ? ow : obj->getW();
    int h = oh > 0 ? oh : obj->getH();

    obj->setVis(true);
    obj->setCoords1(xstart2, ystart2, w, h);

    ystart1 = ystart2;
    xstart1 = xstart2 + w + objSpacing;

    ystart2 += h + objSpacing;

    if ((ystart2 - objSpacing) > wndH)
    {
        wndH = ystart2 - objSpacing;
    }

    if (xstart2 + w > wndW)
    {
        wndW = xstart2 + w;
    }
}

void ParamObject::spaceRight(int num)
{
    while (num > 0)
    {
        xstart1 += groupSpacing;
        xstart2 += groupSpacing;

        if (xstart1 > wndW)
        {
            wndW = xstart1;
        }

        num--;
    }
}

void ParamObject::spaceBelow(int num)
{
    while (num > 0)
    {
        ystart1 += groupSpacing;
        ystart2 += groupSpacing;

        if (ystart1 > wndH)
        {
            wndH = ystart1;
        }

        num--;
    }
}

void ParamObject::returnLeft()
{
    xstart1 = border;
    xstart2 = border;
    ystart1 = wndH + objSpacing;
    ystart2 = wndH + objSpacing;
}

void ParamObject::returnUp()
{
    xstart1 = wndW + objSpacing;
    xstart2 = wndW + objSpacing;
    ystart1 = border;
    ystart2 = border;
}


void ParamObject::finalizePuts()
{
    //WinObject::setWidthHeight(wndW + border, wndH + border);

    setWH(wndW + border - objSpacing, wndH + border - objSpacing);
}

