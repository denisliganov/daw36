

#pragma once

#include "36_globals.h"
#include "36_objects.h"




class Control : public Gobj
{
public:

            Control();
            Parameter*          getParam()      { return param; }
    virtual float               getMinStep()    { return 0; };
            bool                isActive()        { return active; }
    virtual void                updPosition()   {};
    virtual void                updValue()      {};
    virtual void                reset()         {};
    virtual void                removeParam(Parameter* p);
    virtual void                setParam(Parameter* p);
    virtual void                setFontId(FontId fId);
            void                setActive(bool act) { active = act; }

            bool                bgSaved;

protected:

            bool                active;
            bool                headerOn;
            int                 headerHeight;
            Device36*           dev;
            FontId              fontId;
            Image*              parentImage;
            Parameter*          param;
            int                 textHeight;
};

