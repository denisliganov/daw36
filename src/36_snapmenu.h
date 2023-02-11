
#pragma once

#include "36_globals.h"
#include "36_menu.h"

#include <vector>



class SnapMenu : public Gobj
{
public:

        SnapMenu();

        float               getSnapSize();
        void                updateSnapSize();

protected:

        float               snapSize;
        int                 currentItem;

        std::vector<std::string>        vitems;

        void                activateMenuItem(std::string item);
        void                drawSelf(Graphics& g);
        void                handleMouseDown(InputEvent& ev);
        void                handleMouseUp(InputEvent & ev);
        void                handleMouseWheel(InputEvent & ev);
};




