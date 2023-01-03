
#pragma once


#include "36_globals.h"
#include "36_controls.h"





class Scroller : public Control
{
protected:

        bool        vertical;
        bool        active;
        int*        coordRef;
        float       fullSpan;
        float       visibleSpan;
        float       ratio;
        float       offset;
        int         wheelMult;
        int         barPixLen;
        int         offsetPix;
        int         fullPixLen;
        int         currPos;
        int         barPos;


public:

        Scroller(bool is_vertical);
        void        updateLimits(float full_span, float visible_span, float offs);
        void        setOffset(float offs);
        float       getOffset() { return offset; }
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseWheel(InputEvent& ev);
        void        handleMouseDrag(InputEvent& ev);
        void        handleMouseUp(InputEvent & ev);
        void        drawSelf(Graphics & g);
        int         getPosition(InputEvent& ev, int& offset_on_bar);
        void        goToStart();
        void        goToEnd();
};



