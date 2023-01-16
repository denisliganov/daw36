
#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_config.h"


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

class Parameter
{
public:

            ParamType           type;
            UnitsType           unitsType;
            int                 sign;

            int                 globalindex;
            int                 index;

            float               defaultValue;
            float               outVal;
            float               value;
            float               offset;
            float               range;
            float               logoffset;
            float               logRange;
            float               interval;
            float               lastval;  // used for ramping

            bool                grouped;
            bool                reversed;
            bool                recording;
            bool                envaffect;
            bool                envdirect;
            bool                envtweaked;
            bool                presetable;
            long                lastsetframe;
            Device36*           module;
            Trigger*            envelopes;
            Envelope*           autoenv;
            Envelope*           env;
            EnvPoint*           lastrecpoint;
            Control*            ctrlUpdatingFrom;

            float               declickCount;
            float               declickCoeff;

            std::string         prmName;
            std::string         prmValString;
            std::list<Control*> controls;


            Parameter();
            Parameter(float def_val, float offs, float rng);
            Parameter(float def_val, float offs, float rng, ParamType ptype);
            Parameter(std::string param_name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt = Units_Default);
            Parameter(std::string param_name, float def_val, float offs, float rng, UnitsType vt = Units_Default);
            Parameter(int p_value);
            Parameter(float def_val);

    virtual ~Parameter();

            void                setName(std::string name)   { prmName = name; };
            std::string         getName()                   { return prmName; };
            void                setValString(std::string str);
            std::string         getValString();
            std::string         getMaxValString();
            std::string         getUnitStr();
            std::string         getSignStr();
            std::string         calcValStr(float uv);
    virtual void                reset();
    virtual void                resetToInitial();
    virtual void                updateControls();
            float               getRange()      { return range; };
            float               getOffset()     { return offset; };
            float               getOutVal()     { return outVal; }
            float               getValue()      { return value; }
            float               getDefaultVal() { return defaultValue; }
            float               getNormalizedValue();
            float               getEditorValue();
            float               adjustForEditor(float val);

    virtual float               calcOutputValue(float val);
    virtual void                setValue(float val);
    virtual void                setNormalizedValue(float nval);
    virtual void                setDirectValueFromControl(float ctrlval);
    virtual void                setValueFromEnvelope(float envval, Envelope* env);
            void                adjustFromControl(Control* ctrl, int step, float nval=-1);
            void                setDefValue(float initial);
            void                setInterval(float newint) { interval = newint; }
            float               getInterval() { return interval; } 

            void                paramInit(std::string name, ParamType ptype, float def_val, float offs, float rng, UnitsType vt);
            void                addControl(Control* ct);
            void                removeControl(Control* ct);

            void                enqueueEnvelopeTrigger(Trigger* tg);
            void                dequeueEnvelopeTrigger(Trigger* tg);
            void                handleRecordingFromControl(float ctrlval);

            void                finishRecording();
            void                blockEnvAffect() { envaffect = false; }
            void                unblockEnvAffect() { envaffect = true;}
            bool                isRecording() { return recording; }
            void                setReversed(bool rev) { reversed = rev; }
            bool                getReversed() { return reversed; }
            void                setEnvDirect(bool envdir);
            bool                getEnvDirect();
            void                setLastVal(float lval);
        XmlElement*             save();
            void                load(XmlElement* xmlParamNode);
        XmlElement*             save4Preset();
            void                load4Preset(XmlElement* xmlParamNode);
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
            void                updateControls();
};


