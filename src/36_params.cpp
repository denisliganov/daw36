

#include "rosic/rosic.h"

#include "36_params.h"
#include "36_controls.h"
#include "36_controls.h"
#include "36_device.h"
#include "36_utils.h"
#include "36_transport.h"
#include "36_macros.h"

#include "stdio.h"










Parameter::Parameter()
{
    paramInit("", Param_Default, 0, 0, 1, Units_Default);
}

Parameter::Parameter(float min_val, float max_val, float default_val)
{
    paramInit("", Param_Default, min_val, max_val, default_val, Units_Default);
}

Parameter::Parameter(std::string name, float min_val, float max_val, float default_val, UnitsType vt)
{
    paramInit(name, Param_Default, min_val, max_val, default_val, vt);
}

Parameter::Parameter(std::string name, ParamType ptype, float min_val, float max_val, float default_val, UnitsType vt)
{
    paramInit(name, ptype, min_val, max_val, default_val, vt);
}

Parameter::Parameter(std::string name, ParamType param_type)
{
    type = param_type;
    prmName = name;
    index = -1;
    autoPlaced = true;
    module = NULL;

    if (type == Param_Selector || type == Param_Toggle || type == Param_Radio)
    {
        currentOption = 0;

        if (type == Param_Toggle)
        {
            addOption(name, false);
        }
    }
    else if (type == Param_Vol)
    {
        paramInit(name, Param_Vol, 0.f, DAW_VOL_RANGE, 1.f, Units_dB);
    }
    else if (type == Param_Pan)
    {
        paramInit(name, Param_Pan, -1.f, 1.f, 0.f, Units_Percent);
    }
    else if (type == Param_Default)
    {
        paramInit(name, Param_Default, 0.f, 1.f, 0.f, Units_PercentNormalized);
    }
}

Parameter::Parameter(std::string param_name, bool def_val)
{
    type = Param_Toggle;
    index = -1;
    autoPlaced = true;
    module = NULL;

    currentOption = def_val ? 1 : 0;

    addOption(param_name, def_val);

    setName(param_name);
}

void Parameter::paramInit(std::string name, ParamType pt, float min_val, float max_val, float default_val, UnitsType vt)
{
    type = pt;
    unitsType = vt;
    prmName = name;
    index = -1;
    autoPlaced = true;
    recording = false;
    envaffect = true;
    envdirect = true;
    envtweaked = false;
    reversed = false;

    envelopes = NULL;
    module = NULL;
    autoenv = NULL;
    env = NULL;
    lastrecpoint = NULL;
    ctrlUpdatingFrom = NULL;

    offset = min_val;
    range = max_val - min_val;
    defaultValue = default_val;

    outVal = 0;
    interval = -1;
    lastValue = -1;
    declickCount = 0;
    declickCoeff = 0;
    sign = 0;

    if (pt == Param_Log)
    {
        offset = 0;
        range = 1.f;
        logoffset = offset;
        logRange = range;
    }

    globalParams.push_front(this);

    setValue(defaultValue);
}

Parameter::~Parameter()
{
    globalParams.remove(this);
}

void Parameter::addControl(Control* ct)
{
    controls.push_back(ct);

    //ct->addParam(this);
}

bool Parameter::getEnvDirect()
{
    return envdirect;
}

Control* Parameter::getControl()
{
    if (controls.size() > 0)
    {
        return controls.front();
    }
    else
    {
        return NULL;
    }
}


std::string Parameter::getUnitString()
{
    if (unitStr != "")
    {
        return unitStr;
    }

    switch (unitsType)
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
        case Units_PercentNormalized:
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
    switch (unitsType)
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

std::string Parameter::calcValStr(float val)
{
    if (type == Param_Pan)
    {
        char str[100] = {};
        float absVal = (val);
        int pval = abs(int(absVal * 100));
        std::string valStr = String(pval);

        if (value < 0)
        {
            valStr = "<" + valStr;
        }
        else if (value > 0)
        {
            valStr = valStr + ">";
        }

        return valStr;
    }
    else if (type == Param_Vol)
    {
        std::string valStr;
        char str[100] = {};

        if (unitsType == Units_dB)
        {
            if (outVal == 0)
            {
                valStr = "INF";
            }
            else
            {
                double pval = (amp2dB(val));
                sprintf(str, "%.2f", pval);
                valStr = str;
            }
        }
        else
        {
            sprintf(str, ("%d"), int(value*100));
            valStr = str;
        }

        return valStr;
    }
    else
    {
        sign = val > 0 ? 1 : val < 0 ? -1 : 0;

        char str[100] = {};
        std::string stdstr;

        float absVal = (val);

        switch (unitsType)
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
                sprintf(str, ("%.0f"), absVal);
            } break;
            case Units_PercentNormalized:
            {
                sprintf(str, ("%.0f"), absVal*100);
            } break;
            case Units_dB:
            {
                sprintf(str, ("%.1f"), absVal);
            }break;
            case Units_dBGain:
            {
                if (absVal <= 0)
                {
                    sprintf(str, "%.1f", absVal);
                }
                else
                {
                    sprintf(str, "+%.1f", absVal);
                }
            }break;
            case Units_ms:
                sprintf(str, "%.f", absVal);
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
                sprintf(str, "%.0f/%.0f", (1 - absVal) * 100, absVal * 100);
                break;
            case Units_Default:
                sprintf(str, "%.2f", absVal);
                break;
            case Units_Ticks:
                sprintf(str, "%.2f", absVal);
                break;
        }

        if (stdstr.size() == 0)
        {
            stdstr = str;
        }

        return str;
    }
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

void Parameter::finishRecording()
{
    if (recording == true)
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
    if (GRecOn == true && GPlaying && autoenv == NULL)
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

void Parameter::enqueueEnvelopeTrigger(Trigger* tg)
{
    /*
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
    */
}

void Parameter::dequeueEnvelopeTrigger(Trigger* tg)
{
    // dequeue
}

// Dedicated saving methods that avoid saving and loading global index

XmlElement* Parameter::save4Preset()
{
    XmlElement* xmlParam = new XmlElement(T("Parameter"));

    xmlParam->setAttribute(T("index"), index);
    xmlParam->setAttribute(T("value"), defaultValue);

    return xmlParam;
}

XmlElement* Parameter::save()
{
    XmlElement* xmlParam = new XmlElement(T("Parameter"));

    xmlParam->setAttribute(T("GlobalIndex"), globalindex);
    xmlParam->setAttribute(T("index"), index);

    xmlParam->setAttribute(T("value"), defaultValue);

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

    float fval = (float)xmlNode->getDoubleAttribute(T("value"), defaultValue);

    //setValue(fval);
    //setDefValue(fval);
}

void Parameter::setNormalizedValue(float nval)
{
    setValue(nval * range + offset);
}

void Parameter::adjustFromControl(Control* ctrl, int step, float nval, float min_step)
{
    ctrlUpdatingFrom = ctrl;

    if (step != 0)
    {
        if (interval > 0)
        {
            value += step * interval;

            LIMIT(value, offset, offset + range);

            setValue(value);
        }
        else
        {
            float prevV = getNormalizedValue();
            float newV = prevV;

            if (ctrl)
                newV += step * ctrl->getMinStep();
            else
                newV += step * min_step;

            LIMIT(newV, 0, 1);

            float defNorm = float(defaultValue - offset) / range;

            if (defNorm < prevV && defNorm > newV ||
                defNorm > prevV && defNorm < newV)
            {
                newV = defNorm;
            }

            setNormalizedValue(newV);
        }
    }
    else
    {
        LIMIT(nval, 0, 1);

        setNormalizedValue(nval);
    }

    //setDefValue(value);

    blockEnvAffect();  // Block this param update from currently working envelopes

    handleRecordingFromControl(getNormalizedValue());

    //MProject.setChange();
}


// Normalize to 0-1 range

float Parameter::getNormalizedValue()
{
    return (value - offset) / range;
}

float Parameter::getDefaultValueNormalized()
{
    return (defaultValue - offset) / range;
}

float Parameter::getEditorValue()
{
    if (type == Param_Pan)
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
    if (type == Param_Pan)
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
    if (type == Param_Freq)
    {
        if (reversed == false)
        {
            return (float)(20.0 * pow(1000.0, (double)val));
        }
        else
        {
            return (float)(20.0 * pow(1000.0, (double)(1 - val)));
        }
    }
    else if (type == Param_Log)
    {
        if (reversed == false)
        {
            return (float)((double)logoffset * pow((double)logRange, (double)val));
        }
        else
        {
            return (float)((double)logoffset * pow((double)logRange, (double)(1 - val)));
        }
    }
    else if (type == Param_Vol)
    {
        return GetVolOutput(val);
    }
    else
    {
        return val;
    }
}

void Parameter::removeControl(Control* ctrl)
{
    controls.remove(ctrl);
}

void Parameter::setEnvDirect(bool envdir)
{
    envdirect = envdir;
}

void Parameter::setValue(float val)
{
    value = val;

    LIMIT(value, offset, offset + range);

    if (interval > 0)
    {
        value = float(RoundFloat(value / interval)) * interval;
    }

    outVal = calcOutputValue(value);

    setValString(calcValStr(outVal));

    updateLinks();
}

void Parameter::updateLinks()
{
    for (Control* ctrl : controls)
    {
        if (ctrl != ctrlUpdatingFrom)
        {
            ctrl->updPosition();
        }
    }

    if (module != NULL)
    {
        module->handleParamUpdate(this);
    }

    ctrlUpdatingFrom = NULL;
}

void Parameter::setLastVal(float lval)
{
    lastValue = lval;
}

void Parameter::reset()
{
    setValue(defaultValue);
}

void Parameter::setDefValue(float initial)
{
    defaultValue = initial;

    envtweaked = false;
}

void Parameter::setValueFromEnvelope(float envval, Envelope* env)
{
    if (envaffect == true && env != autoenv)
    {
        envtweaked = true;

        // Check for shit

        if (envval < 0 || envval > 1)
        {
            envval = 0;
        }

        setValue(envval * range + offset);

        //setLastVal(outVal);
    }
}

// Toggle

void Parameter::toggleValue()
{
    optValues[0] = !optValues[0]; 

    module->handleParamUpdate(this);
}

void Parameter::setBoolValue(bool val)
{
    optValues[0] = val; 

    module->handleParamUpdate(this);
}

bool  Parameter::getBoolValue() 
{
    return optValues[0];
}

// Radio/Selector

void Parameter::addOption(std::string opt, bool val)    
{ 
    options.push_back(opt); 

    optValues.push_back(val); 
}

void Parameter::addOption(std::string opt)              
{ 
    options.push_back(opt); 
}

std::vector<std::string>& Parameter::getAllOptions()                         
{ 
    return options; 
}

int Parameter::getNumOptions()
{ 
    return options.size(); 
}

int  Parameter::getCurrentOption()
{ 
    return currentOption; 
}

void  Parameter::setCurrentOption(int curr)
{ 
    currentOption = curr; 

    setValString(options[currentOption]); 

    if (module)
    {
        module->handleParamUpdate(this);
    }
}

// Selector

void  Parameter::setOptionVal(int optnum, bool new_val)  
{
    optValues[optnum] = new_val; 

    if (module)
    {
        module->handleParamUpdate(this);
    }
}

bool   Parameter::getOptionVal(int optnum)
{ 
    return optValues[optnum]; 
}

std::string   Parameter::getOptionStr(int optnum)
{ 
    int opt = optnum >= options.size() ? options.size() - 1 : optnum;
    
    return options[opt]; 
}

void  Parameter::toggleOption(int optnum)
{ 
    optValues[optnum] = !optValues[optnum]; 

    if (module)
    {
        module->handleParamUpdate(this);
    }
}


