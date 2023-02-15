

#include "36_history.h"
#include "36_grid.h"
#include "36_edit.h"
#include "36_paramnum.h"
#include "36_element.h"
#include "36_project.h"
#include "36_note.h"
#include "36_instrpanel.h"
#include "36_instr.h"




HistoryAction::HistoryAction(int group, HistActionType at, void * ad1, float fl1, float fl2, int int1, int int2)
{
    groupID = group;
    atype = at;
    adata1 = ad1;
    f1 = fl1;
    f2 = fl2;
    i1 = int1;
    i2 = int2;
}


HistoryAction::HistoryAction(int group, HistActionType at, std::list <Element*> & lst, float fl1, int int1)
{
    groupID = group;
    atype = at;
    f1 = fl1;
    i1 = int1;

    for(Element* el : lst)
    {
        elems.push_back(el);
    }
}

HistoryAction::~HistoryAction()
{
    while(elems.size() > 0)
    {
        elems.pop_front();
    }
}


EditHistory::EditHistory()
{
    currentGroup = 0;

    currAction = actions.begin();

    addNewAction(HistAction_None, NULL, NULL, NULL);
}

void EditHistory::newGroup()
{
    currentGroup++;
}

// Before deleting an element, remove all actions involving this element
//
void EditHistory::removeAndDeleteActionsForElement(std::list <Element*> & lst)
{
    for(Element* el : lst)
    {
        MGrid->clipboard.remove(el);
start:
        for(HistoryAction* action : actions)
        {
            if(action->atype != HistAction_ParamChange)
            {
                Element* el1 = (Element*)action->adata1;

                if(el1 == el)
                {
                    actions.remove(action);

                    goto start;
                }
            }
        }

        delete el;
    }
}

// Some actions, like Move, can be just updated with new params, instead of creating new action
//
// Return: Whether action was updated or not
//
bool EditHistory::checkActionUpdate(HistActionType atype, std::list <Element*> & lst, float f1, float f2, int i1, int i2)
{
    if(atype == HistAction_MoveGroup)
    {
        std::list<HistoryAction*>::iterator itSearch = currAction;

        do
        {
            HistoryAction* act = *itSearch;

            if(act->atype == atype && act->elems == lst && act->groupID == currentGroup)
            {
                act->f1 += f1;
                act->i1 += i1;

                for(Element* el : act->elems)
                {
                    el->move(f1, i1);
                }

                return true;
            }

            itSearch--;
        } while(itSearch != actions.begin() && ((HistoryAction*)*itSearch)->groupID == currentGroup);
    }

    return false;
}

void EditHistory::addNewAction(HistActionType atype, void* adata1, float f1, float f2, int i1, int i2)
{
    flushActions();

    //if(checkActionUpdate(atype, adata1, f1, f2, i1, i2) == false)
    {
        addAction(new HistoryAction(currentGroup, atype, adata1, f1, f2, i1, i2));

        perform(*currAction);
    }
}

void EditHistory::addNewAction(HistActionType atype, std::list <Element*> & lst, float f1, int i1)
{
    flushActions();

    if(checkActionUpdate(atype, lst, f1, 0, i1, 0) == false)
    {
        addAction(new HistoryAction(currentGroup, atype, lst, f1, i1));

        perform(*currAction);
    }
}

void EditHistory::redo()
{
    std::list<HistoryAction*>::iterator itRedo = currAction;

    itRedo++;

    if(itRedo != actions.end())
    {
        MGrid->selReset();

        int id = (*itRedo)->groupID;

        while(itRedo != actions.end() && (*itRedo)->groupID == id)
        {
            currAction = itRedo;

            perform(*currAction);

            itRedo++;
        }

        MProject.setChange();
    }
}

void EditHistory::undo()
{
    if(currAction != actions.end() && currAction != actions.begin())
    {
        MGrid->selReset();

        int id = (*currAction)->groupID;

        while(currAction != actions.begin() && (*currAction)->groupID == id)
        {
            perform(*currAction, false);

            currAction--;
        }

        MProject.setChange();
    }
}

void EditHistory::flushActions()
{
    if(currAction != actions.end())
    {
start:
        std::list<HistoryAction*>::iterator itflush = currAction;

        itflush++;

        while(itflush != actions.end())
        {
            HistoryAction* act = *itflush;

            itflush++;

            if(act->atype == HistAction_AddGroup)
            {
                removeAndDeleteActionsForElement(act->elems);
            }

            removeAction(act);

            goto start;
        }
    }
}

void EditHistory::perform(HistoryAction* act, bool forward)
{
    switch(act->atype)
    {
        case HistAction_DeleteGroup:
        {
            for(Element* el : act->elems)
            {
                if(forward)
                {
                    //jassert(el->deleted == false);

                    el->softdel();
                }
                else
                {
                    el->markDeleted(false);
                }
            }
        }break;

        case HistAction_AddGroup:
        {
            for(Element* el : act->elems)
            {
                if(forward)
                {
                    el->markDeleted(false);
                }
                else
                {
                    el->softdel();
                }
            }
        }break;

        case HistAction_MoveGroup:
        {
            float dtick = act->f1;
            int dline = act->i1;

            for(Element* el : act->elems)
            {
                if(forward)
                {
                    el->move(dtick, dline);
                }
                else
                {
                    el->move(-dtick, -dline);
                }
            }
        }break;

        case HistAction_Resize:
        {
            Element* el = (Element*)act->adata1;

            if(forward)
            {
                el->setTickLength(act->f2);
            }
            else
            {
                el->setTickLength(act->f1);
            }
        }break;

        case HistAction_Resize2:
        {
            for(Element* el : act->elems)
            {
                if(forward)
                {
                    el->setTickDelta(act->f1);
                }
                else
                {
                    el->setTickDelta(-act->f1);
                }
            }
        }break;
/*
        case HistAction_Resize:
        {
            for(Element* el : act->elems)
            {
                if(forward)
                {
                    el->setTickLength(act->f2);
                }
                else
                {
                    el->setTickLength(act->f1);
                }
            }

            MGrid->redraw(true);
        }break;
*/
        case HistAction_ParamChange:
        {
            Parameter* param = (Parameter*)act->adata1;

            if(forward)
            {
                float val2 = act->f2;

                param->setValue(val2);
            }
            else
            {
                float val1 = act->f1;

                param->setValue(val1);
            }
        }break;
    }
}

void EditHistory::addAction(HistoryAction* a)
{
    actions.push_back(a);

    currAction = actions.end();
    currAction--;
}

void EditHistory::removeAction(HistoryAction* a)
{
    if(*currAction == a)
    {
        currAction++;

        if (currAction == actions.end())
        {
            currAction--;

            if (currAction != actions.begin())
            {
                currAction--;
            }
        }
    }

    actions.remove(a);

    delete a;
}

void EditHistory::wipeEntireHistory()
{
    while(actions.size() > 0)
    {
        removeAction(actions.front());
    }

    // Reinitialize

    currentGroup = 0;
    addNewAction(HistAction_None, NULL, NULL, NULL);
    currAction = actions.begin();
}

void EditHistory::wipeParameterFromHistory(Parameter* param)
{
again:

    for(HistoryAction* act : actions)
    {
        switch(act->atype)
        {
            case HistAction_ParamChange:
            {
                Parameter* prm = (Parameter*)act->adata1;

                if(prm == param)
                {
                    removeAction(act);

                    goto again;
                }

            }break;
        }
    }
}


