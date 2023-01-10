

#include "rosic/rosic.h"


#include "36_globals.h"
#include "36.h"
#include "36_params.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_controls.h"
#include "36_events_triggers.h"
#include "36_device.h"
#include "36_utils.h"
#include "36_audio_dev.h"
#include "36_juce_components.h"
#include "36_grid.h"
#include "36_transport.h"
#include "36_macros.h"

#include "stdio.h"


Parameter::Parameter()
{
    paramInit("", Param_Default, 0, 0, 1, Units_Default);
}

Parameter::Parameter(float def_val)
{
    paramInit("", Param_Default, def_val, 0, 1, Units_Default);
}

Parameter::Parameter(float def_val, float offs, float rng)
{
    paramInit("", Param_Default, def_val, offs, rng, Units_Default);
}

Parameter::Parameter(std::string name, float def_val, float offs, float rng, UnitsType vt)
{
    paramInit(name, Param_Default, def_val, offs, rng, vt);
}

Parameter::Parameter(std::string name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt)
{
    paramInit(name, ptype, def_val, offs, rng, vt);
}

void Parameter::paramInit(std::string name, ParamType pt, float def_val, float offs, float rng, UnitsType vt)
{
    type = pt;
    unitsType = vt;

    prmName = name;

    index = 0;
    grouped = false;
    recording = false;
    envaffect = true;
    envdirect = true;
    envtweaked = false;
    presetable = true;
    reversed = false;

    envelopes = NULL;
    module = NULL;
    autoenv = NULL;
    env = NULL;
    lastrecpoint = NULL;
    ctrlUpdatingFrom = NULL;

    offset = offs;
    range = rng;

    defaultValue = def_val;
    outVal = 0;
    interval = -1;
    lastval = -1;
    declickCount = 0;
    declickCoeff = 0;
    sign = 0;

    if(pt == Param_Log)
    {
        offset = 0;
        range = 1.f;
        logoffset = offs;
        logRange = rng;
    }

    params.push_front(this);

    setValue(defaultValue);
}

Parameter::~Parameter()
{
    params.remove(this);
}

std::string Parameter::getUnitStr()
{
    switch(unitsType)
    {
        case Units_Hz:
        case Units_Hz1:
        case Units_Hz2:
            return "Hz";
            break;
        case Units_kHz:
            return "kHz";
            break;
        case Units_Percent:
        {
            return "%";
        } break;
        case Units_dB:
        case Units_dBGain:
        {
            return "dB";
        }break;
        case Units_ms:
        case Units_ms2:
            return "ms";
            break;
        case Units_Seconds:
            return "sec";
            break;
        case Units_Octave:
            return "oct";
            break;
        case Units_Semitones:
            return "st";
            break;
        case Units_Beats:
            return "beats";
            break;
        case Units_Ticks:
            return "ticks";
            break;
        default:
            return "";
            break;
    }
}

std::string Parameter::getSignStr()
{
    switch(unitsType)
    {
        case Units_dB:
            return outVal == 1 ? "" : outVal > 1 ? "+" : "-";
            break;
        default:
            return outVal == 0 ? "" : outVal > 0 ? "" : "-";
            break;
    }

    return "";
}

void  Parameter::setValString(std::string str) 
{
    prmValString = str;
}

std::string  Parameter::getValString() 
{
    return prmValString;
}

std::string  Parameter::getMaxValString() 
{
    return calcValStr(calcOutputValue(offset + range)); 
}

std::string Parameter::calcValStr(float val)
{
    sign = val > 0 ? 1 : val < 0 ? -1 : 0;

    if(unitsType != Units_String)
    {
        char str[100] = {};
        std::string stdstr;

        float absVal = (val);

        switch(unitsType)
        {
            case Units_Hz:
                sprintf(str, "%.0f", absVal);
                break;
            case Units_Hz1:
                sprintf(str, "%.1f", absVal);
                break;
            case Units_Hz2:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_kHz:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Integer:
                sprintf(str, "%.0f", absVal);
                break;
            case Units_Percent:
            {
                if(type == Param_Pan)
                {
                    int pval = abs(int(absVal*100));

                    stdstr = String(pval);

                    if(value < 0)
                    {
                        stdstr = "<" + stdstr;
                    }
                    else if(value > 0)
                    {
                        stdstr = stdstr + ">";
                    }
                }
                else
                {
                    sprintf(str, ("%.0f"), absVal);
                }
            } break;
            case Units_dB:
            {
                if(type == Param_Vol)
                {
                    if(outVal == 0)
                    {
                        stdstr = "INF";
                    }
                    else
                    {
                        double pval = (amp2dB(val));

                        sprintf(str, "%.2f", pval);

                        stdstr = str;
                    }
                }
                else
                {
                    sprintf(str, ("%.1f"), absVal);
                }
            }break;
            case Units_dBGain:
            {
                if(val <= 0)
                {
                    sprintf(str, "%.1f", absVal);
                }
                else
                {
                    sprintf(str, "+%.1f", absVal);
                }
            }break;
            case Units_ms:
                sprintf(str, "%.1f", absVal);
                break;
            case Units_ms2:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Seconds:
                sprintf(str, "%.1f", absVal);
                break;
            case Units_Octave:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Semitones:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Beats:
                sprintf(str, "%.1f", absVal);
                break;
            case Units_DryWet:
                sprintf(str, "%.0f/%.0f", (1 - absVal)*100, absVal*100);
                break;
            case Units_Default:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Ticks:
                sprintf(str, "%.1f", absVal);
                break;
        }

        if(stdstr.size() == 0)
        {
            stdstr = str;
        }

        return stdstr;
    }

    return "";
}

void Parameter::finishRecording()
{
    if(recording == true)
    {
        recording = false;

        //autoenv->Simplify();

        autoenv = NULL;
        env = NULL;

        recParams.remove(this);
    }
}

void Parameter::handleRecordingFromControl(float ctrlval)
{
    if(GRecOn == true && GPlaying && autoenv == NULL)
    {
        // First time envelope creation
        
        // _MKeyboard.SaveState();
        // int cline = _MKeyboard.GetLine();

        /*
        // Advance line if it's already busy by a recording param (cancelled feature)
        Parameter* otherparam = first_rec_param;
        while(otherparam != NULL && cline < 999)
        {
            if(otherparam->cmdenv->track_line == cline)
            {
                cline++;
                otherparam = first_rec_param;
                continue;
            }
            otherparam = otherparam->rec_next;
        }*/

        /*
        _MKeyboard.SetGrid(Grid);
        _MKeyboard.SetPos(float(MainClock->GetTickFromFrame(GridPlayer->currFrame_tsync)), cline);

        cmdenv = Param_CreateCommand(this);

        autoenv = (Envelope*)cmdenv->paramedit;
        //autoenv->p_first->y_norm = ctrlval;
        autoenv->firstpoint->big = false;
        autoenv->folded = true;

        AddNewElement(cmdenv, true);

        autoenv->SetLength(MainClock->GetTickFromFrame(GridPlayer->currFrame_tsync) - cmdenv->start_tick);
        lastrecpoint = autoenv->lastpoint;

        _MKeyboard.RestoreState();

        if(recording == false)
        {
            recording = true;

            Global->recParams.push_back(this);
        }

        _MKeyboard.grid->Redraw(true);
        */
    }
}

void Parameter::addControl(Control* ct)
{
    controls.push_back(ct);

    ct->addParam(this);

}

void Parameter::removeControl(Control* ctrl)
{
    controls.remove(ctrl);
}

void Parameter::enqueueEnvelopeTrigger(Trigger* tg)
{
    tg->tgworking = true;

    // enqueue

    // Set initial envelope data value as tg->prev_value

    if(type == Param_Pan)
    {
        tg->prev_value = 1 - (value - offset)/range;
    }
    else
    {
        tg->prev_value = (value - offset)/range;
    }

    // New envelopes unblock the param ability to be changed by envelope

    unblockEnvAffect();
}

void Parameter::dequeueEnvelopeTrigger(Trigger* tg)
{
    // dequeue
}

// Dedicated saving methods that avoid saving and loading global index

XmlElement* Parameter::save4Preset()
{
    XmlElement * xmlParam = new XmlElement(T("Parameter"));

    xmlParam->setAttribute(T("index"), index);
    xmlParam->setAttribute(T("value"), defaultValue);

    if(type == Param_Bool)
    {
        BoolParam* bp = (BoolParam*)this;

        xmlParam->setAttribute(T("bval"), bp->outval ? 1 : 0);
    }

    return xmlParam;
}

void Parameter::load4Preset(XmlElement* xmlNode)
{
    if(type == Param_Bool)
    {
        BoolParam* bp = (BoolParam*)this;

        bp->SetBoolValue(xmlNode->getBoolAttribute(T("bval"), bp->outval));
    }
    else
    {
        float fval = (float)xmlNode->getDoubleAttribute(T("value"), defaultValue);

        setValue(fval);
        setInitialValue(fval);
    }
}

XmlElement* Parameter::save()
{
    XmlElement * xmlParam = new XmlElement(T("Parameter"));

    xmlParam->setAttribute(T("GlobalIndex"), globalindex);
    xmlParam->setAttribute(T("index"), index);

    if(type == Param_Bool)
    {
        BoolParam* bp = (BoolParam*)this;
        xmlParam->setAttribute(T("bval"), bp->outval ? 1 : 0);
    }
    else
    {
        xmlParam->setAttribute(T("value"), defaultValue);
    }

    return xmlParam;
}

void Parameter::load(XmlElement* xmlNode)
{
    globalindex = xmlNode->getIntAttribute(T("GlobalIndex"), globalindex);

    /*
    index = xmlNode->getIntAttribute(T("index"), index);
    xmlNode->getStringAttribute(T("name"), T("default")).copyToBuffer(name, MAX_PARAM_NAME);
    */

    //char xname[MAX_PARAM_NAME];
    //xmlNode->getStringAttribute(T("name"), T("default")).copyToBuffer(xname, MAX_PARAM_NAME);
    //if(strcmp(name, xname) == 0)
    {
        if(type == Param_Bool)
        {
            BoolParam* bp = (BoolParam*)this;

            bp->SetBoolValue(xmlNode->getBoolAttribute(T("bval"), bp->outval));
        }
        else
        {
            float fval = (float)xmlNode->getDoubleAttribute(T("value"), defaultValue);

            //setValue(fval);
            //setInitialValue(fval);
        }
    }
}

void Parameter::setValueFromControl(Control* ctrl, float ctrl_val)
{
    ctrlUpdatingFrom = ctrl;

    setValue(ctrl_val*range + offset);

    setInitialValue(value);

    blockEnvAffect();  // Block this param update from currently working envelopes

    handleRecordingFromControl(ctrl_val);

    MProject.setChange();
}

void Parameter::adjustValue(float deltaValue)
{
    float v = getNormalizedValue();

    v += deltaValue;

    LIMIT(v, 0, 1);

    setValueFromControl(NULL, v);
}

// Normalize to 0-1 range

float Parameter::getNormalizedValue()
{
    return (value - offset)/range;
}

float Parameter::getEditorValue()
{
    if(type == Param_Pan)
    {
        return 1.f - getNormalizedValue();
    }
    else
    {
        return getNormalizedValue();
    }
}

float Parameter::adjustForEditor(float val)
{
    if(type == Param_Pan)
    {
        return range - (val - offset) + offset;
    }
    else
    {
        return val;
    }
}

float Parameter::calcOutputValue(float val)
{
    if(type == Param_Vol)
    {
        return GetVolOutput(val);
    }
    else if(type == Param_Freq)
    {
        if(reversed == false)
        {
            return (float)(20.0*pow(1000.0, (double)val));
        }
        else
        {
            return (float)(20.0*pow(1000.0, (double)(1 - val)));
        }
    }
    else if(type == Param_Log)
    {
        if(reversed == false)
        {
            return (float)((double)logoffset*pow((double)logRange, (double)val));
        }
        else
        {
            return (float)((double)logoffset*pow((double)logRange, (double)(1 - val)));
        }
    }
    else
    {
        return val;
    }
}

void Parameter::setValue(float val)
{
    value = val;

    if(interval > 0)
    {
        value = float(RoundFloat(value/interval))*interval;
    }

    outVal = calcOutputValue(value);

    setValString(calcValStr(outVal));

    updateControls();
}

void Parameter::updateControls()
{
    for(Control* ctrl : controls)
    {
        if(ctrl != ctrlUpdatingFrom)
        {
            ctrl->updPosition();
        }
    }

    if(module != NULL)
    {
        module->handleParamUpdate(this);
    }

    ctrlUpdatingFrom = NULL;
}

void Parameter::setLastVal(float lval)
{
    lastval = lval;
}

void Parameter::reset()
{
    setValue(defaultValue);
}

void Parameter::resetToInitial()
{
    setValue(defaultValue);
}

void Parameter::setInitialValue(float initial)
{
    defaultValue = initial;

    envtweaked = false;
}

// The only difference between the above function and below is that below sets passed value directly, without
// actualizing it with range and offset

void Parameter::setDirectValueFromControl(float ctrlval)
{
    setValue(ctrlval);

    setInitialValue(ctrlval);

    blockEnvAffect();  // Block this param update from currently working envelopes

    handleRecordingFromControl(ctrlval);

    MProject.setChange();
}

void Parameter::setValueFromEnvelope(float envval, Envelope * env)
{
    if(envaffect == true && env != autoenv)
    {
        envtweaked = true;

        // Check for shit

        if(envval < 0 || envval > 1)
        {
            envval = 0;
        }

        setValue(envval*range + offset);

        //setLastVal(outVal);
    }
}

void Parameter::setEnvDirect(bool envdir)
{
    envdirect = envdir;
}

bool Parameter::getEnvDirect()
{
    return envdirect;
}



BoolParam::BoolParam(bool value)
{
    type = Param_Bool;

    outval = value;
    atoggle = NULL;
}

BoolParam::BoolParam(bool value, std::string name)
{
    type = Param_Bool;

    setName(name);
    outval = value;
    atoggle = NULL;
    unitsType = Units_String;
}

BoolParam::BoolParam(std::string name, bool value, const char* unitsStr)
{
    type = Param_Bool;

    setName(name);
    outval = value;
    atoggle = NULL;
    unitsType = Units_String;

    setValString(unitsStr);
}

void BoolParam::SetBoolValue(bool bval)
{
    outval = bval;

    updateControls();
}

bool BoolParam::getOutVal()
{
    return outval;
}

void BoolParam::updateControls()
{
    Parameter::updateControls();

    if(atoggle != NULL)
    {
        const MessageManagerLock mmLock;

        atoggle->setToggleState(outval, false);
    }
}

