
#pragma once

#include "36_globals.h"
#include "36_params.h"




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


class Parameter : public Param
{
public:
            Parameter();
            Parameter(float min_val, float max_val, float default_val);
            Parameter(float min_val, float max_val, float default_val, ParamType ptype);
            Parameter(std::string param_name, ParamType ptype, float min_val, float max_val, float default_val, UnitsType vt = Units_Default);
            Parameter(std::string param_name, float min_val, float max_val, float default_val, UnitsType vt = Units_Default);
            Parameter(int p_value);
            Parameter(float def_val);
            virtual ~Parameter();

            void                adjustFromControl(Control* ctrl, int step, float nval=-1, float min_step=0.1f);
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
            float               getRange()      { return range; };
            float               getOffset()     { return offset; };
            float               getOutVal()     { return outVal; }
            float               getValue()      { return value; }
            float               getDefaultValue() { return defaultValue; }
            float               getValueNormalized();
            float               getDefaultValueNormalized();
    virtual float               getEditorValue();
            bool                getReversed() { return reversed; }
            float               getInterval() { return interval; } 
            void                handleRecordingFromControl(float ctrlval);
            bool                isRecording() { return recording; }
            void                load(XmlElement* xmlParamNode);
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



