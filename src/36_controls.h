

#pragma once

#include "36_globals.h"
#include "36_objects.h"




class Control : public Gobj
{
public:

            Control();
    virtual void                addParam(Parameter* p);
    virtual void                removeParam(Parameter* p);
    virtual void                updPosition() {};
    virtual void                updValue() {};
    virtual float               getMinStep() {return 0;};
    virtual void                reset() {};
    virtual void                setFontId(FontId fId);

protected:

            bool                bgSaved;
            bool                headerOn;
            int                 headerHeight;
            Device36*           dev;
            FontId              fontId;
            Image*              parentImage;
            Parameter*          param;
            int                 textHeight;
};

