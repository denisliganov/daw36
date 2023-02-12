
#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_window.h"
#include "36_button.h"

#include <vector>



class ParamObject : public WinObject
{
public:

            ParamObject();

    virtual void                addParam(Param* param);
    virtual void                addParamWithControl(Param* param, std::string ctrl_name = "", Control* ctrl = NULL);
            void                dequeueParamEnvelope(Trigger* tgenv);
            void                enqueueParamEnvelope(Trigger* tgenv);
    virtual Param*              getParamByName(char *param_name);
    virtual Param*              getParamByIndex(int devIdx);
    virtual bool                getParamLock() { return paramLocked; };
    virtual std::list<Param*>   getParams() { return params; }
    virtual void                handleParamUpdate(Param* param = NULL) {};
    virtual void                removeParam(Param* param);
    virtual void                setParamLock(bool lock) { paramLocked = lock; };
            void                setInvokeButton(Button36* bt) { invokeButton = bt; }
    virtual void                handleClose();
            void                putStart(int x,int y);
            void                putRight(Gobj* obj, int ow=-1, int oh=-1);
            void                putBelow(Gobj* obj, int ow=-1, int oh=-1);
            void                spaceRight();
            void                spaceBelow();
            void                setBorder(int brd)      { border = brd; }
            void                goLeft();
            void                goTop();
            void                finalizePuts();

protected:

            Trigger*            envelopes;
            Button36*           invokeButton;
            std::list<Param*>   params;

            int                 border;
            int                 xstart1;
            int                 ystart1;

            int                 xstart2;
            int                 ystart2;

            int                 groupSpacing;
            int                 objSpacing;

            int                 wndW;
            int                 wndH;

private:

            bool                paramLocked;
};


