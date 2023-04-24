



#include "36_vu.h"
#include "36_draw.h"
#include "36.h"
#include "36_instr.h"




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
    //setTouchable(false);

    vertical = vert;

    leftVal = rightVal = 0;

    decrement = .02f;

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
    Instr* i = dynamic_cast<Instr*>(parent);

    if(vertical)
    {
        int left = (int)(height*getL());
        int right = (int)(height*getR());
        int both = (int)(height*((getL() + getR())/2.f));

        fill(g, .24f);

        if(i)
        {
            i->setMyColor(g, 1);
        }
        else
        {
            //gSetMonoColor(g, .99f);
            gSetColor2(g, 0xff80E0800, 1);
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
            i->setMyColor(g, .3f);
        }
        else
        {
            gSetMonoColor(g, .3f);
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
            gSetMonoColor(g, .8f);
            //gSetColor2(g, 0xffFFB040, 1);
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

void ChanVU::handleMouseDown(InputEvent& ev)
{
    if (parent)
    {
        parent->handleChildEvent(this, ev);
    }
}


