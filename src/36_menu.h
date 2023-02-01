

#pragma once



#include "36_globals.h"
#include "36_objects.h"
#include "36_window.h"



class ContextMenuItem;

class ContextMenu : public WinObject
{
friend MainWindow;
friend DropBox;

public:

        ContextMenu(Gobj* obj, DropBox* drop_box = NULL);

        void                addMenuItem( std::string item );
        void                remap();
        void                activateMenuItem(std::string item);

protected:

        Gobj*               menuObject;
        int                 itemHeight;
        DropBox*            dropBox;

        std::list<ContextMenuItem*>    items;

        void                drawSelf(Graphics & g);
        void                handleClose();
};



