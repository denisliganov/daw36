
#pragma once

#include "36_globals.h"
#include "36_config.h"

#include <list>
#include <vector>
#include <string>



typedef enum ParamType
{
    Param_Default,
    Param_Vol,
    Param_Pan,
    Param_Freq,
    Param_Log,
    Param_Toggle,
    Param_Selector,
    Param_Radio
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






class Parameter
{
public:
            Parameter();
            Parameter(std::string param_name, ParamType param_type);
            Parameter(std::string param_name, bool def_val);            // For Param_Toggle
            Parameter(float min_val, float max_val, float default_val);
            Parameter(float min_val, float max_val, float default_val, ParamType ptype);
            Parameter(std::string param_name, ParamType ptype, float min_val, float max_val, float default_val, UnitsType vt = Units_Default);
            Parameter(std::string param_name, float min_val, float max_val, float default_val, UnitsType vt = Units_Default);

            virtual ~Parameter();


            void                addControl(Control* ct);
            std::string         getName()           { return prmName; };
            int                 getIndex()          { return index; }
            bool                getEnvDirect();
            ParamType           getType()       { return type; }
            void                removeControl(Control* ct);
            void                setIndex(int idx)   { index = idx; }
            void                setName(std::string name)   { prmName = name; };
            void                setEnvDirect(bool envdir);
            void                setModule(ParamObject* md) { module = md; };


            void                adjustFromControl(Control* ctrl, int step, float nval = -1, float min_step = 0.1f);
    virtual float               adjustForEditor(float val);
            void                blockEnvAffect() { envaffect = false; }
    virtual std::string         calcValStr(float uv);
            void                dequeueEnvelopeTrigger(Trigger* tg);
            void                enqueueEnvelopeTrigger(Trigger* tg);
            void                finishRecording();
            std::string         getValString();
            std::string         getMaxValString();
            std::string         getUnitString();
            std::string         getSignStr();
            float               getRange() { return range; };
            float               getOffset() { return offset; };
            float               getOutVal() { return outVal; }
            float               getValue() { return value; }
            float               getDefaultValue() { return defaultValue; }
            float               getNormalizedValue();
            float               getDefaultValueNormalized();
    virtual float               getEditorValue();
            bool                getAutoPlaced() { return autoPlaced; }
            bool                getReversed() { return reversed; }
            float               getInterval() { return interval; }
            void                handleRecordingFromControl(float ctrlval);
            bool                isRecording() { return recording; }
            void                load(XmlElement* xmlParamNode);
    virtual void                reset();
            void                setAutoPlaced(bool auto_placed) { autoPlaced = auto_placed; }
    virtual void                setValue(float val);
    virtual void                setNormalizedValue(float nval);
    virtual void                setValueFromEnvelope(float envval, Envelope* env);
            void                setDefValue(float initial);
            void                setInterval(float newint) { interval = newint; }
            void                setValString(std::string str);
            void                setReversed(bool rev) { reversed = rev; }
            void                setLastVal(float lval);
            void                setUnitString(std::string unit_str) { unitStr = unit_str; }
            XmlElement*         save();
            XmlElement*         save4Preset();
            virtual void        updateLinks();
            void                unblockEnvAffect() { envaffect = true; }


            Control*            ctrlUpdatingFrom;
            bool                envdirect;
            int                 globalindex;
            int                 index;
            ParamObject*        module;
            std::string         prmName;
            ParamType           type;
            std::string         unitStr;

            std::list<Control*> controls;

            float               lastValue;          // used for ramping
            float               declickCount;
            float               declickCoeff;
            Trigger*            envelopes;
            Envelope*           autoenv;
            Envelope*           env;
            bool                envaffect;
            bool                envtweaked;
            long                lastsetframe;
            EnvPoint*           lastrecpoint;
            bool                autoPlaced;


// Toggle
            bool                getBoolValue() { return currentOption > 0 ? true : false; }
            void                toggleValue();

// Radio/Selector
            void                addOption(std::string opt, bool val)    { options.push_back(opt); optValues.push_back(val); }
            void                addOption(std::string opt)              { options.push_back(opt); }
            std::vector<std::string>&   getAllOptions()                         { return options; }
            int                 getNumOptions()                         { return options.size(); }
            int                 getCurrentOption()                      { return currentOption; }
            void                setCurrentOption(int curr)               { currentOption = curr; }

// Selector
            void                setOptionVal(int optnum, bool new_val)  { optValues[optnum] = new_val; }
            bool                getOptionVal(int optnum)                { return optValues[optnum]; }
            void                toggleOption(int optnum)                 { optValues[optnum] = !optValues[optnum]; }

protected:

            virtual float       calcOutputValue(float val);
            void                paramInit(std::string name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt);

            float               defaultValue;
            float               interval;
            float               logoffset;
            float               logRange;
            float               outVal;
            float               offset;
            float               range;
            bool                reversed;
            bool                recording;
            int                 sign;
            UnitsType           unitsType;
            float               value;
            std::string         prmValString;


            std::vector<std::string>    options;
            std::vector<bool>           optValues;
            int                         currentOption;
};


