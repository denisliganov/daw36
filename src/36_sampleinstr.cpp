

#include "rosic/rosic.h"

#include "36_sampleinstr.h"
#include "36_events_triggers.h"
#include "36_note.h"
#include "36_pattern.h"
#include "36_utils.h"
#include "36_audio_dev.h"
#include "36_textinput.h"
#include "36_samplewin.h"
#include "36_grid.h"
#include "36_draw.h"
#include "36_renderer.h"
#include "36_transport.h"
#include "36_paramvol.h"
#include "36_paramnum.h"


//Sample buffer for sinc interpolation

static float Yi[256];


unsigned int        Config_RenderInterpolation;
unsigned int        Config_DefaultInterpolation;



Sample::Sample(float* data, char* smp_path, SF_INFO sfinfo)
{
    type = Instr_Sample;

    sampleData = data;

    if(smp_path != NULL)
    {
        filePath = smp_path;
 
        char* nm = (char*)smp_path;

        while (strchr(nm, '\\') != NULL)
        {
            nm = strchr(nm, '\\') + 1;
        }

        objName = nm;

        objName = ToLowerCase(objName);

        std::string::size_type pos = objName.find(".");

        objName = objName.substr(0, pos);  // the part till the space

        //str1 = str3.substr(pos + 1); // the part after the space
    }

    sample_info = sfinfo;

    waveImage = NULL;

    numFrames = sample_info.frames;
    numChannels = sample_info.channels;

    rateDown = (float)sample_info.samplerate/MAudio->getSampleRate();
    rateUp = MAudio->getSampleRate()/sample_info.samplerate;

    lastNoteLength = -1;

    // Init sample loop
    looptype = LoopType_NoLoop;
    setLoopPoints(0, long(numFrames - 1));

    // Init normalizing
    normalized = false;
    normFactor = 1.0f;

    updateNormalizeFactor();

    timelen = numFrames/MAudio->getSampleRate();

    envVol = new Envelope(CmdEnv_VolEnv);
    envVol->setTickLength(jmax(timelen, 2.f));
    //envVol->timebased = true;

    // Add some default breakpoints to the sample envelope

    envVol->addPoint(0.3f, 0.5f);
    envVol->addPoint(0.6f, 0.0f);

    createSelfPattern();
}

Sample::~Sample()
{
///
}

void Sample::activateTrigger(Trigger* tg)
{
    SampleNote* samplent = (SampleNote*)tg->el;

    bool skip = !(samplent->initCursor(&tg->wt_pos));

    if(skip == false)
    {
        tg->outsync = false;
        tg->tgworking = true;
        tg->muted = false;
        tg->broken = false;
        tg->auCount = 0;
        tg->setState(TS_Sustain);
        tg->framePhase = 0;

        if(tg->noteVal == -1)
        {
            tg->noteVal = samplent->getNoteValue();
            tg->freq = samplent->freq;
            tg->freq_incr_base = samplent->dataStep;
        }
        else
        {
            tg->freq = NoteToFreq(tg->noteVal);
            tg->freq_incr_base = calcSampleFreqIncrement(tg->noteVal - BaseNote);
        }

        tg->volBase = samplent->vol->getOutVal();
        tg->panBase = samplent->pan->getOutVal();

        tg->ep1 = envVol->points.front();

        tg->envVal1 = tg->ep1->y_norm;
        tg->env_phase1 = 0;

        tg->freq_incr_sgn = !samplent->reversed ? 1 : -1;

        activeTriggers.push_back(tg);
    }
}

void Sample::copyDataToClonedInstrument(Instrument * instr)
{
    //Instrument::makeClone(instr);
    
    Sample* nsample = (Sample*)instr;

    delete nsample->envVol;

    nsample->envVol = envVol->clone();

    if(normalized != nsample->normalized)
    {
        nsample->toggleNormalize();
    }

    nsample->setLoopPoints(lp_start, lp_end);
    nsample->looptype = looptype;
}

float Sample::calcSampleFreqIncrement(int semitones)
{
    return rateDown*(CalcFreqRatio(semitones));
}

bool Sample::checkBounds(SampleNote* samplent, Trigger* tg, long num_frames)
{
    if(looptype == LoopType_NoLoop)
    {
        if(samplent->isOutOfBounds(&tg->wt_pos) == true)
        {
            tg->setState(TS_Finished);

            //endframe = num_frames - ANTIALIASING_FRAMES;
            //if(endframe < 0)

            endFrame = 0;
        }
        else if(tg->framePhase >= samplent->framelen)
        {
            tg->setState(TS_SoftFinish);
        }
    }
    else
    {
        if(tg->tgState == TS_Sustain && !tg->previewTrigger && tg->framePhase >= samplent->framelen)
        {
            // Go to release state as note is over

            tg->setState(TS_Release);
        }
    }

    if(tg->tgState == TS_Release)
    {
#if 0
        if(envVol != NULL)
        {
            /*
            bool finished = false;
            if(tg->tgvolloc != NULL)
            {
                finished = VEnv->IsOutOfBounds(venvphase);
                endframe = num_frames - (venvphase - VEnv->frame_length);
            }
            else
            {
                finished = VEnv->IsOutOfBounds(tg->frame_phase);
                endframe = num_frames - (tg->frame_phase - VEnv->frame_length);
            }

            if(finished)
                tg->tgstate = TgState_Finished;
            */

            if(tg->ep1->next == NULL)
            {
                tg->setState(TS_SoftFinish);
            }

            //if(endframe < 0)

            endframe = 0;
        }
        else
#endif
        {
            tg->setState(TS_SoftFinish);
        }
    }

    if(tg->tgState == TS_Finished)
    {
        return true;
    }

    return false;
}

SubWindow* Sample::createWindow()
{
    return window->addWindow(new SampleObject());
}

// Return pixel length for a number of frames, depending on sample rate and current tick width

int Sample::calcPixLength(long num_frames, long sample_rate, float tickwidth)
{
    float rate = (float)MAudio->getSampleRate()/sample_info.samplerate;

    return RoundDouble(((double)numFrames/(double)MAudio->getSampleRate())/MTransp->getSecondsPerTick()*MGrid->getPixelsPerTick()*rate);
}

void Sample::dumpData()
{
    File f("sampledump.bin");

    f.create();

    FileOutputStream* os = f.createOutputStream();

    os->write(sampleData, int(numFrames*sample_info.channels*sizeof(float)));

    delete os;
}

inline void Sample::getMonoData(double cursor_pos, float* dataLR)
{
    float x1 = (float)(int)cursor_pos;
    float x2 = x1 + 1;

    float y1 = sampleData[(int)x1];
    float y2;
    float  Y[6];

    if((int)x2 == numFrames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sampleData[(int)x2];
    }

    double x3 = cursor_pos;

    if (x1 == x3)
    {
        *dataLR = sampleData[(int)x3];
    }
    else
    {
        unsigned int method = Config_DefaultInterpolation;

        if(Render_isWorking == true)
        {
            method = Config_RenderInterpolation;
        }

        switch (method)
        {
            case Interpol_linear:
            {
               *dataLR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)cursor_pos - x1;
                float xm1 = x1 > 0 ? sampleData[(int)(x1-1)] : 0;
                float _x0  = sampleData[(int)x1];
                float _x1  = (x1+1) <= (numFrames -1) ? sampleData[(int)(x1 + 1)] : sampleData[(int)x1];
                float _x2  = (x1+2) <= (numFrames -1) ? sampleData[(int)(x1 + 2)] : sampleData[(int)x1];

                *dataLR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) + (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)cursor_pos - x1;
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i)< (numFrames -1))
                    {
                        Y[i] = sampleData[(int)pX+i];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }

               *dataLR = gaussInterpolation(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = cursor_pos - x1;
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i < num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < numFrames))
                    {
                        Yi[i] = sampleData[(int)(uiLBorder + i)];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }

                *dataLR = sincInterpolate(Yi, dX, num);

//                char buf[100] = {0};

//                sprintf(buf, "%u %u %.08f %.08f %.08f %.08f",
//                    (int)x1, 
//                    (int)x2, 
//                    dX, sample_data[(int)x1], *dataLR, sample_data[(int)x2]);

//                DBG(buf);
            }
            break;

            default:
            {
            }

            break;
        }
    }
}

inline void Sample::getStereoData(double cursor_pos, float* dataL, float* dataR)
{
    double x1, x2, x3;
    float y1, y2;
    float  Y[6];

    // process left

    x1 = (float)((int)cursor_pos);
    x2 = x1 + 1;
    x3 = cursor_pos;

    y1 = sampleData[(int)x1*2];

    if((int)x2 >= numFrames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sampleData[(int)x2*2];
    }

    // If current pos matches to sample point, don't need to interpolate (make CPU overhead)

    if (x3 == x1)
    {
        *dataL = sampleData[(int)x3*2];
    }
    else
    {
        unsigned int method = Config_DefaultInterpolation;

        if(Render_isWorking == true)
        {
            method = Config_RenderInterpolation;
        }

        switch (method)
        {
            case Interpol_linear:
            {
                *dataL = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sampleData[(int)(x1-1)*2] : 0;
                float _x0  = sampleData[(int)x1*2];
                float _x1  = (x1+1) <= (numFrames - 1) ? sampleData[(int)(x1 + 1)*2] : sampleData[(int)x1*2];
                float _x2  = (x1+2) <= (numFrames - 1) ? sampleData[(int)(x1 + 2)*2] : sampleData[(int)x1*2];

               *dataL = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) + (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (numFrames -1))
                    {
                        Y[i] = sampleData[(int)(pX+i)*2];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataL = gaussInterpolation(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < numFrames))
                    {
                        Yi[i] = sampleData[(int)(uiLBorder + i)*2];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }
                *dataL = sincInterpolate(Yi,dX,num);
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // process right

    y1 = sampleData[(int)x1*2 + 1];

    if(((int)x2 + 1) >= numFrames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sampleData[(int)x2*2 + 1];
    }

    /* If current pos matches to sample point, don't need to interpolate (make CPU overhead) */
    if (x3 == x1)
    {
        *dataR = sampleData[(int)x3*2 + 1];
    }
    else
    {
        unsigned int method = Config_DefaultInterpolation;

        if(Render_isWorking == true)
        {
            method = Config_RenderInterpolation;
        }

        switch (method)
        {
            case Interpol_linear:
            {
                *dataR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sampleData[(int)(x1-1)*2 + 1] : 0;
                float _x0  = sampleData[(int)x1*2 + 1];

                float _x1  = (x1+1) <= (numFrames -1) ? sampleData[(int)(x1 + 1)*2 + 1] : sampleData[(int)x1*2 + 1];
                float _x2  = (x1+2) <= (numFrames -1) ? sampleData[(int)(x1 + 2)*2 + 1] : sampleData[(int)x1*2 + 1];

               *dataR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) + (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (numFrames -1))
                    {
                        Y[i] = sampleData[(int)(pX+i)*2 + 1];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataR = gaussInterpolation(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < numFrames))
                    {
                        Yi[i] = sampleData[(int)(uiLBorder + i)*2 + 1];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }

               *dataR = sincInterpolate(Yi, dX, num);
            }
            break;

            default:
            {
            }
            break;
        }
    }
}

inline float Sample::gaussInterpolation(float* Yi, float dX)
{
    float ret_val = 0;

    dX += 2;

    ret_val = (float)((((((-1.0/12.0*Yi[2]-1.0/120.0*Yi[0]+1.0/24.0*Yi[1]+1.0/120.0*Yi[5]+1.0/12.0*Yi[3]-1.0/24.0*Yi[4])*dX + \
                       (13.0/12.0*Yi[2]+1.0/8.0*Yi[0]-7.0/12.0*Yi[1]-1.0/12.0*Yi[5]-Yi[3]+11.0/24.0*Yi[4]))*dX + \
                       (-59.0/12.0*Yi[2]-17.0/24.0*Yi[0]+71.0/24.0*Yi[1]+7.0/24.0*Yi[5]+49.0/12.0*Yi[3]-41.0/24.0*Yi[4]))*dX + \
                       (107.0/12.0*Yi[2]+15.0/8.0*Yi[0]-77.0/12.0*Yi[1]-5.0/12.0*Yi[5]-13.0/2.0*Yi[3]+61.0/24.0*Yi[4]))*dX + \
                       (-137.0/60.0*Yi[0]+5.0*Yi[1]-5.0*Yi[2]+10.0/3.0*Yi[3]-5.0/4.0*Yi[4]+1.0/5.0*Yi[5]))*dX + Yi[0]);

    return ret_val;
}

void Sample::load(XmlElement * instrNode)
{
    Instrument::load(instrNode);

    bool norm = instrNode->getBoolAttribute(T("Normalized"));

    if(normalized != norm)
    {
        toggleNormalize();
    }

    lp_start = (long)instrNode->getStringAttribute(T("LoopStart")).getLargeIntValue();
    lp_end = (long)instrNode->getStringAttribute(T("LoopEnd")).getLargeIntValue();
    looptype = (LoopType)instrNode->getIntAttribute(T("LoopType"));

    XmlElement * envX = instrNode->getChildByName(T("SmpEnv"));

    if(envX != NULL)
    {
        envVol->load(envX);
    }
}

long Sample::processTrigger(Trigger * tg, long num_frames, long buff_frame)
{
    SampleNote* samplent = (SampleNote*)tg->el;

    bool fill = true;

    bool skip = false;

    // Initial stuff

    tg->freq_incr_active = tg->freq_incr_base;

    tg->vol_val = tg->volBase;
    tg->pan_val = tg->panBase;

    preProcessTrigger(tg, &skip, &fill, num_frames, buff_frame);

    if(!skip)
    {
        long cc, tc0 = buff_frame*2;
        float sd1, sd2;

        for(cc = 0; cc < num_frames; cc++)
        {
            if(samplent->isOutOfBounds(&tg->wt_pos) == true)
            {
                break;
            }

            if(fill)
            {
                if(1 == sample_info.channels) // Mono sample
                {
                    getMonoData(tg->wt_pos, &sd1);

                    tempBuff[tc0++] = sd1*tg->envVal1;
                    tempBuff[tc0++] = sd1*tg->envVal1;
                }
                else if(2 == sample_info.channels) // Stereo sample
                {
                    getStereoData(tg->wt_pos, &sd1, &sd2);

                    tempBuff[tc0++] = sd1*tg->envVal1;
                    tempBuff[tc0++] = sd2*tg->envVal1;
                }
            }

            tg->env_phase1 += MAudio->getInvertedSampleRate();

            /*
            while(tg->ep1->next != NULL && (tg->env_phase1 >= tg->ep1->next->tick || tg->ep1->next->tick == tg->ep1->tick))
            {
                tg->ep1 = tg->ep1->next;
            }

            if(tg->ep1->next != NULL && (tg->ep1->next->tick > tg->ep1->tick))
            {
                tg->envVal1 = Interpolate_Line(tg->ep1->tick, tg->ep1->y_norm, tg->ep1->next->tick, tg->ep1->next->y_norm, tg->env_phase1); //tg->ep1->cf*one_divided_per_sample_rate;
                tg->envVal1 = GetVolOutput(tg->envVal1);
            }*/

            //ebuff1[buffframe + ic] = tg->envV1;

            tg->envVal1 = 1;

            tg->wt_pos += tg->freq_incr_sgn*tg->freq_incr_active;

            if(tg->freq_incr_sgn == 1 && tg->wt_pos >= lp_end)
            {
                if(looptype == LoopType_ForwardLoop)
                {
                    tg->wt_pos = lp_start;
                }
                else if(looptype == LoopType_PingPongLoop)
                {
                    tg->freq_incr_sgn = -1;

                    tg->wt_pos = lp_end;
                }
            }
            else if(tg->freq_incr_sgn == -1 && tg->wt_pos <= lp_start)
            {
                if(looptype == LoopType_ForwardLoop)
                {
                    tg->wt_pos = lp_end;
                }
                else if(looptype == LoopType_PingPongLoop)
                {
                    tg->freq_incr_sgn = 1;

                    tg->wt_pos = lp_start;
                }
            }

            tg->framePhase++;
        }

        checkBounds(samplent, tg, cc);

        return cc;
    }
    else
    {
        return 0;
    }
}

void Sample::setLoopStart(long start)
{
    lp_start = start;
}

void Sample::setLoopEnd(long end)
{
    lp_end = end;
}

void Sample::setLoopPoints(long start, long end)
{
    lp_start = start;

    lp_end = end;
}

inline double Sample::sinc(double a)
{
    return sin(PI*a)/(PI*a);
}

inline double Sample::sincWindowedBlackman(double a, double b, unsigned int num)
{
    return sinc(a)*(0.42 - 0.5*cos((2.f*PI*b)/((float)num)) + 0.08*cos((4.f*PI*b)/((float)num)));
}

inline float Sample::sincInterpolate(float* Yi, double dX, unsigned int num)
{
    double ret_val = 0;
    unsigned char mid = num/2;
    int j = (int)(0 - mid);
    unsigned int i = 0;

    for (; i < num; ++i)
    {
        ret_val += Yi[i]*sincWindowedBlackman((double)(j - dX), (double) (i - dX), num);

        ++j;
    }

    return (float)ret_val;
}

void Sample::save(XmlElement * instrNode)
{
    Instrument::save(instrNode);

    instrNode->setAttribute(T("Normalized"), normalized ? 1 : 0);
    instrNode->setAttribute(T("LoopStart"), String(lp_start));
    instrNode->setAttribute(T("LoopEnd"), String(lp_end));
    instrNode->setAttribute(T("LoopType"), int(looptype));

    // XmlElement * envX = envVol->save("SmpEnv");
    // instrNode->addChildElement(envX);
}

void Sample::toggleNormalize()
{
    if(normFactor != 0)
    {
        normalized = !normalized;
        float mult;

        if(normalized == true)
        {
            mult = normFactor;
        }
        else
        {
            mult = 1.0f/normFactor;
        }

        long dc = 0;
        long total = long(numFrames*sample_info.channels);

        while(dc < total)
        {
            sampleData[dc] *= mult;

            dc++;
        }
    }
}

void Sample::updateNormalizeFactor()
{
    float dmax = 0;
    float dcurr;
    long dc = 0;
    long total = long(numFrames*sample_info.channels);

    while(dc < total)
    {
        dcurr = fabs(sampleData[dc]);

        if(dcurr > dmax)
        {
            dmax = dcurr;
        }

        dc++;
    }

    if(dmax > 0)
    {
        normFactor = 1.0f/dmax;
    }
}

void Sample::updWaveImage()
{
    if(waveImage != NULL)
    {
        delete waveImage;

        waveImage = NULL;
    }

    //int pixlen = (int)(((double)sample_info.frames/(double)_MAudio->getSampleRate())/MainClock->getSecondsPerTick()*_MGrid->getPixelsPerTick());

    double ticks = (double)numFrames*rateUp/MTransp->getFramesPerTick();
    int pixels = ticks*MGrid->getPixelsPerTick();
    float ppf = rateUp*(MGrid->getPixelsPerTick()/MTransp->getFramesPerTick());

    if (pixels > 0)
    {
        float* arrayVals = (float*)malloc(2*pixels*sizeof(float));

        memset(arrayVals, 0, 2*pixels * sizeof(float));

        if(arrayVals == NULL)
        {
            MWindow->showAlertBox("Not enough memory for wave image!", "OK");

            return;
        }

        long    frameCount = 0;
        float   ratio = float(pixels)/numFrames;
        float   dataPlus = 0;
        float   dMax = 0;
        float   dataMinus = 0;
        int     dpCount = 0;
        int     dmCount = 0;
        int     idx = 0;

        while (frameCount < numFrames)
        {
            float newData = sampleData[frameCount*numChannels];

            if (numChannels == 2)
            {
                newData += sampleData[frameCount*numChannels + 1];
                newData /= 2;
            }

            if(newData > 0)
            {
                dataPlus += newData;
                dpCount++;
            }
            else if(newData < 0)
            {
                dataMinus += abs(newData);
                dmCount++;
            }

            if((int)(ratio*frameCount) > idx)
            {
                if(dpCount > 0)
                {
                    dataPlus /= (dpCount);
                    arrayVals[idx*2] = dataPlus;

                    if(dataPlus > dMax)
                        dMax = dataPlus;
                }

                if(dmCount > 0)
                {
                    dataMinus /= (dmCount);
                    arrayVals[idx*2 + 1] = dataMinus;

                    if(dataMinus > dMax)
                        dMax = dataMinus;
                }

                dataPlus = dataMinus = 0;
                dpCount = dmCount = 0;

                idx++;
            }

            frameCount++;
        }

        waveImage = new Image(Image::ARGB, pixels, MGrid->getLineHeight() - 1, true);

        Graphics    imageContext(*(waveImage));
        int         halfHeight = int(float(MGrid->getLineHeight() - 3) / 2);
        int         yCenter = halfHeight + 1;
        long        sampleCount = 0;
        float       normFactor = 1.f;

        if(dMax > 0)
        {
            normFactor = 1.f/dMax;
        }

        frameCount = 0;

        //imageContext.setColour(Colour(getColor()).withBrightness(.9f));
        setMyColor(imageContext, .9f);

        for(int pixCount = 0; pixCount < pixels; pixCount++)
        {
            int valP = RoundFloat(normFactor * halfHeight * arrayVals[pixCount*2]);
            int valM = RoundFloat(normFactor * halfHeight * arrayVals[pixCount*2 + 1]);

            int len = valP + valM;
            int yTop = yCenter - valP;
            int yBot = yCenter + valM;

            gLineVertical(imageContext, pixCount, yTop, yBot + 1);

            //imageContext.setPixel(pixCount, yTop);
            //imageContext.setPixel(pixCount, yBot);
        }

        free(arrayVals);
    }

/*
    while(xCoord < pixels)
    {
        float data = sample_data[frameCount];
        float data0 = frameCount > 0 ? sample_data[frameCount - 1] : data;
        float data1 = frameCount < sample_info.frames-1 ? sample_data[frameCount + 1] : data;

        if(sample_info.channels == 2)
        {
            data += sample_data[frameCount + 1];
            data /= 2;
        }

        int yCoord = yCenter - RoundFloat(halfHeight*data) + 2;
        int yCoord0 = yCenter - RoundFloat(halfHeight*data0) + 2;
        int yCoord1 = yCenter - RoundFloat(halfHeight*data1) + 2;

        gLine(imageContext, xCoord-1, yCoord0, xCoord, yCoord0);
        gLine(imageContext, xCoord, yCoord, xCoord + 1, yCoord1);

        xCoord++;

        frameCount = long(float(xCoord)/pixelRatio);
    }
    */
}



