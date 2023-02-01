

#pragma once

#include "sndfile.h"

#include "36_globals.h"


#define RENDERING_DEFAULT_FOLDER       ("Rendered\\")


//  Global pointer to instance of Rendering module. This module provides set of interfaces to configure rendering parameters
//  Once started, renderer will request chunks of samples by invoking callback routine provided by main app.

//  extern Renderer*        pRenderer;


extern Renderer         Render_;
extern RenderConfig     Render_Config;
extern bool             Render_isWorking;   // Whether rendering is going on



#define RNDR_MAX_FILENAME_LENGTH 300

/* Codes which client should provide */
typedef enum
{
    RNDR_Continue = 0,
    RNDR_Complete,
    RNDR_Abort
} RenderStatus;

// Prototype of callback function
typedef RenderStatus RndrCallback(void *sample_buff, unsigned long frameCount);

//extern
//DWORD RNDR_ThreadProc(LPVOID lpParam);


// Internal states of the renderer module
typedef enum
{
    RNDR_STATUS_READY = 0,
    RNDR_STATUS_IO_READING,
    RNDR_STATUS_IO_WRITING,
    RNDR_STATUS_DATA_REQUESTED,
    RNDR_STATUS_DATA_PROCESSING,
    RNDR_STATUS_THREAD_STOPPING,
    RNDR_STATUS_PAUSED,
    RNDR_STATUS_RUNNING_IDLE,
    RNDR_STATUS_INVALID
} Render_Status;

// error codes which renderer can return
typedef enum
{
    RNDR_NO_ERROR = 0,
    RNDR_ERROR_WRITE_FILE,
    RNDR_ERROR_READ_FILE,
    RNDR_ERROR_INVALID_PARAMS,
    RNDR_ERROR_INTERNAL_FAIL,
    RNDR_ERROR_MEM_ALLOC_FAIL,
    RNDR_ERROR_API_MISUSE,
    RNDR_ERROR_OPEN_FILE_FAILED,
    RNDR_ERROR_MAX
} Render_Error;

typedef enum
{
    RNDR_FORMAT_WAVE,
    RNDR_FORMAT_OGG,
    RNDR_FORMAT_FLAC,
    RNDR_FORMAT_MP3
} Render_Format;

enum
{
    RNDR_QUALITY_8BIT = 0,
    RNDR_QUALITY_16BIT,
    RNDR_QUALITY_24BIT,
    RNDR_QUALITY_32BIT,
    RNDR_QUALITY_FLOAT,
    RNDR_QUALITY_VORBIS_BASE,
    RNDR_QUALITY_VORBIS_0,
    RNDR_QUALITY_VORBIS_1,
    RNDR_QUALITY_VORBIS_2,
    RNDR_QUALITY_VORBIS_3,
    RNDR_QUALITY_VORBIS_4,
    RNDR_QUALITY_VORBIS_5,
    RNDR_QUALITY_VORBIS_6,
    RNDR_QUALITY_VORBIS_7,
    RNDR_QUALITY_VORBIS_8,
    RNDR_QUALITY_VORBIS_9,
    RNDR_QUALITY_VORBIS_10,
    RNDR_QUALITY_VORBIS_POOR,       /* map to quality 1 */
    RNDR_QUALITY_VORBIS_GOOD,       /* map to quality 4 */
    RNDR_QUALITY_VORBIS_BEST,       /* map to quality 6 */
    RNDR_QUALITY_VORBIS_BRILIANT,   /* map to quality 10 */
    RNDR_QUALITY_MAX
};

typedef unsigned int DawRenderQuality;

typedef struct RenderConfig
{
    // Lenght of input buffer in frames. Frame is a set of sample values for each channel.
    //   E.g. if num_channels is 2 then frame is just a pair of float values

    unsigned long inbuff_len;

    // Output file format. Only wav/flac/ogg are currently suported formats

    Render_Format format;

    // Quality variables. One common and three separate for each format (applicable for saving)
    unsigned int  quality;

    int q1;
    int q2;

    // File name (with full path) to render to

    char          file_name[RNDR_MAX_FILENAME_LENGTH];
    String        output_dir;
} DawRenderConfig;

class Renderer
{
public:

        Renderer();
       ~Renderer();

        // API to configure renderer parameters
        Render_Error    SetConfig(const RenderConfig* p_config_data);
        // API to get current renderer parameters config
        Render_Error    GetConfig(const RenderConfig** pp_config_data);
        //API to set the length of the project being rendered (in amount of frames)
        Render_Error    SetAudioLength(long ullAudioLength);
        //API to get the length of the project being rendered (in amount of frames)
        Render_Error    GetAudioLength(long & ullAudioLength);
        //API to get current rendering position (calculated in frames)
        Render_Error    GetCurrentPos(long & ullCurrentFramePos);
        Render_Status   GetState();
        // as result: renderer will prepare internal variables, open a file stream and create a rendering thread
        Render_Error    Open();
        // renderer will start a rendering thread requesting client's callback function for every portion of samples
        Render_Error    Start(RndrCallback* p_callback_func);
        // pause a rendering thread for a while. If call Start() again, rendering will resume
        Render_Error    Pause();
        // resume paused rendering
        Render_Error Renderer::Resume();
        // stop rendering thread. Calling of Start() again will start a rendering from the beginning
        Render_Error    Stop();
        // Stop and close rendering thread, close file stream 
        Render_Error    Close();

private:

        // semaphore handle 
        HANDLE          renderMutex;

        // Callback function 
        RndrCallback*   pCallBack;

        // Current state of renderer 
        Render_Status   state;
        // Configuration record 
        RenderConfig    config;
        //_MainObject thread procedure
        static DWORD    Render_ThreadProc(LPVOID lpParam);
        HANDLE          RndrThread;
        SNDFILE*        out_file;
        float*          RENDER_BUFF;
        void*           ThreadParam;
        void            AcquireSema();
        void            ReleaseSema();

        //Current position of the renderer
        long            ullCurrentFramePos;

        //Total length of the song being rendered (in frames)
        long            ullSongFrameLength;
};

    void            Init_Renderer();
    bool            Handle_RenderStart();
    void            Handle_RenderStop();


