

#include "36_snapmenu.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_transport.h"
#include "36_text.h"
#include "36_macros.h"





SnapMenu::SnapMenu()
{
    vitems.push_back("Bar");
    vitems.push_back("Beat");
    vitems.push_back("Tick");
    vitems.push_back("Tick/2");
    vitems.push_back("Tick/3");
    vitems.push_back("Tick/4");
    vitems.push_back("Tick/6");
    vitems.push_back("(None)");

    width = 50;

    currentItem = 1;

    updateSnapSize();
}

float SnapMenu::getSnapSize()
{
    return snapSize;
}

void SnapMenu::updateSnapSize()
{
    if(currentItem == 0)
        snapSize = (float)MTransp->getTicksPerBar();
    else if (currentItem == 1)
        snapSize = (float)MTransp->getTicksPerBeat();
    else if (currentItem == 2)
        snapSize = 1;
    else if (currentItem == 3)
        snapSize = 0.5f;
    else if (currentItem == 4)
        snapSize = 1.f/3.f;
    else if (currentItem == 5)
        snapSize = 0.25f;
    else if (currentItem == 6)
        snapSize = 1.f/6.f;
}

void SnapMenu::activateContextMenuItem(std::string item)
{
    int idx = 0;

    for(auto str : vitems)
    {
        if (str == item)
        {
            currentItem = idx;
            break;
        }
        idx++;
    }

    updateSnapSize();

    redraw();
}

void SnapMenu::handleMouseDown(InputEvent & ev)
{
    ContextMenu* menu = new ContextMenu(this);

    for(auto str : vitems)
    {
        menu->addMenuItem(str);
    }

    MWindow->showMenuWindow(menu, x1, y2 + 2);
}

void SnapMenu::handleMouseWheel(InputEvent & ev)
{
    currentItem -= ev.wheelDelta;

    LIMIT(currentItem, 0, vitems.size() - 1);

    updateSnapSize();

    redraw();
}

void SnapMenu::handleMouseUp(InputEvent& ev)
{
    ///
}

void SnapMenu::drawSelf(Graphics& g)
{
    fillWithMonoColor(.4f);

    //gSetMonoColor(g, 0.45f);
    gSetMonoColor(g, 0.22f);
    gDrawRect(g, x1, y1, x2, y2);

    //gSetMonoColor(g, 0.7f);
    //gText(g, FontInst, "Snap", x1 + 2, y1 + 9);

    gSetMonoColor(g, 1);

    gText(g, FontBig, vitems[currentItem], x1 + width/2 - gGetTextWidth(FontBig, vitems[currentItem])/2, y2 - height/2 + 7);
}



