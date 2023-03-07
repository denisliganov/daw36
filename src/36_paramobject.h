
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

    virtual void                addParam(Param* param);
            void                dequeueParamEnvelope(Trigger* tgenv);
            void                enqueueParamEnvelope(Trigger* tgenv);
            void                finalizePuts();
    virtual Param*              getParamByName(char *param_name);
    virtual Param*              getParamByIndex(int devIdx);
    virtual bool                getParamLock() { return paramLocked; };
    virtual std::list<Param*>   getParams() { return params; }
            void                goLeft();
            void                goTop();
    virtual void                handleParamUpdate(Param* param = NULL) {};
    virtual void                handleClose();
            void                putStart(int x,int y);
            void                putRight(Gobj* obj, int ow=-1, int oh=-1);
            void                putBelow(Gobj* obj, int ow=-1, int oh=-1);
    virtual void                removeParam(Param* param);
    virtual void                setParamLock(bool lock) { paramLocked = lock; };
            void                setInvokeButton(Button36* bt) { invokeButton = bt; }
            void                spaceRight(int num=1);
            void                spaceBelow();
            void                setBorder(int brd)      { border = brd; }
            void                setObjSpacing(int os) { objSpacing = os; }
            void                setGroupSpacing(int gs) { groupSpacing = gs; }

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


