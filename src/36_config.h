
#pragma once

#include "36_globals.h"



#define USE_OLD_JUCE
#define DAW_WINDOWS_ARE_DETACHED                1
#define DAW_SINGLE_SUBWINDOW                    1
#define DAW_DEFAULT_WINDOW_WIDTH                1920
#define DAW_DEFAULT_WINDOW_HEIGHT               1080

#define MAX_NAME_LENGTH                         100
#define MAX_PATH_LENGTH                         256 // (260)
#define MAX_ALIAS_STRING                        10
#define MAX_BUFF_SIZE                           16896 // max buffer size is equal to max samples size
#define NUM_CHANNELS                            36
#define NUM_SENDS                               3

#define BUFF_CHUNK_SIZE                         64
#define WAVETABLE_SIZE                          32768
#define DEFAULT_BUFFER_SIZE                     2048
#define DEFAULT_SAMPLE_RATE                     44100
#define NUM_PATTERN_LINES                       100
#define NUM_PIANOROLL_LINES                     120

#define SAMPLES_PATH                            "Samples\\"
#define PROJECTS_PATH                           "Projects\\"
#define PLUGIN_LIST_FILENAME                    "vst_fxlist.lst"

#define DECLICK_COUNT                           128    // should not be greater than 256 or too small

#define PI_F                                    3.14159265f

#define LOCAL_PLUGIN_FOLDER                     "Plugins\\"
#define VST_EXT_PATH_1                          "C:\\Program Files\\Steinberg\\VstPlugins\\"
#define VST_EXT_PATH_2                          "C:\\Program Files\\VstPlugins\\"


// for 6 dB max
//#define VolRange                  (1.41253754f)
//#define InvVolRange               (0.70794578f)

// for 3 dB max
//#define VolRange                  (1.18850222f)
//#define InvVolRange               (0.84139514f)

// for 4 dB max
//#define VolRange                  (1.25892541f)
//#define InvVolRange               (0.79432823f)

// for 6 dB max

#define DAW_VOL_RANGE                           1.41421356f
#define DAW_INVERTED_VOL_RANGE                  0.70710678f

#define DAW_MAINWINDOW_TITLEBAR_HEIGHT          20
#define DAW_CHILDWINDOW_TITLEBAR_HEIGHT         20

#define EffHeaderHeight                         30

#define MenuColor                               0xffFFE080


#define SETTING_STEPMODE                        "Step Mode"


class ConfigData
{
protected:

public:

        ConfigData();   
};


