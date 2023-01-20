
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
        InputEvent  event;


public:

        Scroller(bool is_vertical);
        void        updlimits(float full_span, float visible_span, float offs);
        void        setoffs(float offs);
        void        adjoffs(float delta);
        float       getoffs() { return offset; }
        float       getvisible() { return visibleSpan; }
        void        drawself(Graphics & g);
        int         getpos(InputEvent& ev, int& offset_on_bar);
        void        gotostart();
        void        gotoend();
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseWheel(InputEvent& ev);
        void        handleMouseDrag(InputEvent& ev);
        void        handleMouseUp(InputEvent & ev);
};



