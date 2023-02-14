
#include "36_template.h"
#include "36_draw.h"




TemplateWinObj::TemplateWinObj()
{
    setWidthHeight(300, 300);
}

void TemplateWinObj::drawSelf(Graphics& g)
{
    fill(g, 0.4f);
}

void TemplateWinObj::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    //
}



