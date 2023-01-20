
#pragma once

#include "36_globals.h"
#include "36_window.h"
#include "36_draw.h"
#include "36_text.h"

#include <string>





class AlertBox : public WinObject
{
protected:

        std::string message;

        int         boxWidth;
        int         boxHeight;
        int         buttGap;
        int         outerGap;
        int         buttX;
        int         buttY;
        int         fontHeight;
        FontId      fontId;
        Button36*   buttPressed;

public:

        AlertBox( std::string message, std::string button1 = "", std::string button2 = "", std::string button3 = "");
        void            drawself(Graphics & g);
        void            addButton(std::string button);
        void            handleChildEvent(Gobj * obj,InputEvent & ev);
        std::string     getChoice();
};



