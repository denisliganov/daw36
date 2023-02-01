


#include "36_globals.h"
#include "36_objects.h"
#include "36_menu.h"
#include "36_controls.h"
#include "36_draw.h"
#include "36.h"
#include "36_text.h"
#include "36_config.h"
#include "36_dropbox.h"




#define MenuItemColor       0xffE0F0FF

#define MenuItemHeight      17
#define MenuWidth           80

#define MenuVertSpacing       12
#define MenuHorizSpacing      23



class ContextMenuItem : public Gobj
{
private:

        std::string     itemstr;

        int             tw;
        int             th;

        FontId          fontId;

public:

        ContextMenuItem(std::string str)
        {
            itemstr = str;

            fontId = FontBold;

            tw = gGetTextWidth(fontId, itemstr);
            th = gGetTextHeight(fontId);

            if(str.size() > 0)
            {
                height = MenuItemHeight;
            }
            else
            {
                height = MenuItemHeight/1.5f;
            }
        }

        void drawSelf(Graphics& g)
        {
            if(itemstr == "")   //  delimiter
            {
                setc(g, .55f);
                lineH(g, height/2 - 1, float(MenuHorizSpacing), float(width - 1));
            }
            else
            {
                if(undermouse)
                {
                    fill(g, 0.5f);
                }
                else
                {
                    fill(g, 0.4f);
                }

                setc(g, 1.f);

                txt(g, fontId, itemstr, MenuHorizSpacing, height/2 + th/2 - 2);
            }
        }

        void handleMouseEnter(InputEvent & ev)  { redraw(); }
        void handleMouseLeave(InputEvent & ev)  { redraw(); }
        void handleMouseDown(InputEvent& ev)
        {
            parent->activateMenuItem(itemstr);
        }
};

ContextMenu::ContextMenu(Gobj* obj, DropBox* drop_box)
{
    menuObject = obj;
    dropBox = drop_box;

    width = 30;
    itemHeight = MenuItemHeight;
}

void ContextMenu::addMenuItem( std::string item_str )
{
    ContextMenuItem* mi = new ContextMenuItem(item_str);

    addObject(mi);

    items.push_back(mi);

    int iw = gGetTextWidth(FontBold, item_str) + 150 + MenuHorizSpacing;

    if(iw > width)
    {
        width = iw;
    }

    int mHeight = MenuVertSpacing*2;

    for(ContextMenuItem* item : items)
    {
        mHeight += item->getH();
    }

    setCoords1(0, 0, width, mHeight);
}

void ContextMenu::drawSelf(Graphics & g)
{
    fill(g, .4f);
}

void ContextMenu::handleClose()
{
    if (dropBox != NULL)
    {
        dropBox->handleContextMenuClosed();
    }
}

void ContextMenu::remap()
{
    int itemY = MenuVertSpacing;

    for(ContextMenuItem* item : items)
    {
        item->setCoords1(1, itemY, getW() - 2, item->getH());

        itemY += item->getH();
    }

    Component::setBounds(Component::getX(), Component::getY(), width, height);
}

void ContextMenu::activateMenuItem(std::string item)
{
    menuObject->activateMenuItem(item);
}



