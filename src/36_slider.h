
#pragma once

#include "36_globals.h"
#include "36_controls.h"




class Slider36 : public Control
{
friend Parameter;

protected:

            bool    vertical;
            int     slLen;
            int     slstep;
            int     scale;
            int     length;
            float   slMul;

    virtual void    updValue();
    virtual void    updPosition();
    virtual void    handleMouseDrag(InputEvent& ev);
    virtual void    handleMouseDown(InputEvent& ev);
            void    handleMouseEnter() { redraw(); };
            void    handleMouseLeave() { redraw(); };
            void    handleMouseWheel(InputEvent& ev);
            void    remap();
            void    drawSelf(Graphics& g);

public:

            Slider36() {};
            Slider36(bool vert);
std::string getHint();
};

