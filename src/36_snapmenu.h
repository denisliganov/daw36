
#pragma once

#include "36_globals.h"
#include "36_menu.h"

#include <vector>



class SnapMenu : public Gobj
{
protected:

        float   snapSize;
        int     currentItem;

        std::vector<std::string>        vitems;

        void    drawSelf(Graphics& g);
        void    activateContextMenuItem(std::string item);
        void    handleMouseDown(InputEvent& ev);
        void    handleMouseUp(InputEvent & ev);
        void    handleMouseWheel(InputEvent & ev);

public:

        SnapMenu();

        float   getSnapSize();
        void    updateSnapSize();
};




