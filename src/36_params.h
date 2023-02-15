
#pragma once

#include "36_globals.h"

#include <list>


typedef enum ParamType
{
    Param_Vol,
    Param_Pan,
    Param_Freq,
    Param_Log,
    Param_Bool,
    Param_Default
}ParamType;



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




