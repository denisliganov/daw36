
#pragma once


#include "36_globals.h"
#include "36_controls.h"





class Scroller : public Control
{
public:

        Scroller(bool is_vertical);
        void                adjOffs(float delta);
        void                drawSelf(Graphics & g);
        float               getoffs() { return offset; }
        float               getvisible() { return visiblepart; }
        int                 getPos(InputEvent& ev, int& offset_on_bar);
        void                gotoStart();
        void                gotoEnd();
        void                handleMouseDown(InputEvent& ev);
        void                handleMouseWheel(InputEvent& ev);
        void                handleMouseDrag(InputEvent& ev);
        void                handleMouseUp(InputEvent & ev);
        void                setOffs(float offs);
        void                updBounds(float full_span, float visible_span, float offs);

protected:

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


