

#include "36_dropbox.h"
#include "36_menu.h"
#include "36_window.h"
#include "36_draw.h"
#include "36_text.h"




DropBox::DropBox(int menu_id, std::string name)
{
    menuId = menu_id;

    setObjName(name);

    fontId = FontInst;

    menu = NULL;

    width = gGetTextWidth(fontId, objName) + 20;
    height = gGetTextHeight(fontId) + 19;
}

void DropBox::addItem(std::string item)
{
    menuItems.push_back(item);
}

void DropBox::activateContextMenuItem(std::string item)
{
    parent->activateContextMenuItem(item);

    deactivate();

    redraw();
}

void DropBox::activate()
{
    parent->deactivateDropBoxById(menuId);

    if (menuItems.size() > 0)
    {
        menu = new ContextMenu(this, this);

        for(auto str : menuItems)
        {
            menu->addMenuItem(str);
        }
    
        MWindow->showMenuWindow(menu, x1, y2 + 1);
    }
}

void DropBox::deactivate()
{
    MWindow->deleteContextMenu();

    menu = NULL;
}

void DropBox::handleContextMenuClosed()
{
    if(parent->getWindow()->getActiveObj() != this) // Avoid reopening, when this DB already open
    {
        deactivate();

        redraw();
    }
}

void DropBox::handleMouseDown(InputEvent & ev)
{
    if (menu == NULL)
    {
        activate();
    }
    else
    {
        deactivate();
    }

    redraw();

    parent->handleChildEvent(this, ev);
}

void DropBox::handleMouseWheel(InputEvent & ev)
{
    //
}

void DropBox::handleMouseUp(InputEvent& ev)
{
    //
}

void DropBox::drawself(Graphics& g)
{
    if (mouseHovering)
    {
        fill(g, 0.6f);
    }
    else
    {
        fill(g, 0.3f);
    }

    rect(g, 0.2f);

    setc(g, 1.f);

    gText(g, fontId, objName, x1 + width/2 - gGetTextWidth(fontId, objName)/2, y2 - height/2 + gGetTextHeight(fontId)/2 - 1);
}



