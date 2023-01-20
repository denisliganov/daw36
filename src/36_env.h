
#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_element.h"
#include "36_config.h"

#include <list>


// Envelopes
#define ENVPOINT_TOUCH_RADIUS       (5)


typedef enum EnvAction
{
    ENVUSUAL,       // Usual hovering
    ENVPOINTING,    // Mouse on point (moving or hovering)
    ENVDELETING,    // Points deletion
    ENVDRAWMOVE,    // Freehand draw
    ENVLINEMOVE     // Freehand line draw
}EnvAction;


class EnvPoint : public Gobj
{
public:

        float       tick;
        float       y_norm;
        bool        deleted;
        bool        big;

        EnvPoint() {}
};

typedef enum EnvType
{
    CmdEnv_VolEnv,
    CmdEnv_PanEnv,
    CmdEnv_ParamEnv,
}CEnvType;

class Envelope : public Element
{
public:

            std::list  <EnvPoint*>  points;

            EnvType     envType;
            Parameter*  param;
            float       lastValue;
            int         aaCount;
            float       aaBaseValue;
            long        last_buffframe;
            long        last_buffframe_end;
            EnvPoint*   activePoint;
            EnvAction   envAction;
            float       env_xstart;
            bool        verticalDragOnly;
            float       buff[MAX_BUFF_SIZE];
            float       buffoutval[MAX_BUFF_SIZE];


            Envelope(EnvType ct);

            Envelope*   clone(Element* el);
            Envelope*   clone();
    virtual bool        processBuffer1(long curr_frame, long buffframe, long num_frames, int step, long start_frame, Trigger* tgenv = NULL, Trigger* tgi = NULL);
            EnvPoint*   addPoint(float x, float y_norm, bool big = true);
            void        placePoint(EnvPoint* p);
            float       getValue(long frame);
            float       getValue(float tick);
            void        removePoint(EnvPoint* p);
            void        deletePoint(EnvPoint* p);
            void        calcTime();
            void        stopBuffer();
            void        updateLastValue();
            void        deleteRange(float x_curr, float x_start, bool start_except);
            void        drag(int mouse_x, int mouse_y, unsigned flags);
            void        check(int mouse_x, int mouse_y);
            void        handleMouseDown(InputEvent& ev);
            bool        isOutOfBounds(long frame_phase);
            long        getFramesRemaining(long frame_phase);
            void        setTickLength(float newlen);
            void        markDeleted(bool del);
            void        drwongrid(Graphics & g,Grid * grid);
};


