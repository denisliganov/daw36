
#pragma once


#include "36_globals.h"
#include "36_controls.h"





class Scroller : public Control
{
protected:

        bool        vertical;
        bool        active;
        int*        coordref;
        float       fullspan;
        float       visiblepart;
        float       ratio;
        float       offset;

        int         barpixlen;
        int         offsetpix;
        int         fullpixlen;
        int         currpos;
        int         barpos;
        InputEvent  event;


public:

        Scroller(bool is_vertical);
        void        updlimits(float full_span, float visible_span, float offs);
        void        setoffs(float offs);
        void        adjoffs(float delta);
        float       getoffs() { return offset; }
        float       getvisible() { return visiblepart; }
        void        drawSelf(Graphics & g);
        int         getpos(InputEvent& ev, int& offset_on_bar);
        void        gotostart();
        void        gotoend();
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseWheel(InputEvent& ev);
        void        handleMouseDrag(InputEvent& ev);
        void        handleMouseUp(InputEvent & ev);
};

class Scrolled : public Gobj
{
public:

        Scroller*           vscr;
        Scroller*           hscr;

        Scrolled();
        void                setHoffs(float offs);
        float               getHoffs();
        void                setVoffs(float offs);
        float               getVoffs();

};


