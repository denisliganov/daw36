


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
#define MenuItemsGap        6
#define MenuItemIndent      28
#define MenuWidth           100



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

            fontId = FontInst;

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

        void drawself(Graphics& g)
        {
            if(itemstr == "")   //  delimiter
            {
                gSetMonoColor(g, .55f);
                gLineHorizontal(g, y1 + height/2 - 1, float(x1 + MenuItemIndent), float(x1 + width - 1));

                //gSetMonoColor(g, .6f);
                //gLineHorizontal(g, y1 + height/2, float(x1 + 5), float(x1 + width - 1 - 5));
            }
            else
            {
                if(undermouse)
                {
                    fill(g, 0.6f);
                }
                else
                {
                    fill(g, 0.5f);
                }

                setc(g, 1.f);

                gText(g, fontId, itemstr, x1 + MenuItemIndent, y1 + height/2 + th/2 - 2);
            }
        }

        void handleMouseEnter(InputEvent & ev)  { redraw(); }
        void handleMouseLeave(InputEvent & ev)  { redraw(); }
        void handleMouseDown(InputEvent& ev)
        {
            parent->activatemenuitem(itemstr);
        }
};

ContextMenu::ContextMenu(Gobj* obj, DropBox* drop_box)
{
    menuObject = obj;
    dropBox = drop_box;

    width = 30;
    itemHeight = MenuItemHeight;
}

void ContextMenu::handleClose()
{
    if (dropBox != NULL)
    {
        dropBox->handleContextMenuClosed();
    }
}

void ContextMenu::addMenuItem( std::string item_str )
{
    ContextMenuItem* mi = new ContextMenuItem(item_str);

    addObject(mi);

    items.push_back(mi);

    int iw = gGetTextWidth(FontBold, item_str) + 150 + MenuItemIndent;

    if(iw > width)
    {
        width = iw;
    }

    int mHeight = MenuItemsGap*2 - 3;

    for(ContextMenuItem* item : items)
    {
        mHeight += item->getH();
    }

    setCoords1(0, 0, width, mHeight);
}

void ContextMenu::remap()
{
    int itemY = MenuItemsGap;

    for(ContextMenuItem* item : items)
    {
        item->setCoords1(1, itemY, getW() - 2, item->getH());

        itemY += item->getH();
    }

    Component::setBounds(Component::getX(), Component::getY(), width, height);
}

void ContextMenu::activatemenuitem(std::string item)
{
    menuObject->activatemenuitem(item);
}

void ContextMenu::drawself(Graphics & g)
{
    fill(g, .5f);
}



