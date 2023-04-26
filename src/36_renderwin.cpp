
#include "36_renderwin.h"
#include "36_params.h"
#include "36_draw.h"
#include "36.h"
#include "36_button.h"
#include "36_knob.h"



RenderWinObject::RenderWinObject()
{
    format = new Parameter("Format", Param_Radio);
    format->addOption("OGG");
    format->addOption("WAV");
    format->addOption("FLAC");

    quality = new Parameter("Quality", Param_Radio);

    formatBox = new SelectorBox(format);
    qualityBox = new SelectorBox(quality);
    startRender = new Button36(false, "Render");

    // Position all controls
}

void RenderWinObject::drawSelf(Graphics& g)
{
    WinObject::fill(g, 0.4f);
}

void RenderWinObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void RenderWinObject::handleParamUpdate(Parameter * param)
{
    
}


