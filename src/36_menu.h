

#pragma once



#include "36_globals.h"
#include "36_objects.h"
#include "36_window.h"



class ContextMenuItem;

class ContextMenu : public WinObject
{
friend MainWindow;
friend DropBox;

protected:

       std::list<ContextMenuItem*>    items;

       Gobj*            menuObject;
       int              itemHeight;
       DropBox*         dropBox;

public:

        ContextMenu(Gobj* obj, DropBox* drop_box = NULL);

        void            addMenuItem( std::string item );
        void            remap();
        void            activatemenuitem(std::string item);
        void            drawself(Graphics & g);
        void            handleClose();
};



