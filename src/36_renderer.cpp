

#include "36_renderer.h"
#include "36_pattern.h"
#include "36_audio_dev.h"
#include "36_juce_components.h"
#include "36.h"
#include "36_transport.h"
#include "36_audiomanager.h"
#include "36_events_triggers.h"



#define RNDR_THREAD_ERROR ((DWORD) - 1)

Renderer::Renderer()
{
    memset(&(this->config), 0, sizeof(RenderConfig));

    this->config.inbuff_len = 512;
    this->RENDER_BUFF = NULL;
    this->state = RNDR_STATUS_INVALID;
    this->pCallBack = NULL;
    this->out_file = NULL;
    this->RndrThread = 0;
    this->ullCurrentFramePos = 0;
    this->ullSongFrameLength = 0;
    this->renderMutex = CreateMutex(NULL, FALSE, NULL);
}

Renderer::~Renderer()
{
    if (this->RENDER_BUFF != NULL)
    {
        free(this->RENDER_BUFF);
    }
}

void Renderer::AcquireSema()
{
    WaitForSingleObject(this->renderMutex,INFINITE);
}

void Renderer::ReleaseSema()
{
    ReleaseMutex(this->renderMutex);
}

/* API to configure renderer parameters */
//
Render_Error Renderer::SetConfig(const RenderConfig* p_config_data)
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if (NULL != p_config_data)
    {
        if ((this->state == RNDR_STATUS_INVALID) || (this->state == RNDR_STATUS_READY))
        {
            if (p_config_data->inbuff_len != this->config.inbuff_len)
            {
                if (this->RENDER_BUFF != NULL)
                {
                    free(this->RENDER_BUFF);

                    this->RENDER_BUFF = (float*) malloc(p_config_data->inbuff_len * sizeof(float) * 2);

                    if (this->RENDER_BUFF == NULL)
                    {
                        ret_val = RNDR_ERROR_MEM_ALLOC_FAIL;
                    }
                }
            }

            memcpy(&(this->config), p_config_data, sizeof(RenderConfig));
        }
        else
        {
            ret_val = RNDR_ERROR_API_MISUSE;
        }
    }
    else
    {
        ret_val = RNDR_ERROR_INVALID_PARAMS;
    }

    return (ret_val);
}

/* API to get current renderer parameters config */
Render_Error Renderer::GetConfig(const RenderConfig** pp_config_data)
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if (NULL != pp_config_data)
    {
        *pp_config_data = &(this->config);
    }
    else
    {
        ret_val = RNDR_ERROR_INVALID_PARAMS;
    }

    return (ret_val);
}

Render_Error Renderer::GetCurrentPos(long & ullCurrentFramePos)
{
    ullCurrentFramePos = this->ullCurrentFramePos;

    return RNDR_NO_ERROR;
}

Render_Error Renderer::SetAudioLength(long ullAudioLength)
{
    this->ullSongFrameLength = ullAudioLength;

    return RNDR_NO_ERROR;
}

Render_Error Renderer::GetAudioLength(long & ullAudioLength)
{
    ullAudioLength = this->ullSongFrameLength;

    return RNDR_NO_ERROR;
}

/* as result: renderer will prepare internal variables, open a file stream and create a rendering thread */
Render_Error Renderer::Open()
{
    Render_Error ret_val = RNDR_NO_ERROR;

    SF_INFO      sfinfo  = {0};

    double  vorbis_quality = 0.0;

    sfinfo.channels = 2;
    sfinfo.samplerate = (int)MAudio->getSampleRate();
    sfinfo.seekable = 0;

    /* Lets define format */

    switch(this->config.format)
    {
        case RNDR_FORMAT_WAVE:
            sfinfo.format = SF_FORMAT_WAV;
            break;
        case RNDR_FORMAT_OGG:
            sfinfo.format = (SF_FORMAT_OGG | SF_FORMAT_VORBIS);
            break;
        case RNDR_FORMAT_FLAC:
            sfinfo.format = SF_FORMAT_FLAC;
            break;
        case RNDR_FORMAT_MP3:
            break;

        default:
            break;
    }

    switch (this->config.quality)
    {
        case RNDR_QUALITY_8BIT:

            if( ((sfinfo.format & 0x0FFF0000) == SF_FORMAT_WAV) ||
                ((sfinfo.format & 0x0FFF0000) == SF_FORMAT_RAW) )
            {
                sfinfo.format |= SF_FORMAT_PCM_U8;
            }
            else if ((sfinfo.format & 0x0FFF0000) == SF_FORMAT_FLAC)
            {
                sfinfo.format |= SF_FORMAT_PCM_S8;
            }
            break;

        case RNDR_QUALITY_16BIT:
            sfinfo.format |= SF_FORMAT_PCM_16;
            break;
        case RNDR_QUALITY_24BIT:
            sfinfo.format |= SF_FORMAT_PCM_24;
            break;
        case RNDR_QUALITY_32BIT:
            sfinfo.format |= SF_FORMAT_PCM_32;
            break;
        case RNDR_QUALITY_FLOAT:
            sfinfo.format |= SF_FORMAT_FLOAT;
            break;
        case RNDR_QUALITY_VORBIS_POOR:
            vorbis_quality = 0.1;
            break;
        case RNDR_QUALITY_VORBIS_GOOD:
            vorbis_quality = 0.3;
            break;
        case RNDR_QUALITY_VORBIS_BEST:
            vorbis_quality = 0.6;
            break;
        case RNDR_QUALITY_VORBIS_BRILIANT:
            vorbis_quality = 1.0;
            break;
        case RNDR_QUALITY_VORBIS_0:
            vorbis_quality = 0.0;
            break;
        case RNDR_QUALITY_VORBIS_1:
            vorbis_quality = 0.1;
            break;
        case RNDR_QUALITY_VORBIS_2:
            vorbis_quality = 0.2;
            break;
        case RNDR_QUALITY_VORBIS_3:
            vorbis_quality = 0.3;
            break;
        case RNDR_QUALITY_VORBIS_4:
            vorbis_quality = 0.4;
            break;
        case RNDR_QUALITY_VORBIS_5:
            vorbis_quality = 0.5;
            break;
        case RNDR_QUALITY_VORBIS_6:
            vorbis_quality = 0.6;
            break;
        case RNDR_QUALITY_VORBIS_7:
            vorbis_quality = 0.7;
            break;
        case RNDR_QUALITY_VORBIS_8:
            vorbis_quality = 0.8;
            break;
        case RNDR_QUALITY_VORBIS_9:
            vorbis_quality = 0.9;
            break;
        case RNDR_QUALITY_VORBIS_10:
            vorbis_quality = 1.0;
            break;
        default:
            break;
    }

    if (this->RENDER_BUFF != NULL)
    {
        free(this->RENDER_BUFF);
    }

    this->RENDER_BUFF = (float*)malloc(this->config.inbuff_len * sizeof(float) * 2);

    if (this->RENDER_BUFF == NULL)
    {
        ret_val = RNDR_ERROR_MEM_ALLOC_FAIL;
    }

    if (ret_val == RNDR_NO_ERROR)
    {
        this->out_file = sf_open(this->config.file_name, SFM_WRITE, &sfinfo);

        if (this->out_file != 0)
        {
            if (sfinfo.format & SF_FORMAT_WAV)
            {
                sf_command(this->out_file, SFC_SET_NORM_FLOAT, NULL, SF_TRUE);
                sf_command(this->out_file, SFC_SET_ADD_PEAK_CHUNK, NULL, SF_TRUE);
                sf_command(this->out_file, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE);
            }
            else if (sfinfo.format & SF_FORMAT_OGG)
            {
                sf_command(this->out_file, SFC_SET_VBR_ENCODING_QUALITY,
                                                    &(vorbis_quality), sizeof(double));
            }

            /* Lets create the thread */

            this->RndrThread = CreateThread(NULL, //No security settings
                                            0, // default stack size
                                            (LPTHREAD_START_ROUTINE)&(this->Render_ThreadProc),
                                            (void*)this,
                                            CREATE_SUSPENDED,
                                            NULL);

            if (NULL != this->RndrThread)
            {
                this->state = RNDR_STATUS_READY;
            }
            else
            {
                ret_val = RNDR_ERROR_INTERNAL_FAIL;

                /* Close file handle */

                sf_close(this->out_file);

                this->out_file = NULL;
            }
        }
        else
        {
            ret_val = RNDR_ERROR_OPEN_FILE_FAILED;
        }
    }

    return (ret_val);
}

/* renderer will start a rendering thread requesting client's callback function for every portion of samples */
Render_Error Renderer::Start(RndrCallback* p_callback_func)
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if (NULL != p_callback_func)
    {
        if ((0 == this->RndrThread) || (this->state != RNDR_STATUS_READY))
        {
            ret_val = RNDR_ERROR_API_MISUSE;
        }
        else
        {
            this->pCallBack = p_callback_func;

            this->ullCurrentFramePos = 0;

            this->state = RNDR_STATUS_DATA_PROCESSING;

            if (RNDR_THREAD_ERROR == ResumeThread(this->RndrThread))
            {
                ret_val = RNDR_ERROR_INTERNAL_FAIL;

                this->state = RNDR_STATUS_INVALID;
            }
        }
    }
    else
    {
        ret_val = RNDR_ERROR_INVALID_PARAMS;
    }

    return (ret_val);
}

// pause a rendering thread for a while.
//
Render_Error Renderer::Pause()
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if ( (0 == this->RndrThread) || (this->state == RNDR_STATUS_READY) || (this->state == RNDR_STATUS_INVALID) )
    {
        ret_val = RNDR_ERROR_API_MISUSE;
    }
    else
    {
        if (RNDR_THREAD_ERROR != SuspendThread(this->RndrThread))
        {
            this->state = RNDR_STATUS_READY;
        }
        else
        {
            ret_val = RNDR_ERROR_INTERNAL_FAIL;
        }
    }

    return (ret_val);
}

Render_Error Renderer::Resume()
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if (this->state != RNDR_STATUS_PAUSED)
    {
        ret_val = RNDR_ERROR_API_MISUSE;
    }
    else if (RNDR_THREAD_ERROR == ResumeThread(this->RndrThread))
    {
        ret_val = RNDR_ERROR_INTERNAL_FAIL;

        this->state = RNDR_STATUS_INVALID;
    }
    else
    {
        this->state = RNDR_STATUS_DATA_PROCESSING;
    }

    return (ret_val);
}

Render_Status Renderer::GetState()
{
    return this->state;
}
/* stop rendering thread. Calling of Start() again will start a rendering from the beginning*/
Render_Error Renderer::Stop()
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if ( (0 == this->RndrThread)             ||
         (this->state == RNDR_STATUS_READY)  ||
         (this->state == RNDR_STATUS_INVALID) )
    {
        ret_val = RNDR_ERROR_API_MISUSE;
    }
    else
    {
        this->state = RNDR_STATUS_THREAD_STOPPING;
    }

    return (ret_val);
}

/* Stop and close rendering thread, close file stream */
Render_Error Renderer::Close()
{
    Render_Error ret_val = RNDR_NO_ERROR;

    if (0 == this->RndrThread)
    {
        ret_val = RNDR_ERROR_API_MISUSE;
    }
    else
    {
        /* Critical place start */
        this->AcquireSema();

        if (TRUE == CloseHandle(this->RndrThread))
        {
            this->state = RNDR_STATUS_INVALID;
        }
        else
        {
            ret_val = RNDR_ERROR_INTERNAL_FAIL;
        }

        this->RndrThread = 0;

        if (this->out_file != 0)
        {
            sf_close(this->out_file);
        }

        this->out_file = 0;

        /* Critical place end */

        this->ReleaseSema();
    }

    return (ret_val);
}

DWORD Renderer::Render_ThreadProc(LPVOID lpParam)
{
    DWORD           ret_val = 0;
    Renderer*       pThis = (Renderer*) lpParam;
    RenderStatus    ret_code = RNDR_Abort;
    RndrCallback*   pCallBack = pThis->pCallBack;
    //length of the buffer should be no greater than total frames count
    unsigned long	uiBufLength = min(pThis->config.inbuff_len, unsigned long(pThis->ullSongFrameLength - pThis->ullCurrentFramePos));
    unsigned int    counter = 0;

    if (NULL != pCallBack)
    {
        while ((RNDR_STATUS_DATA_PROCESSING == pThis->state) &&
                    (pThis->ullCurrentFramePos < pThis->ullSongFrameLength + 50) &&
                          (RNDR_Continue == pCallBack(pThis->RENDER_BUFF, uiBufLength)))
        {
        // Critical section start
            pThis->AcquireSema();
            //ret_code = pCallBack(RENDER_BUFF, (sizeof(RENDER_BUFF)/sizeof(float)) / pThis->config.num_channels);
            sf_write_float(pThis->out_file, pThis->RENDER_BUFF, pThis->config.inbuff_len *2);
            ++counter;
            pThis->ullCurrentFramePos += uiBufLength;
            uiBufLength = min(uiBufLength, unsigned long(pThis->ullSongFrameLength - pThis->ullCurrentFramePos + 50));
            if (counter > 300)
            {
                sf_write_sync(pThis->out_file);
                counter = 0;
            }

        // Critical section end

            pThis->ReleaseSema();
        }

           // ExitThread(0);
           // pThis->RndrThread = 0;

        if ((counter < 300) && (counter > 0))
        {
            sf_write_sync(pThis->out_file);
        }

        pThis->AcquireSema();
        pThis->state = RNDR_STATUS_READY;
        pThis->ReleaseSema();
    }

    //ret_val = STILL_ACTIVE;
    return ret_val;
}


RenderStatus Audio_RenderCallback(void *sample_buff, unsigned long frameCount)
{
    RenderStatus ret_val = RNDR_Continue;
    if (GPlaying != true)
    {
        ret_val = RNDR_Complete;
    }
    else
    {
        //PortAudio_StreamCallback(NULL, sample_buff, frameCount, NULL, 0, NULL);
        MAudio->generalCallBack(NULL, sample_buff, frameCount);
    }
    return ret_val;
}

bool Handle_RenderStart()
{
    long lastFrame = MPattern->getLastElementFrame();
    if(lastFrame > 0 && GPlaying == false)
    {
        if(Render_isWorking == false)
        {
            GoToHome();

            JAudManager->removeAudioCallback((AudioIODeviceCallback*)JAudCallBack);

            TogglePlayback();

            Render_isWorking = true;
            Render_.SetConfig(&Render_Config);
            Render_.SetAudioLength(lastFrame);
            Render_.Open();
            Render_.Start(&Audio_RenderCallback);
        }
        else
        {
            MPattern->queueEvent();

            Render_.SetAudioLength(lastFrame);
            Render_.Resume();
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Handle_RenderStop()
{
    if (Render_isWorking == true)
    {
        Render_isWorking = false;
        Render_.Stop();
        Render_.Close();

        JAudManager->addAudioCallback((AudioIODeviceCallback*)JAudCallBack);
    }

    StopPlayback();
}

bool            Render_isWorking;

// Renderer

Renderer        Render_;
RenderConfig    Render_Config;


void Init_Renderer()
{
    ///Render_ = new Renderer();

    strcpy(Render_Config.file_name, ".\\test.wav");

    Render_Config.format = RNDR_FORMAT_WAVE;
    Render_Config.inbuff_len = 2048;
    Render_Config.quality = RNDR_QUALITY_32BIT;

    String str(T(".\\Rendered\\"));

    Render_Config.output_dir = str;
    Render_Config.q1 = 2;
    Render_Config.q2 = 5;
    Render_.SetConfig(&Render_Config);
}

