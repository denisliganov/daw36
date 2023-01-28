
#pragma once

#include "36_globals.h"
#include "36_objects.h"



class Playhead : public Timer, public Gobj
{
friend Transport;

protected:

            int         absX;
            int         relX;
            int         posX;
            int         add;
            Grid*       grid;
            Pattern*    patt;

            void        timerCallback();
            void        drawSelf(Graphics& g);

public:

            Playhead(Grid* grd);
            void        restartTimer(int interval = 30);
            void        reset();
            bool        handleAutomationRecording(float currStep);
            void        updatePosFromFrame();
            void        updateFrameFromPos();
            int         getPosX()   { return posX; }
};

