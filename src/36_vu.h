

#pragma once


#include "36_objects.h"

void Vu_ShowCoolFalldown();



class ChanVU : public Gobj
{
protected:

        float       decrement;
        float       maxValue;
        bool        vertical;

        float       leftVal;
        float       rightVal;

        float       getL();
        float       getR();
        void        drawSelf(Graphics & g);

public:

        ChanVU(bool vert);
        ~ChanVU();

        void    setValues(float L, float R);
        void    tick();
};

class InstrVU : public ChanVU
{
protected:

        void    drawSelf(Graphics & g);

public:

        InstrVU();
};

