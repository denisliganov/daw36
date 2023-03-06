

#pragma once

#include "36_globals.h"
#include "36_objects.h"




class Control : public Gobj
{
public:

            Control();
    virtual void                addParam(Param* p);
    virtual void                removeParam(Param* p);
    virtual void                updPosition() {};
    virtual void                updValue() {};
    virtual float               getMinStep() {return 0;};
    virtual void                reset() {};
    virtual void                setFontId(FontId fId);

protected:

            bool                headerOn;
            int                 headerHeight;
            Device36*           dev;
            FontId              fontId;
            Image*              parentImage;
            Param*              param;
            int                 textHeight;
};

