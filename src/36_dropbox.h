

#pragma once

#include "36_globals.h"
#include "36_objects.h"
#include "36_text.h"

#include <vector>



class DropBox : public Gobj
{
friend ControlPanel;

public:

            DropBox(int menu_id, std::string name);
            void                addItem(std::string item);
            void                activate();
            void                deactivate();
            int                 getMenuId() { return menuId; }
            void                handleContextMenuClosed();

protected:

            FontId              fontId;
            int                 menuId;
            ContextMenu*        menu;

            std::vector<std::string>    menuItems;

            void                activateMenuItem(std::string item);
            void                drawSelf(Graphics& g);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent & ev);
            void                handleMouseWheel(InputEvent & ev);
            void                handleMouseEnter(InputEvent & ev)  { redraw(); }
            void                handleMouseLeave(InputEvent & ev)  { redraw(); }

};


