
#include "36_samplewin.h"
#include "36_params.h"
#include "36_draw.h"






SampleObject::SampleObject()
{
//    slider1->addParam(new Parameter());
//    slider2->addParam(new Parameter());
//    slider3->addParam(new Parameter());

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




