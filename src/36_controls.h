

#pragma once

#include "36_globals.h"
#include "36_objects.h"




class Control : public Gobj
{
public:

            Control();
    virtual void                addParam(Param* p);
    virtual void                updPosition() {};
    virtual void                updValue() {};
    virtual float               getMinStep() {return 0;};
    virtual void                reset() {};
    virtual void                setFontId(FontId fId);

protected:

            Param*              param;
            Device36*           dev;
            FontId              fontId;
            Image*              parentImage;
            int                 textHeight;
};

