
#include "36_configwin.h"
#include "36_slider.h"
#include "36_params.h"
#include "36_paramswin.h"
#include "36_draw.h"




ParamObject::ParamObject()
{
    addObject(slider1 = new Slider36(false), 10, 10, 60, 10);
    addObject(slider2 = new Slider36(false), 10, 25, 60, 10);
    addObject(slider3 = new Slider36(false), 10, 40, 60, 10);

    slider1->addParam(new Parameter());
    slider2->addParam(new Parameter());
    slider3->addParam(new Parameter());

    setWidthHeight(300, 300);
}
  
void ParamObject::drawself(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void ParamObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}



