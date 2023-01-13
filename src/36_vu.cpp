



#include "36_vu.h"
#include "36_draw.h"
#include "36.h"
#include "36_instr.h"
#include "36_mixer.h"




void Vu_ShowCoolFalldown()
{
    //_MControlPanel->MVol->vu->SetLR(Random::getSystemRandom().nextFloat()*0.5f + 0.5f, Random::getSystemRandom().nextFloat()*0.5f + 0.5f);
    /*
    for(Instrument* i : _MInstrPanel->instrs)
    {
        i->vu->SetLR(Random::getSystemRandom().nextFloat()*0.5f + 0.5f, Random::getSystemRandom().nextFloat()*0.5f + 0.5f);
    }*/
}



ChanVU::ChanVU(bool vert)
{
    vertical = vert;

    leftVal = rightVal = 0;

    decrement = .04f;

    maxValue = DAW_VOL_RANGE; // 1.5f;
}

ChanVU::~ChanVU()
{
///
}

float ChanVU::getL()
{
    return leftVal;
}

float ChanVU::getR()
{
    return rightVal;
}

void ChanVU::setValues(float L, float R)
{
    if(L > leftVal)
    {
        leftVal = L;

        if(leftVal > maxValue)
        {
            leftVal = maxValue;
        }
    }

    if(R > rightVal)
    {
        rightVal = R;

        if(rightVal > maxValue)
        {
            rightVal = maxValue;
        }
    }

    if(!isChanged())
    {
        redraw();
    }
}

void ChanVU::tick()
{
    leftVal -= decrement;
    rightVal -= decrement;

    if(leftVal < 0) leftVal = 0;
    if(rightVal < 0) rightVal = 0;

    redraw();
}

void ChanVU::drawSelf(Graphics & g)
{
    Instrument* i = dynamic_cast<Instrument*>(parent);

    if(vertical)
    {
        int left = (int)(height*getL());
        int right = (int)(height*getR());
        int both = (int)(height*((getL() + getR())/2.f));

        fillWithMonoColor(.4f);

        if(i)
        {
            i->setMyColor(g, 1);
        }
        else
        {
            //gSetMonoColor(g, .99f);
            gSetColor2(g, 0xffFFB040, 1);
        }

        int y = 0;

        while(y < height)
        {
            //if(y < left) gLineHorizontal(g, y2 - y, x1, x1 + 3);
            //if(y < right) gLineHorizontal(g, y2 - y, x2 - 3, x2);
            if(y < both) gLineHorizontal(g, y2 - y, x1, x2);

            y += 1;
        }
    }
    else
    {
        int left = (int)((width)*(getL()));
        int right = (int)((width)*(getR()));

        if(i)
        {
            i->setMyColor(g, .4f);
        }
        else
        {
            gSetMonoColor(g, .4f);
        }

        int x = 0;

        while(x < width)
        {
            gLineVertical(g, x1 + x, y1, y1 + height/2 - 1);
            gLineVertical(g, x1 + x, y1 + height/2 + 1, y2);

            x += 1;
        }

        if(i)
        {
            i->setMyColor(g, 1);
        }
        else
        {
            //gSetMonoColor(g, .99f);
            gSetColor2(g, 0xffFFB040, 1);
        }

        x = 0;

        while(x < width)
        {
            if(x < left)
            {
                gLineVertical(g, x1 + x, y1, y1 + height/2 - 1);
            }

            if(x < right)
            {
                 gLineVertical(g, x1 + x, y1 + height/2 + 1, y2);
            }

            x += 1;
        }
    }
}

InstrVU::InstrVU() : ChanVU(true)
{
    
}

void InstrVU::drawSelf(Graphics & g)
{
    float val = (getL() + getR())/2.f;

    val = pow(val, 0.3f);

    val *= (1 - 0.45f);

    Instrument* i = dynamic_cast<Instrument*>(parent);

    int left = (int)(height*getL());
    int right = (int)(height*getR());

    i->setMyColor(g, .2f);

    gFillRect(g, x1, y1, x2, y2);

    gSetColor2(g, 0xffFFB040, 1);

    int yc = 0;

    while(yc < height)
    {
        if(yc < left)
        {
            // gSetPixel(g, x1, y2 - yc); 
            gFillRect(g, x1, y2 - yc, x1+1, y2 - yc);
        }

        if(yc < right)
        {
            // gSetPixel(g, x2, y2 - yc); 
            gFillRect(g, x1, y2 - yc, x1+1, y2 - yc);
        }

        yc += 2;
    }

}


