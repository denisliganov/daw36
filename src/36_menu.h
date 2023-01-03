

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
        void            mapObjects();
        void            activateContextMenuItem(std::string item);
        void            drawSelf(Graphics & g);
        void            handleClose();
};



