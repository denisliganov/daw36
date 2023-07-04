
#include "36_makeup.h"
#include "36_draw.h"
#include "36.h"
#include "36_grid.h"




MakeUpObject::MakeUpObject()
{
    setWH(500, 400);
}

void MakeUpObject::drawSelf(Graphics& g)
{
    fill(g, 0.2f);

    Colour clr;

    clr = Colour(155, 58, 94);
    drawGlassRect(g, (float)10, (float)10, 40, 40, clr, 1, 2, true, true, false, false);

    clr = Colour(155, 155, 155);
    drawGlassRect(g, (float)80, (float)10, 20, 15, clr, 1, 2, true, true, false, false);

    clr = Colour(155, 200, 155);
    drawGlassRect(g, (float)110, (float)10, 30, 30, clr, 1, 0, false, false, false, false);

    clr = Colour(200, 200, 200);
    drawGlassRect(g, (float)180, (float)10, 50, 40, clr, 1, 8, false, false, false, false);

    clr = Colour(100, 110, 110);
    drawGlassRect(g, (float)260, (float)10, 40, 40, clr, 1, 8, false, false, false, false);

    clr = Colour(180, 80, 80);
    drawGlassRect(g, (float)310, (float)10, 50, 60, clr, 1, 8, false, false, false, false);

    clr = Colour(180, 80, 80);
    drawGlassRect(g, (float)390, (float)10, 50, 60, clr, 1, 8, true, true, true, true);



    clr = Colour(155, 58, 94);
    drawGlassRect1(g, (float)10, (float)80, 40, 40, clr, 1, 2, true, true, false, false);

    clr = Colour(155, 155, 155);
    drawGlassRect1(g, (float)80, (float)80, 20, 15, clr, 1, 2, true, true, false, false);

    clr = Colour(155, 200, 155);
    drawGlassRect1(g, (float)110, (float)80, 30, 30, clr, 1, 0, false, false, false, false);

    clr = Colour(200, 200, 200);
    drawGlassRect1(g, (float)180, (float)80, 50, 40, clr, 1, 8, false, false, false, false);

    clr = Colour(100, 110, 110);
    drawGlassRect1(g, (float)260, (float)80, 40, 40, clr, 1, 8, false, false, false, false);

    clr = Colour(180, 80, 80);
    drawGlassRect1(g, (float)310, (float)80, 50, 60, clr, 0, 8, false, false, false, false);

    clr = Colour(180, 80, 80);
    drawGlassRect1(g, (float)390, (float)80, 50, 60, clr, 0, 0, true, true, true, true);


    clr = Colour(140, 140, 220);
    drawGlassRect(g, (float)250, (float)200, 220, 80, clr, 1, 20, false, false, false, false);


    drawGlassRound(g,  50, 200, 60, Colour(125, 100, 10), 1);
    drawGlassRound(g,  150, 200, 44, Colour(90, 80, 10), 1);
    drawGlassRound(g,  50, 280, 80, Colour(55, 45, 35), 1);

    //drawGlassRound(g,  250, 280, 120, Colour(80, 90, 90), 1);

    drawGlassRound(g,  140, 120, 20, Colour(180, 200, 250), 1);
    drawGlassRound(g,  250, 200, 40, Colour(140, 140, 220), 1);
    drawGlassRound(g,  340, 210, 30, Colour(80, 80, 250), 1);

//    paintButton(g, 300, 300, 40, 30, false, false);
//    paintButton(g, 350, 300, 80, 80, false, false);
}

void MakeUpObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}


