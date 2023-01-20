
#pragma once

#include "36_globals.h"
#include "36_controls.h"



Button36* GetButton(Gobj* parent, std::string name);



class Button36 : public Control
{
protected:

            bool            pressed;
            bool            sticking;
            bool            isLedType;

            void            updPosition();
            void            drawButtonBase(Graphics& g);
            void            drawself(Graphics& g);
            void            handleMouseWheel(InputEvent& ev);

public:

            Button36(bool stick, std::string title = "");
            bool            isPressed();
    virtual void            press();
    virtual void            release();
    virtual void            process(bool down);
            void            toggle();
            void            addParam(Parameter* prm);
            void            setLedType(bool is_led) { isLedType = is_led; }
    virtual void            handleMouseDown(InputEvent& ev);
    virtual void            handleMouseUp(InputEvent& ev);
};


class GroupButton : public Button36
{
protected:

            int             groupId;
            GroupButton*    lastPressed;

            int             getGroupId() { return groupId; }

public:
            GroupButton(int group_id);
    virtual void            handleMouseDown(InputEvent& ev);
    virtual void            handleMouseUp(InputEvent& ev);
            void            revertToPrevious();
};



