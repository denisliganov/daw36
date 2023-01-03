
#pragma once



#include <forward_list>
#include <list>


#include "36_globals.h"
#include "36_config.h"

//namespace M {

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

//};

