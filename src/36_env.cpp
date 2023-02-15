

#include "36_env.h"
#include "36_controls.h"
#include "36_env.h"
#include "36_keyboard.h"
#include "36_events_triggers.h"
#include "36_utils.h"
#include "36_grid.h"
#include "36_transport.h"
#include "36_audio_dev.h"
#include "36_button.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_paramnum.h"




Envelope::Envelope(EnvType ct)
{
    envType = ct;

    lastValue = 0;
    aaBaseValue = 0;

    aaCount = 0;

    memset(buff, 0, MAX_BUFF_SIZE*sizeof(float));
}

Envelope* Envelope::clone(Element* el)
{
    Envelope* cl = clone();

    cl->param = param;

    return cl;
}

Envelope* Envelope::clone()
{
    Envelope* clone = new Envelope(envType);

    return clone;
}

void Envelope::placePoint(EnvPoint* p)
{
    float tick = p->tick;

    if(points.size() == 0)
    {
        points.push_back(p);
    }
    else
    {
        for(auto itr = points.begin(); itr != points.end(); )
        {
            if(tick <= (*itr)->tick)
            {
                points.insert(itr, p);

                break;
            }
            else if((*itr) == points.back())
            {
                points.push_back(p);

                break;
            }
            else
            {
                itr++;
            }
        }
    }

    calcTime();
}

void Envelope::calcTime()
{
    framelen = long(ticklen*MTransp->getFramesPerTick());
}

void Envelope::removePoint(EnvPoint* p)
{
    points.remove(p);
}

void Envelope::deletePoint(EnvPoint* p)
{
    removePoint(p);

    delete p;

    calcTime();
    updateLastValue();
}

EnvPoint* Envelope::addPoint(float x, float y_norm, bool big)
{
    EnvPoint* p = new EnvPoint;

    p->tick = (float)x1;
    p->y_norm = y_norm;
    p->deleted = false;
    p->big = big;

    placePoint(p);

    updateLastValue();

    return p;
}

float Envelope::getValue(float tick)
{
    if(points.size() > 1)
    {
        EnvPoint* ep1 = points.front();
        EnvPoint* ep2 = ep1;
        float xval      = tick;

        for (EnvPoint* ep : points)
        {
            if (ep == points.front())
            {
                continue;
            }

            if (xval <= ep->tick)
            {
                ep2 = ep;
                break;
            }

            ep1 = ep;
        }

        if(ep1 == ep2)
        {
            return lastValue;
        }

        if(xval > ep1->tick && xval < ep2->tick)
        {
            return Interpolate_Line(ep1->tick, ep1->y_norm, ep2->tick, ep2->y_norm, xval);
        }
        else if(xval == ep1->tick)
        {
            return ep1->y_norm;
        }
        else if(xval == ep2->tick)
        {
            return ep2->y_norm;
        }
    }
    else if(points.size() == 1)
    {
        return points.front()->y_norm;
    }

    return 0;
}

float Envelope::getValue(long frame)
{
    float xval;

    xval = frame * MTransp->getInvertedFPT();

    return getValue(xval);
}

bool Envelope::processBuffer1(long      curr_frame, 
                                long        buffframe,
                                long        num_frames, 
                                int         step, 
                                long        start_frame, 
                                Trigger*    tgenv, 
                                Trigger*    tgi)
{
    last_buffframe = buffframe;

    //float* single_buffer;
    //float* single_buffer_ov;
    //single_buffer = buff;
    //single_buffer_ov = buffov;

/*
    Command* cmd1 = NULL;
    if(cmd->param != NULL)
    {
        cmd1 = (Command*)cmd->param->envelopes->el;
    }

    if(cmd == cmd1 || cmd1 == NULL)
    {
        single_buffer = buff;
        single_buffer_ov = buffov;
    }
    else if(cmd1 != NULL)
    {
        single_buffer = ((Envelope*)cmd1->paramedit)->buff;
        single_buffer_ov = ((Envelope*)cmd1->paramedit)->buffov;
    }
*/

    float   fstep;
    float   xval;
    long    fPhase;
    long    fPhaseCeiling;

    if(tgenv != NULL)
    {
        fPhase = tgenv->framePhase;
        fPhaseCeiling = tgenv->framePhase + num_frames;
    }
    else
    {
        fPhase = long(curr_frame - start_frame);
    }

    fstep = MTransp->getInvertedFPT()*step;
    xval = MTransp->getInvertedFPT()*fPhase;

    float       useval = 1;
    int         cntdown = 1;
    long        i = 0;

    while(i < num_frames && i < MAX_BUFF_SIZE && xval < ticklen)
    {
        float val = 0;

        if(points.size() > 1)
        {
            cntdown--;

            if(cntdown == 0)
            {
                EnvPoint*   ep1 = points.front();
                EnvPoint*   ep2 = ep1;

                for (EnvPoint* ep : points)
                {
                    if (ep == points.front())
                    {
                        continue;
                    }

                    if (xval <= ep2->tick)
                    {
                        ep2 = ep;
                        break;
                    }

                    ep1 = ep;
                }

                if(ep2 == points.back())
                {
                    val = lastValue;
                }
                else if(ep2 != NULL && xval <= ep2->tick)
                {
                    if(xval > ep1->tick && xval < ep2->tick)
                    {
                        val = Interpolate_Line(ep1->tick, ep1->y_norm, ep2->tick, ep2->y_norm, xval);
                    }
                    else if(xval == ep1->tick)
                    {
                        val = ep1->y_norm;
                    }
                    else if(xval == ep2->tick)
                    {
                        val = ep2->y_norm;
                    }
                }

                cntdown = step;
                xval += fstep;

                if(tgenv != NULL)
                {
                    tgenv->framePhase += step;
                }
            }
        }
        else
        {
            if(points.size() == 1)
            {
                val = points.front()->y_norm;
            }
        }

        useval = val;

        if(envType == CmdEnv_VolEnv || envType == CmdEnv_PanEnv)
        {
            if(tgenv != NULL)
            {
                if(/*curr_frame == start_frame && i == 0 && */tgenv->prev_value == -123.0f)
                {
                    tgenv->prev_value = val;
                }
                else
                {
                    if(tgenv->aaCount > 0)
                    {
                        tgenv->cf1 = (float)tgenv->aaCount/DECLICK_COUNT;
                        tgenv->cf2 = 1.0f - tgenv->cf1;
                        useval = tgenv->aaBaseVal*tgenv->cf1 + val*tgenv->cf2;
                        tgenv->aaCount--;
                    }
                    else if(fabs(val - tgenv->prev_value) > 0.01f)
                    {
                        useval = tgenv->prev_value;

                        tgenv->aaBaseVal = tgenv->prev_value;
                        tgenv->aaCount = DECLICK_COUNT;
                        tgenv->cf1 = 1;
                        tgenv->cf2 = 0;
                    }

                    tgenv->prev_value = useval;
                }
            }
            else
            {
                if(curr_frame == start_frame && i == 0)
                {
                    tgi->prev_value = val;
                }
                else
                {
                    if(aaCount > 0)
                    {
                        float cf1 = (float)aaCount/DECLICK_COUNT;
                        float cf2 = 1.0f - cf1;

                        useval = aaBaseValue*cf1 + val*cf2;

                        aaCount--;
                    }
                    else if(fabs(val - tgi->prev_value) > 0.15f)
                    {
                        useval = tgi->prev_value;

                        aaBaseValue = tgi->prev_value;

                        aaCount = DECLICK_COUNT;
                    }

                    tgi->prev_value = useval;
                }
            }
        }

        // check for shit

        if(val < 0 || val > 1)
        {
            val = 0;
        }

        if(useval < 0 || useval > 1)
        {
            useval = 0;
        }

        buff[buffframe + i] = val;

        if(envType == CmdEnv_VolEnv)
        {
            buffoutval[buffframe + i] = GetVolOutput(useval*DAW_VOL_RANGE);
        }
        else if(envType == CmdEnv_PanEnv)
        {
            buffoutval[buffframe + i] = (useval*2) - 1;
        }
        else
        {
            buffoutval[buffframe + i] = useval;
        }

        i++;
    }	

    last_buffframe_end = buffframe + i;

    // update the parameter directly from here if required

    if(num_frames > 0 && param != NULL && param->getEnvDirect())
    {
        param->setValueFromEnvelope(useval, this);
        param->setLastVal(param->getOutVal());
    }

    // Trigger phase can exceed number of frames, but it shouldn't

    if(tgenv != NULL && tgenv->framePhase > fPhaseCeiling)
    {
        tgenv->framePhase = fPhaseCeiling;
    }

    return true;
}

void Envelope::updateLastValue()
{
    if(points.size() > 0)
    {
        if(points.back()->tick <= ticklen)
        {
            lastValue = points.back()->y_norm;
        }
        else
        {
            if(points.size() > 1)
            {
                for(auto itr = points.begin(); itr != points.end(); itr++)
                {
                    auto itr1 = itr;

                    itr1++;

                    if(*itr1 != NULL)
                    {
                        EnvPoint* ep = (*itr);
                        EnvPoint* ep1 = (*itr1);

                        if(ep1->tick > ticklen)
                        {
                            lastValue = Interpolate_Line(ep->tick, ep->y_norm, ep1->tick, ep1->y_norm, ticklen);

                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(envType == CmdEnv_VolEnv)
        {
            lastValue = 1;
        }
        else if(envType == CmdEnv_PanEnv)
        {
            lastValue = 0;
        }
    }
}

void Envelope::deleteRange(float x_curr, float x_start, bool start_except)
{
    if(x_curr >= x_start)
    {
start:
        for(EnvPoint* ep : points)
        {
            if(ep != points.front() && ep->tick >= x_start && ep->tick <= x_curr && !(start_except && ep->tick == x_start))
            {
                deletePoint(ep);
                goto start;
            }
        }
    }
    else if(x_curr < x_start)
    {
start1:
        for(EnvPoint* ep : points)
        {
            if(ep != points.front() && ep->tick < x_start && ep->tick >= x_curr)
            {
                deletePoint(ep);
                goto start1;
            }
        }
    }
}

void Envelope::drag(int mouse_x, int mouse_y, unsigned flags)
{
    int py = y2 - mouse_y;

    if(py < 0)
    {
        py = 0;
    }

    if(py > height)
    {
        py = height;
    }

    float yval = (float)py/height;

    if(flags & kbd_alt)
    {
        switch(envType)
        {
            case CmdEnv_VolEnv:
                yval = 1.0f/DAW_VOL_RANGE;
                break;

            case CmdEnv_PanEnv:
                yval = 0.5f;
                break;
        }
    }

    int px;

    if(verticalDragOnly == true)
    {
        px = 0;
    }
    else
    {
        px = mouse_x - x1;
    
        if(px < 0)
        {
            px = 0;
        }

        if(px > width)
        {
            px = width;
        }
    }

    float env_xcurr = (float)px;

    if(envAction == ENVPOINTING)
    {
        float oldx = activePoint->tick;

        activePoint->tick = (float)px;
        activePoint->y_norm = yval;

        removePoint(activePoint);
        placePoint(activePoint);

        if(flags & kbd_shift && activePoint != points.back())
        {
            float dx = activePoint->tick - oldx;

            for(auto itr = points.begin(); itr != points.end(); itr++)
            {
                if((*itr)->tick <= activePoint->tick)
                    continue;

                (*itr)->tick += dx;
            }
        }

        updateLastValue();

    }
    else if(envAction == ENVDRAWMOVE)
    {
        deleteRange(env_xcurr, env_xstart, true);

        if(px > 0)
        {
            addPoint((float)px, yval);
        }
        else
        {
            points.front()->tick = (float)px;
            points.front()->y_norm = (float)py/height;
        }

        env_xstart = env_xcurr;
    }
    else if(envAction == ENVDELETING)
    {
        if(px > 0)
        {
            deleteRange(env_xcurr, env_xstart, false);
        }
        else
        {
            points.front()->tick = (float)px;
            points.front()->y_norm = (float)py/height;
        }
    }
    else if(envAction == ENVLINEMOVE)
    {
        deleteRange(env_xcurr, env_xstart, true);
    
        if(px > 0)
        {
            addPoint((float)px, yval);
        }
        else
        {
            points.front()->tick = (float)px;
            points.front()->y_norm = (float)py/height;
        }
    }

    calcTime();
}

void Envelope::check(int mouse_x, int mouse_y)
{
    bool wasPointing = false;

    if(envAction == ENVPOINTING)
    {
        wasPointing = true;
    }

    verticalDragOnly = false;

    activePoint = NULL;

    envAction = ENVUSUAL;

    if(checkMouseTouching(mouse_x, mouse_y))
    {
        envAction = ENVUSUAL;

        for(EnvPoint* p : points)
        {
            if(fabs(mouse_x - (x1 + p->tick)) <= ENVPOINT_TOUCH_RADIUS &&
                abs(mouse_y - (y1 + height - p->y_norm*(float)height)) <= ENVPOINT_TOUCH_RADIUS)
            {
                envAction = ENVPOINTING;
                activePoint = p;

                if(p == points.front())
                {
                    verticalDragOnly = true;
                }
                else
                {
                    verticalDragOnly = false;
                }

                break;
            }
        }
    }

    if(activePoint == NULL)
    {
        if((abs(mouse_x - (x1 + width)) <= 3)&&(mouse_y >= y1 - 12)&&(mouse_y <= (y1 + height)))
        {
            MGrid->setActionMode(GridMode_ElemResizing);

            MGrid->resizeEdge = Resize_Right;
            MGrid->activeElem = (Element*)this;
        }
        else if((abs(mouse_y - (y1 + height)) <= 2)&&(mouse_x >= x1)&&(mouse_x <= x1 + width))
        {
            MGrid->setActionMode(GridMode_ElemResizing);

            MGrid->resizeEdge = Resize_Bottom;
            MGrid->activeElem = (Element*)this;
        }
    }

    if((wasPointing == true) && envAction == ENVUSUAL)
    {
        redraw();
    }
}

void Envelope::handleMouseDown(InputEvent& ev)
{
    if(ev.leftClick)
    {
        if(ev.mouseY >= y1)
        {
            int px = ev.mouseX - x1;
            int py = y1 + height - ev.mouseY;
            float yval = (float)py/height;

            if(ev.keyFlags & kbd_alt)
            {
                switch(envType)
                {
                    case CmdEnv_VolEnv:
                        yval = 1.0f/DAW_VOL_RANGE;
                        break;

                    case CmdEnv_PanEnv:
                        yval = 0.5f;

                        break;
                }
            }

            if(ev.keyFlags & kbd_ctrl)
            {
                envAction = ENVDRAWMOVE;

                env_xstart = (float)px;
            }
            else if(envAction != ENVPOINTING)
            {
                envAction = ENVPOINTING;

                activePoint = addPoint((float)px, yval);

                verticalDragOnly = false;
            }
            else if(envAction == ENVPOINTING)
            {
                activePoint->big = true;
            }

            calcTime();
        }
    }
    else
    {
        int px = ev.mouseX - x1;

        int py = y1 + height - ev.mouseY;

        if(!(ev.keyFlags && kbd_ctrl))
        {
            if(envAction & ENVPOINTING)
            {
                if(activePoint != points.front())
                {
                    deletePoint(activePoint);
                }
            }

            envAction = ENVDELETING;

            env_xstart = (float)px;
        }
        else
        {
            if(envAction & ENVPOINTING)
            {
                // 
            }
            else
            {
                activePoint = addPoint((float)px, (float)py/height);
            }

            envAction = ENVLINEMOVE;

            env_xstart = (float)px;
        }
    }
}

// Returns number of frames remaining between envelope length and passed trigger's phase

long Envelope::getFramesRemaining(long frame_phase)
{
    return framelen - frame_phase;
}

bool Envelope::isOutOfBounds(long frame_phase)
{
    if(frame_phase < 0 || frame_phase > framelen)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Envelope::setTickLength(float tl)
{
    Element::setTickLength(tl);

    updateLastValue();
}

void Envelope::markDeleted(bool del)
{
    deleted = del;

    if(del == false)
    {
        Envelope* env = (Envelope*)this;

        if(env->param->isRecording())
        {
            env->param->finishRecording();
        }
    }
}

void Envelope::drawOnGrid(Graphics & g,Grid * grid)
{
    Colour clr = Colour(0xff29295D);
    Colour clr1 = Colour(0xff4A4A7F);

    g.setColour(clr.withAlpha(0.2f));
    g.fillRect(x1, y1, width, height + 1);

    if(type == CmdEnv_VolEnv)
    {
        g.setColour(clr1.withAlpha(0.75f));

        int lhgt = RoundFloat(height*(1 - 1/DAW_VOL_RANGE));

        gLineHorizontal(g, y1 + lhgt, x1 + 1, x1 + width - 1);
    }
    else if(type == CmdEnv_PanEnv)
    {
        g.setColour(clr1.withAlpha(0.75f));

        gLineHorizontal(g, RoundFloat(y1 + (float)height/2), x1, x1 + width);
    }

    float scale = grid->getPixelsPerTick();

    if(type != CmdEnv_PanEnv)
    {
        gSetColor(g, 0x1fFFFFFF);

        Path path;

        EnvPoint* pf = points.front();

        if(pf != NULL)
        {
            path.startNewSubPath((float)(x1 + pf->tick*scale), (float)(y1 + height));
            path.lineTo((float)(x1 + pf->tick*scale), (float)(y1 + height - pf->y_norm*(float)height));
        }

        for(auto itr = points.begin(); itr != points.end(); itr++)
        {
            EnvPoint* p = *itr;
            auto itrnext = itr;
            itrnext++;
            EnvPoint* pnext = itrnext != points.end() ? (*itrnext) : NULL;

            if(p->deleted == false)
            {
                if(p->tick <= ticklen && (pnext == NULL || pnext->tick <= ticklen))
                {
                    if(pnext != NULL)
                    {
                        path.lineTo((float)(x1 + pnext->tick*scale), (float)(y1 + height - pnext->y_norm*(float)height));
                    }
                    else
                    {
                        path.lineTo((float)(x1 + width), (float)(y1 + height - p->y_norm*(float)height));
                    }
                }
                else
                {
                    float px1 = p->tick;
                    float py1 = p->y_norm;

                    float px2;
                    float py2;

                    if(pnext != NULL)
                    {
                        px2 = pnext->tick;
                        py2 = pnext->y_norm;
                    }
                    else
                    {
                        px2 = p->tick;
                        py2 = p->y_norm;
                    }

                    float py3 = Interpolate_Line(px1, py1, px2, py2, ticklen);

                    path.lineTo((float)(x1 + width), (float)(y1 + height - py3*(float)height));

                    break;
                }
            }
        }

        path.lineTo((float)(x1 + width), (float)(y1 + height));
        path.closeSubPath();

        g.fillPath(path);
    }

    g.setColour(clr1.withAlpha(0.8f));

    gLineHorizontal(g, y1 + height, x1, x1 + width);

    gLineVertical(g, x1 + width - 1, y1, y1 + height);
    gLineVertical(g, x1, y1, y1 + height);


    EnvPoint* pprev = points.front();
    Path envpath;

    for(EnvPoint* p : points)
    {
        if(p->tick <= ticklen)
        {
            gSetColor(g, (0xafFFFFFF));

            if(p != points.front())
            {
                gLine(g, x1 + p->tick*scale, y1 + height - p->y_norm*(float)height, x1 + pprev->tick*scale, y1 + height - pprev->y_norm*(float)height);
            }

            pprev = p;

            int xr = RoundFloat(x1 + p->tick*scale);
            int yr = RoundFloat(y1  + height - p->y_norm*(float)height);

            gSetColor(g, (0xafFFFFFF));

            if(p->big)
            {
                gLineVertical(g, xr, yr - 2, yr + 3);

                gLineHorizontal(g, yr, xr - 2, xr + 3);
                gLineHorizontal(g, yr - 1, xr - 1, xr + 2);
                gLineHorizontal(g, yr + 1, xr - 1, xr + 2);
            }


            if(envAction == ENVPOINTING && activePoint == p)
            {
                gSetColor(g, (0xffFF6F2F));

                gLine(g, xr - 3, yr, xr, yr - 3);
                gLine(g, xr, yr - 3, xr + 3, yr);
                gLine(g, xr + 3, yr, xr, yr + 3);
                gLine(g, xr, yr + 3, xr - 3, yr);

                g.setPixel(xr + 3, yr);
            }

            pprev = p;
        }
        else // invisible point. draw last cut line and break
        {
            gSetColor(g, (0xafFFFFFF));

            float px1 = pprev->tick;
            float px2 = p->tick;

            float py1 = pprev->y_norm;
            float py2 = p->y_norm;

            float py3 = Interpolate_Line(px1, py1, px2, py2, ticklen);

            gLine(g, (float)(x1 + px1*scale), (float)(y1 + height - py1*(float)height), (float)(x2 - 1), (float)(y1 + height - py3*(float)height));

            break;
        }
    }
}



