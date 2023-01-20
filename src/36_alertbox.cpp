

#include "36_alertbox.h"
#include "36_draw.h"
#include "36_button.h"
#include "36_config.h"




AlertBox::AlertBox( std::string msg, std::string bttxt1, std::string bttxt2, std::string bttxt3 )
{
    message = msg;

    fontId = FontInst;
    fontHeight = gGetTextHeight(fontId);

    buttGap = 10;
    outerGap = 50;

    int buttWidth = 0;

    if(bttxt1.size() > 0)
        buttWidth += gGetTextWidth(fontId, bttxt1) + buttGap*4 + buttGap*2;

    if(bttxt2.size() > 0)
        buttWidth += gGetTextWidth(fontId, bttxt2) + buttGap*4 + buttGap*2;

    if(bttxt3.size() > 0)
        buttWidth += gGetTextWidth(fontId, bttxt3) + buttGap*4 + buttGap*2;

    buttWidth -= buttGap*2;

    boxWidth = gGetTextWidth(fontId, message);

    if(buttWidth > boxWidth)
        boxWidth = buttWidth;

    boxWidth += outerGap*2;
    boxHeight = outerGap*3;

    buttX = boxWidth/2 - buttWidth/2;
    buttY = (boxHeight/3)*2 - fontHeight/2 - buttGap;


    addButton(bttxt1);
    addButton(bttxt2);
    addButton(bttxt3);

    setWidthHeight(boxWidth, boxHeight);
}

void AlertBox::addButton( std::string buttext )
{
    if(buttext.size() > 0)
    {
        int buttW = buttGap*4 + gGetTextWidth(fontId, buttext);

        addObject(new Button36(false, buttext), buttX, buttY, buttW, buttGap*2 + fontHeight, "");

        buttX += buttW + buttGap*2;
    }
}

void AlertBox::drawself(Graphics & g)
{
    fill(g, .5f);

    rect(g, 0.55f);

    setc(g, 0.99f);

    gText(g, fontId, message, width/2 - gGetTextWidth(fontId, message)/2, outerGap);
}

void AlertBox::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    buttPressed = dynamic_cast<Button36*>(obj);

    if(!ev.clickDown)
        holderWindow->exitModalState(0);
}

std::string AlertBox::getChoice()
{
    if(buttPressed)
    {
        return buttPressed->getObjName();
    }

    return "";
}


