
#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_window.h"
#include "36_button.h"

#include <vector>



class ParamObject : public Gobj
{
public:

            ParamObject();
            ~ParamObject();
    virtual void                addParam(Parameter* param);
            void                dequeueParamEnvelope(Trigger* tgenv);
            void                enqueueParamEnvelope(Trigger* tgenv);
            void                finalizePuts();
    virtual Parameter*          getParamByName(char *param_name);
    virtual Parameter*          getParamByIndex(int devIdx);
    virtual bool                getParamLock() { return paramLocked; };
    virtual std::list<Parameter*>       getParams() { return params; }
    virtual void                handleParamUpdate(Parameter* param = NULL) {};
    virtual void                handleClose();
            void                placeControls1(int maxW = -1, int maxH=-1);
            void                placeControls2(int maxW = -1, int maxH=-1);
            void                putStart(int x,int y);
            void                putRight(Gobj* obj, int ow=-1, int oh=-1);
            void                putBelow(Gobj* obj, int ow=-1, int oh=-1);
            void                returnLeft();
            void                returnUp();
    virtual void                removeParam(Parameter* param);
    virtual void                setParamLock(bool lock) { paramLocked = lock; };
            void                setInvokeButton(Button36* bt) { invokeButton = bt; }
            void                spaceRight(int num=1);
            void                spaceBelow(int num=1);
            void                setBorder(int brd)      { border = brd; }
            void                setObjSpacing(int os) { objSpacing = os; }
            void                setGroupSpacing(int gs) { groupSpacing = gs; }

protected:

            Trigger*            envelopes;
            Button36*           invokeButton;
            std::list<Parameter*>   params;

            int                 border;

            // for horizontal placement
            int                 xstart1;
            int                 ystart1;

            // for vertical placement
            int                 xstart2;
            int                 ystart2;

            int                 groupSpacing;
            int                 objSpacing;

            int                 wndW;
            int                 wndH;

private:

            bool                paramLocked;
};


