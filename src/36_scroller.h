
#pragma once


#include "36_globals.h"
#include "36_controls.h"



class Scroller : public Control
{
public:

        Scroller(bool is_vertical);
        void                adjustOffset(float delta);
        float               getOffset() { return offset; }
        float               getVisiblePart() { return visiblepart; }
        void                gotoStart();
        void                gotoEnd();
        void                setOffset(float offs);
        void                updBounds(float full_span, float visible_span, float offs);

protected:

        void                drawSelf(Graphics & g);
        int                 getPos(InputEvent& ev, int& offset_on_bar);
        void                handleMouseDown(InputEvent& ev);
        void                handleMouseWheel(InputEvent& ev);
        void                handleMouseDrag(InputEvent& ev);
        void                handleMouseUp(InputEvent & ev);

        bool                active;
        int                 barpixlen;
        int                 barpos;
        int                 currpos;
        int*                coordref;
        InputEvent          event;
        float               fullspan;
        int                 fullpixlen;
        float               ratio;
        float               offset;
        int                 offsetpix;
        bool                vertical;
        float               visiblepart;
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


