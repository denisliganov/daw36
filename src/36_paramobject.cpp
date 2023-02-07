
#include "36_paramobject.h"
#include "36_params.h"
#include "36_controls.h"
#include "36_events_triggers.h"



ParamObject::ParamObject()
{
    paramLocked = false;
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


