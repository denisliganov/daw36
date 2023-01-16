

#pragma once

#include "36_globals.h"
#include "36_objects.h"




class Control : public Gobj
{
protected:

            Parameter*          param;
            Device36*           dev;
            Image*              parentImage;

            virtual void        reset() {};

public:

            Control();
            virtual void        addParam(Parameter* p);
            virtual void        updPosition() {};
            virtual void        updValue() {};
            virtual float       getMinStep() {return 0;};
};

