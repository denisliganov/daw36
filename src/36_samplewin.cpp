
#include "36_samplewin.h"
#include "36_slider.h"
#include "36_params.h"
#include "36_draw.h"




SampleObject::SampleObject()
{
    ParamWin::addObject(slider1 = new Slider36(false), 10, 210, 60, 10);
    ParamWin::addObject(slider2 = new Slider36(false), 10, 225, 60, 10);
    ParamWin::addObject(slider3 = new Slider36(false), 10, 240, 60, 10);

    slider1->addParam(new Parameter());
    slider2->addParam(new Parameter());
    slider3->addParam(new Parameter());

    setWidthHeight(300, 300);
}

void SampleObject::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 0.4f);

    g.fillAll();
}

void SampleObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}




