

#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_text.h"

#include <vector>



class DropBox : public Gobj
{
friend ControlPanel;

protected:

        FontId          fontId;
        int             menuId;
        ContextMenu*    menu;

        std::vector<std::string>    menuItems;

        void        drawSelf(Graphics& g);
        void        activateContextMenuItem(std::string item);
        void        handleMouseDown(InputEvent& ev);
        void        handleMouseUp(InputEvent & ev);
        void        handleMouseWheel(InputEvent & ev);
        void        handleMouseEnter(InputEvent & ev)  { redraw(); }
        void        handleMouseLeave(InputEvent & ev)  { redraw(); }

public:

        DropBox(int menu_id, std::string name);
        void        addItem(std::string item);
        void        handleContextMenuClosed();
        void        activate();
        void        deactivate();
        int         getMenuId() { return menuId; }
};


