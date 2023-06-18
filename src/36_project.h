
#pragma once

#include <forward_list>
#include <list>

#include "36_globals.h"


//#define _WIN32_WINNT 0x0500

//Find out wich platform we build for

//#if (defined(_WIN32) || defined(_WIN64) || defined(_WIN32_WINNT))


#define       USE_WIN32 1


typedef short   int16; 
typedef long    Tframe; 
typedef float   Tdata;


#define RELEASEBUILD FALSE


#define BUFF_PROCESSING_CHUNK_SIZE              64
// for 6 dB max
//#define VolRange                              (1.41253754f)
//#define InvVolRange                           (0.70794578f)
// for 3 dB max
//#define VolRange                              (1.18850222f)
//#define InvVolRange                           (0.84139514f)
// for 4 dB max
//#define VolRange                              (1.25892541f)
//#define InvVolRange                           (0.79432823f)
// for 6 dB max
#define DAW_VOL_RANGE                           1.41421356f
#define DAW_INVERTED_VOL_RANGE                  0.70710678f
#define DAW_MAINWINDOW_TITLEBAR_HEIGHT          20
#define DAW_CHILDWINDOW_TITLEBAR_HEIGHT         20
#define DAW_DEFAULT_WINDOW_WIDTH                1920
#define DAW_DEFAULT_WINDOW_HEIGHT               1080
#define DECLICK_COUNT                           128    // should not be greater than 256 or too small
#define DEFAULT_BUFFER_SIZE                     2048
#define DEFAULT_SAMPLE_RATE                     44100
#define LOCAL_PLUGIN_FOLDER                     "Plugins\\"
#define MAX_NAME_LENGTH                         100
#define MAX_PATH_LENGTH                         256 // (260)
#define MAX_ALIAS_STRING                        10
#define MAX_BUFF_SIZE                           16896 // max buffer size is equal to max samples size
#define NUM_CHANNELS                            36
#define NUM_SENDS                               3
#define NUM_PATTERN_LINES                       100
#define NUM_PIANOROLL_LINES                     120
#define PI_F                                    3.14159265f
#define PLUGIN_LIST_FILENAME                    "vst_fxlist.lst"
#define PATH_PROJECTS                           "Projects\\"
#define PATH_SAMPLES                            "Samples\\"
#define PATH_PRESETS                            "Presets\\"
#define USE_OLD_JUCE
#define VST_EXT_PATH_1                          "C:\\Program Files\\Steinberg\\VstPlugins\\"
#define VST_EXT_PATH_2                          "C:\\Program Files\\VstPlugins\\"

#define WAVETABLE_SIZE                          32768


#define EffHeaderHeight                         30

#define MenuColor                               0xffFFE080






class LoadThread;


class LoadThread  : public ThreadWithProgressWindow
{
public:

    LoadThread(File f);
    ~LoadThread();
    void run();

    File projectfile;
};



class Project36
{
protected:

            bool        changeHappened;
            bool        loading;
            char        projName[MAX_NAME_LENGTH];
            bool        newProj;
            File*       projectFile;
            File*       lastSessions[10];
            int         numLastSessions;
            String      projectPath;

            void        setName(String name);
            bool        doesSessionExist(const char* title);

public:


            std::forward_list<Pattern*>     patternList;


            void        init();
            void        resetChange() { changeHappened = false; }
            void        setChange();
            bool        isChanged() { return changeHappened; }
            const char* getName();
            void        deleteAllElems();
            void        deleteProject();
            void        loadElementsFromNode(XmlElement* xmlMainNode, Pattern* pttarget);
            bool        loadProjectData(File chosenFile, LoadThread* thread);
            void        saveProjectData(File chosenFile);
            bool        saveProject(bool as);
            bool        askAndSave();
            void        loadProject(File* f);
            void        newProject();
            void        saveSettings();
            void        loadSavedSettings();
            bool        isLoading() { return loading; }
            void        sortLastSessions(File* newEntry);
            void        initSessions(XmlElement* xmlSettings);
            void        releaseAllOnExit();
            String&     getProjectPath() { return projectPath; }
};



