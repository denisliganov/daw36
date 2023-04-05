
#pragma once

#include "36_globals.h"
#include "36_controls.h"




class Slider36 : public Control
{
friend Parameter;

public:

            Slider36() {};
            Slider36(bool vert);

    virtual void                setParam(Parameter* p) {prm = p;}
            std::string         getHint();

protected:

            bool                vertical;
            int                 slLen;
            int                 slstep;
            int                 scale;
            int                 length;
            float               slMul;
            Parameter*          prm;

    virtual void                updValue();
    virtual void                updPosition();
    virtual void                handleMouseDrag(InputEvent& ev);
    virtual void                handleMouseDown(InputEvent& ev);
            void                handleMouseEnter() { redraw(); };
            void                handleMouseLeave() { redraw(); };
            void                handleMouseWheel(InputEvent& ev);
            void                remap();
            void                drawSelf(Graphics& g);

};

