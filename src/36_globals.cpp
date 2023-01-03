
#include "36_globals.h"





template <typename Type> Type marax(Type a, Type b) 
{
    return a > b ? a : b;
}

template <class ObjClass> class LList
{
    ObjClass* first;
    ObjClass* last;
    ObjClass* curr;
    int num;

    LList()
    {
        first = last = NULL;
        curr = NULL:
        num = 0;
    }

    ObjClass* Begin()
    {
        curr = first;

        return curr;
    }

    ObjClass* GetNext()
    {
        curr = curr->next;

        return curr;
    }
};



