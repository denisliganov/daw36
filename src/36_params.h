
#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_config.h"

#include <vector>

class AToggleButton;



typedef enum ParamType
{
    Param_Vol,
    Param_Pan,
    Param_Freq,
    Param_Log,
    Param_Bool,
    Param_Default
}ParamType;


typedef enum UnitsType
{
    Units_Ticks,
    Units_Hz,
    Units_Hz1,
    Units_Hz2,
    Units_kHz,
    Units_ms,
    Units_ms2,
    Units_Seconds,
    Units_Percent,
    Units_Integer,
    Units_Beats,
    Units_Octave,
    Units_dB,
    Units_dBGain,
    Units_Semitones,
    Units_DryWet,
    Units_String,
    Units_Default
}UnitsType;


class Param
{
public:
            Param();
            void                addControl(Control* ct);
            std::string         getName()           { return prmName; };
            int                 getIndex()          { return index; }
            bool                getEnvDirect();
            ParamType           getType()       { return type; }
            void                removeControl(Control* ct);
    virtual void                reset() {}
            void                setIndex(int idx)   { index = idx; }
            void                setName(std::string name)   { prmName = name; };
            void                setEnvDirect(bool envdir);
            void                setModule(ParamObject* md) { module = md; };

protected:

            Control*            ctrlUpdatingFrom;
            bool                envdirect;
            int                 globalindex;
            int                 index;
            ParamObject*        module;
            std::string         prmName;
            ParamType           type;
            std::string         unitStr;

            std::list<Control*> controls;
};

class Parameter : public Param
{
public:
            Parameter();
            Parameter(float def_val, float offs, float rng);
            Parameter(float def_val, float offs, float rng, ParamType ptype);
            Parameter(std::string param_name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt = Units_Default);
            Parameter(std::string param_name, float def_val, float offs, float rng, UnitsType vt = Units_Default);
            Parameter(int p_value);
            Parameter(float def_val);
            virtual ~Parameter();

            void                adjustFromControl(Control* ctrl, int step, float nval=-1, float min_step=0.1f);
            float               adjustForEditor(float val);
            void                blockEnvAffect() { envaffect = false; }
            std::string         calcValStr(float uv);
            void                dequeueEnvelopeTrigger(Trigger* tg);
            void                enqueueEnvelopeTrigger(Trigger* tg);
            void                finishRecording();
            std::string         getValString();
            std::string         getMaxValString();
            std::string         getUnitString();
            std::string         getSignStr();
            float               getRange()      { return range; };
            float               getOffset()     { return offset; };
            float               getOutVal()     { return outVal; }
            float               getValue()      { return value; }
            float               getDefaultValue() { return defaultValue; }
            float               getValueNormalized();
            float               getDefaultValueNormalized();
            float               getEditorValue();
            bool                getReversed() { return reversed; }
            float               getInterval() { return interval; } 
            void                handleRecordingFromControl(float ctrlval);
            bool                isRecording() { return recording; }
            void                load(XmlElement* xmlParamNode);
            void                load4Preset(XmlElement* xmlParamNode);
    virtual void                reset();
    virtual void                setValue(float val);
    virtual void                setNormalizedValue(float nval);
    virtual void                setValueFromEnvelope(float envval, Envelope* env);
            void                setDefValue(float initial);
            void                setInterval(float newint) { interval = newint; }
            void                setValString(std::string str);
            void                setReversed(bool rev) { reversed = rev; }
            void                setLastVal(float lval);
            void                setUnitString(std::string unit_str)     { unitStr = unit_str; }
        XmlElement*             save();
        XmlElement*             save4Preset();
    virtual void                updateLinks();
            void                unblockEnvAffect() { envaffect = true;}

            float               lastValue;  // used for ramping
            float               declickCount;
            float               declickCoeff;
            Trigger*            envelopes;
            Envelope*           autoenv;
            Envelope*           env;
            bool                envaffect;
            bool                envtweaked;
            long                lastsetframe;
            EnvPoint*           lastrecpoint;
            bool                grouped;

protected:

    virtual float               calcOutputValue(float val);
            void                paramInit(std::string name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt);

            float               defaultValue;
            float               interval;
            float               logoffset;
            float               logRange;
            float               outVal;
            float               offset;
            bool                presetable;
            float               range;
            bool                reversed;
            bool                recording;
            int                 sign;
            UnitsType           unitsType;
            float               value;

            std::string         prmValString;
};

class ParamToggle : public Param
{
public:

            ParamToggle(std::string name, bool def_val) {prmName = name; value = def_val;}

            bool                getValue()  { return value; }
            void                toggle();

protected:

            bool                value;
};

class ParamRadio : public Param
{
public:

            ParamRadio(std::string name) { prmName = name; currentOption = 0; }

            void                addOption(std::string opt) { options.push_back(opt); }
            int                 getNumOptions()     { return options.size(); }
       std::list<std::string>&  getOptions()     { return options; }
            int                 getCurrent()     { return currentOption; }
            void                setCurrent(int curr) { currentOption = curr;}

protected:

            int                 currentOption;

            std::list<std::string>  options;
};

class ParamSelector : public Param
{
public:

            ParamSelector(std::string name) { prmName = name; }
            void                addOption(std::string opt, bool val) { options.push_back(opt); optVals.push_back(val); }
            std::vector<std::string>&  getOptions()     { return options; }
            void                setValue(int optnum, bool new_val) { optVals[optnum] = new_val; lastChangedNum = optnum; }
            void                toggleValue(int optnum) { optVals[optnum] = !optVals[optnum]; lastChangedNum = optnum; }
            bool                getValue(int optnum)    { return optVals[optnum]; }
            int                 getLastChanged() { return lastChangedNum; }
            int                 getNumOptions()     { return options.size(); }

protected:

            int                 lastChangedNum;

            std::vector<std::string>  options;
            std::vector<bool>         optVals;
};

class BoolParam : public Parameter
{
public:

            bool                outval;
            AToggleButton*      atoggle;

            BoolParam(bool state);
            BoolParam(bool state, std::string name);
            BoolParam(std::string name, bool state, const char* unitsStr);
            void                SetBoolValue(bool bval);
            bool                getOutVal();
            void                updateLinks();
};


