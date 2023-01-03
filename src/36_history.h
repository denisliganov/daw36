

#pragma once


#include "36_globals.h"

#include <list>




typedef enum HistActionType
{
    HistAction_None,
    HistAction_AddGroup,
    HistAction_DeleteGroup,
    HistAction_MoveGroup,
    HistAction_ParamChange,
    HistAction_Resize,
    HistAction_Resize2,
}HistActionType;

class HistoryAction
{
friend EditHistory;

protected:

        HistActionType  atype;

        void*       adata1;
        void*       adata2;
        void*       adata3;

        float       f1;
        float       f2;
        int         i1;
        int         i2;

        int         groupID;

        std::list<Element*> elems;

public:

        HistoryAction(int group, HistActionType at, void* ad1, float fl1, float fl2, int int1, int int2);
        HistoryAction(int group, HistActionType at, std::list <Element*> & lst, float fl1, int int1);
        ~HistoryAction();
};


class EditHistory
{
protected:

        int         currentGroup;

        std::list<HistoryAction*>              actions;
        std::list<HistoryAction*>::iterator    currAction;

        bool        checkActionUpdate(HistActionType atype, std::list <Element*> & lst, float f1, float f2, int i1, int i2);
        void        addAction(HistoryAction* a);
        void        removeAction(HistoryAction* a);
        void        wipeElementFromHistory(Element* el);
        void        wipeParameterFromHistory(Parameter* param);
        void        removeAndDeleteActionsForElement(std::list <Element*> & lst);
        void        perform(HistoryAction* act, bool forward = true);

public:

        EditHistory();

        void        newGroup();
        void        undo();
        void        redo();
        void        addNewAction(HistActionType atype, void* ptr1, float f1 = 0, float f2 = 0, int i1 = 0, int i2 = 0);
        void        addNewAction(HistActionType atype, std::list <Element*> & lst, float f1 = 0, int i1 = 0);
        void        flushActions();
        void        wipeEntireHistory();
};


