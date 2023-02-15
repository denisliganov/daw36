
#include "36_renderwin.h"
#include "36_paramradio.h"
#include "36_draw.h"
#include "36.h"
#include "36_button.h"
#include "36_parambox.h"



RenderWinObject::RenderWinObject()
{
    format = new ParamRadio("Format");
    format->addOption("OGG");
    format->addOption("WAV");
    format->addOption("FLAC");

    quality = new ParamRadio("Quality");

    formatBox = new RadioBox(format);
    qualityBox = new RadioBox(quality);

    startRender = new Button36(false, "Render");

    // Position all controls

}

void RenderWinObject::drawSelf(Graphics& g)
{
    fill(g, 0.4f);
}

void RenderWinObject::handleChildEvent(Gobj * obj,InputEvent & ev)
{
    int a = 1;
}

void RenderWinObject::handleParamUpdate(Param * param)
{
    
}


