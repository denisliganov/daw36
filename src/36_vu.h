

#pragma once


#include "36_objects.h"

void Vu_ShowCoolFalldown();



class ChanVU : public Gobj
{
public:

            ChanVU(bool vert);
            ~ChanVU();

            void                setValues(float L, float R);
            void                tick();

protected:

            float               decrement;
            float               maxValue;
            bool                vertical;

            float               leftVal;
            float               rightVal;

            float               getL();
            float               getR();
            void                drawSelf(Graphics & g);
            void                handleMouseDown(InputEvent& ev);

};

class InstrVU : public ChanVU
{
protected:

            void                drawSelf(Graphics & g);
            void                handleMouseDown(InputEvent& ev);
public:

        InstrVU();
};

