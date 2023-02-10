
#include "36_paramobject.h"
#include "36_params.h"
#include "36_controls.h"
#include "36_events_triggers.h"



ParamObject::ParamObject()
{
    border = 10;
    xstart1 = ystart1 = border;
    xstart2 = ystart2 = border;
    groupSpacing = 8;
    objSpacing = 4;
    wndW = border;
    wndH = border;
    horizPut = false;

    paramLocked = false;

    invokeButton = NULL;
}

void ParamObject::addParam(Param* param)
{
    param->setModule(this);
    param->setEnvDirect(false);

    params.push_back(param);
}

void ParamObject::addParamWithControl(Param* param, std::string oname, Control* ctrl)
{
    addParam(param);

    if (ctrl)
    {
        param->addControl(ctrl);

        addObject(ctrl);
    }

    handleParamUpdate(param);
}

Param* ParamObject::getParamByName(char *param_name)
{
    for(Param* param : params)
    {
        if(_stricmp(param->getName().data(), param_name) == 0)
        {
            return param;
        }
    }

    return NULL;
}

Param* ParamObject::getParamByIndex(int index)
{
    for(Param* param : params)
    {
        if(param->getIndex() == index)
        {
            return param;
        }
    }

    return NULL;
}

void ParamObject::removeParam(Param* param)
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
    tg->prev_value = prm->getValueNormalized();

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

void ParamObject::putRight(Gobj* obj, int ow, int oh)
{
    if (obj)
    {
        Gobj::addObject(obj, xstart1, ystart1, ow, oh);
    }

    xstart2 = xstart1;
    ystart2 = ystart1 + oh + objSpacing;

    xstart1 += ow + objSpacing;

    if (xstart1 > wndW)
    {
        wndW = xstart1;
    }

    if (ystart1 + oh + objSpacing > wndH)
    {
        wndH = ystart1 + oh + objSpacing;
    }
}

void ParamObject::putBelow(Gobj* obj, int ow, int oh)
{
    if (obj)
    {
        Gobj::addObject(obj, xstart2, ystart2, ow, oh);
    }

    ystart1 = ystart2;
    xstart1 = xstart2 + ow + objSpacing;

    ystart2 += oh + objSpacing;

    if (ystart2 > wndH)
    {
        wndH = ystart2;
    }

    if (xstart2 + ow + objSpacing > wndW)
    {
        wndW = xstart2 + ow + objSpacing;
    }
}

void ParamObject::spaceRight()
{
    xstart1 += groupSpacing;
    xstart2 += groupSpacing;

    if (xstart1 > wndW)
    {
        wndW = xstart1;
    }
}

void ParamObject::spaceBelow()
{
    ystart1 += groupSpacing;
    ystart2 += groupSpacing;

    if (ystart1 > wndH)
    {
        wndH = ystart1;
    }
}

void ParamObject::goLeft()
{
    xstart1 = border;
    ystart1 = wndH;
    xstart2 = border;
    ystart2 = wndH;
}

void ParamObject::goTop()
{
    xstart1 = wndW;
    ystart1 = border;
    xstart2 = wndW;
    ystart2 = border;
}


void ParamObject::finalizePuts()
{
    WinObject::setWidthHeight(wndW + border, wndH + border);
}

