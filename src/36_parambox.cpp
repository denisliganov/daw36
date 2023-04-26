


#include "36_parambox.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_params.h"
#include "36_utils.h"
#include "36_instr.h"



/*
void drawText(Graphics& g)
{
    int txty = textHeight - 3;

    //setc(g, 0.8f);
    setc(g, 1.f);

    txtfit(g, fontId, param->getName(), 3, txty, width/2);

    setc(g, 1.f);

    std::string valstr = param->getValString();

    int sub = 0;

    if(valstr.data()[0] == '-' || 
       valstr.data()[0] == '+' ||
       valstr.data()[0] == '<')
    {
        int poffs = gGetTextWidth(fontId, valstr.substr(0, 1));

        txt(g, fontId, param->getValString().substr(0, 1), width/2 - poffs, txty);

        sub = 1;
    }

    setc(g, 1.f);

    txt(g, fontId, param->getValString().substr(sub), width/2, txty);
    txt(g, fontId, param->getUnitString(), width - tw3 - 2, txty);
}

*/



