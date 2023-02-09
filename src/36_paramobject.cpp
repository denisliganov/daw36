
#include "36_paramobject.h"
#include "36_params.h"
#include "36_controls.h"
#include "36_events_triggers.h"



ParamObject::ParamObject()
{
    border = 10;
    xstart1 = ystart1 = border;
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
    Gobj::addObject(obj, xstart1, ystart1, ow, oh);

    if (horizPut)
    {
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
    else
    {
        ystart1 += oh + objSpacing;

        if (ystart1 > wndH)
        {
            wndH = ystart1;
        }

        if (xstart1 + ow + objSpacing > wndW)
        {
            wndW = xstart1 + ow + objSpacing;
        }
    }
}

void ParamObject::putBelow(Gobj* obj, int ow, int oh)
{
    Gobj::addObject(obj, xstart1, ystart1, ow, oh);

    if (horizPut)
    {
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
    else
    {
        ystart1 += oh + objSpacing;

        if (ystart1 > wndH)
        {
            wndH = ystart1;
        }

        if (xstart1 + ow + objSpacing > wndW)
        {
            wndW = xstart1 + ow + objSpacing;
        }
    }
}

void ParamObject::goLeft()
{
    if (horizPut)
    {
        xstart1 = border;
        ystart1 = wndH;
//        wndH = ymax + groupSpacing;
    }
    else
    {
        xstart1 = wndW;
        ystart1 = border;
    }
}

void ParamObject::goTop()
{
    if (horizPut)
    {
        xstart1 = border;
        ystart1 = wndH;
//        wndH = ymax + groupSpacing;
    }
    else
    {
        xstart1 = wndW;
        ystart1 = border;
    }
}

void ParamObject::putSpace()
{
    if (horizPut)
    {
        xstart1 += groupSpacing;
    }
    else
    {
        ystart1 += groupSpacing;
    }

    if (xstart1 > wndW)
    {
        wndW = xstart1;
    }

    if (ystart1 > wndH)
    {
        wndH = ystart1;
    }
}


void ParamObject::finalizePuts()
{
    WinObject::setWidthHeight(wndW + border, wndH + border);
}

