

#include "36.h"
#include "36_project.h"
#include "36_audio_dev.h"
#include "36_renderer.h"
#include "36_ctrlpanel.h"
#include "36_instrpanel.h"
#include "36_sampleinstr.h"
#include "36_instr.h"
#include "36_effects.h"
#include "36_keyboard.h"
#include "36_pattern.h"
#include "36_button.h"
#include "36_browser.h"
#include "36_history.h"
#include "36_utils.h"
#include "36_grid.h"
#include "36_vu.h"
#include "36_transport.h"
#include "36_textinput.h"
#include "36_edit.h"
#include "36_transport.h"
#include "36_audiomanager.h"
#include "36_alertbox.h"





// Project class instance

Project36       MProject;

extern int      rVer;
extern int      rDay;
extern int      rMonth;
extern int      rYear;

int             rVer = 101;
int             rDay = 7, rMonth = 7, rYear = 2010;



LoadThread::LoadThread(File f) : ThreadWithProgressWindow (T("Loading saved project..."), false, false)
{
    setStatusMessage (T("Loading saved project..."));

    projectfile = f;
}

LoadThread::~LoadThread()
{
}

void LoadThread::run()
{
    const MessageManagerLock mmlock;

    MProject.loadProjectData(projectfile, this);
}


void Project36::init()
{
    newProj = true;

    setName("Untitled");

    projectPath;

   //memset(projpath, 0, MAX_PATH_STRING);

    projectFile = NULL;

    loadSavedSettings();

    loading = false;

    resetChange();
}

void Project36::setName(String name)
{
    if(name.length()<= MAX_NAME_LENGTH)
    {
        name.copyToBuffer(projName, MAX_NAME_LENGTH);
    }
}

void Project36::releaseAllOnExit()
{
    MProject.saveSettings();

    //MInstrPanel->setEnable(false);

/*
    while(MInstrPanel->getInstrs().size() > 0)
    {
        MInstrPanel->deleteInstrument(MInstrPanel->getInstrs().front());
    }*/

    // Erase remembered session files

    for(int si = 0; si < numLastSessions; si++)
    {
        delete lastSessions[si];
    }

    if (WorkDirectory != NULL)
    {
        free(WorkDirectory);
    }
}


void Project36::setChange()
{
    if(!changeHappened)
    {
        MObject->redraw();
    }

    changeHappened = true;

    if(!isLoading())
    {
        MWindow->updateTitle(); 
    }
}

const char* Project36::getName()
{
    return (const char*)projName;
}

void Project36::deleteAllElems()
{
    std::string ch = MWindow->showAlertBox("Clean all elements (no undo)?", "Yes", "No");

    if(ch == "Yes") // if they picked 'OK'
    {
        if(GPlaying == true)
        {
            MTransp->stopPlayback(true);

            GetButton(MCtrllPanel, "bt.play")->release();
        }

        MPattern->deleteAllElements(true, true);

        MObject->redraw();
    }
    else
    {
        return;
    }
}

void Project36::deleteProject()
{
    WaitForSingleObject(AudioMutex, INFINITE);

    if(GPlaying == true)
    {
        MTransp->stopPlayback(true);

        GetButton(MCtrllPanel, "bt.play")->release();
    }

/*
    while(MInstrPanel->getInstrs().size() > 0)
    {
        MInstrPanel->deleteInstrument(MInstrPanel->getInstrs().front());
    }*/

    MPattern->deleteAllElements(true, true);

    SoloInstr = NULL;

    SoloMixChannel = NULL;

    Octave = 5;

    MTransp->reset();

    // Position cursor, playback pos and mainbar

    MGrid->setPixelsPerTick(6);

    // Reset scrollers here (todo)

    projectPath = "";

    setName("Untitled");

    resetChange();

    MWindow->updateTitle();

    ReleaseMutex(AudioMutex);
}

void Project36::sortLastSessions(File* newEntry)
{
    if(newEntry != NULL)
    {
        File* got;
        File* put = NULL;
        bool replaced = false;

        for(int si = 0; si < numLastSessions; si++)
        {
            if(*lastSessions[si] != *newEntry)
            {
                got = lastSessions[si];
                lastSessions[si] = put;
                put = got;
            }
            else
            {
                lastSessions[0] = lastSessions[si];

                if(put != NULL)
                {
                    lastSessions[si] = put;
                }

                replaced = true;
                break;
            }
        }

        if(replaced == false)
        {
            lastSessions[0] = new File(*newEntry);

            if(numLastSessions < 10)
            {
                if(numLastSessions > 0)
                {
                    lastSessions[numLastSessions] = put;
                }

                numLastSessions++;
            }
        }
    }
}

bool Project36::doesSessionExist(const char* title)
{
    for(int si = 0; si < numLastSessions; si++)
    {
        if(strcmp(lastSessions[si]->getFileName(), title) == 0)
        {
            return true;
        }
    }

    return false;
}

void Project36::initSessions(XmlElement* xmlSettings)
{
    String elname = "Session" + String(0);

    XmlElement* xmlSession = NULL;

    xmlSession = xmlSettings->getChildByName(elname);

    while(xmlSession != NULL)
    {
        lastSessions[numLastSessions] = new File(xmlSession->getStringAttribute(T("FilePath"), T("")));
    
        numLastSessions++;
    
        elname = "Session" + String(numLastSessions);
    
        xmlSession = xmlSettings->getChildByName(elname);
    }

    sortLastSessions(NULL);
}

void Project36::loadElementsFromNode(XmlElement* xmlMainNode, Pattern* pttarget)
{
    XmlElement* xmlChild = NULL;

    forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Element"))
    {
        ElemType type = (ElemType)xmlChild->getIntAttribute(T("Type"));
        float stick = (float)xmlChild->getDoubleAttribute(T("StartTick"));
        float etick = (float)xmlChild->getDoubleAttribute(T("EndTick"));
        float ticklength = (float)xmlChild->getDoubleAttribute(T("TickLength"));
        int trackline = xmlChild->getIntAttribute(T("TrackLine"));
        int pattindex = xmlChild->getIntAttribute(T("PattIndex"));
    }
}

bool Project36::loadProjectData(File chosenFile, LoadThread* thread)
{
    loading = true;

    bool retval = false;
    XmlDocument myProject(chosenFile);
    XmlElement* xmlMainNode = myProject.getDocumentElement();

    if(NULL == xmlMainNode)
    {
        MWindow->showAlertBox("Can't open project file");
    }
    else
    {
        if(xmlMainNode->hasTagName(T("ChaoticProject")))
        {
            MTransp->setTicksPerBeat(xmlMainNode->getIntAttribute(T("TPB")));
            MTransp->setBeatsPerBar(xmlMainNode->getIntAttribute(T("BPB")));

            Octave = xmlMainNode->getIntAttribute(T("Octave"));

            // Load and set master volume
/*
            XmlElement* xmlChild = NULL;
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Parameter"))
            {
                String sname = xmlChild->getStringAttribute(T("name"));

                if(sname == T("MasterVolume"))
                {
                //    MasterVol->Load(xmlChild);
                }
            }*/

            // Load all instruments
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Instrument"))
            {
                InstrType itype = (InstrType)xmlChild->getIntAttribute(T("InstrType"));

                String iname = xmlChild->getStringAttribute(T("InstrName"));
                String ipath = xmlChild->getStringAttribute(T("InstrPath"));

                File f(ipath);

                if(thread != NULL)
                {
                    thread->setStatusMessage(T("Loading instrument: ") + iname);
                }

                Instrument* instr = NULL;
/*
                if(itype == Instr_Sample)
                {
                    instr = MInstrPanel->addSample((const char*)ipath);
                }
                else if(itype == Instr_VstPlugin)
                {
                    instr = MInstrPanel->addVst((const char*)ipath, NULL);
                }*/

                if(instr != NULL)
                {
                    instr->load(xmlChild);
                }
            }

            // Load all effects into mixchannels
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("MixChannel"))
            {
                char indexstr[15];
                MixChannel* mchan = NULL;

                xmlChild->getStringAttribute(T("Index")).copyToBuffer(indexstr, 15);

                // todo: load mixchannel with instrument
                mchan = NULL;

                if(mchan != NULL)
                {
                    //mchan->load(xmlChild);

                    forEachXmlChildElementWithTagName(*xmlChild, xmlChildEff, T("Effect"))
                    {
                        //DevType etype = (DevType)xmlChildEff->getIntAttribute(T("DevType"));
                        String ename = xmlChildEff->getStringAttribute(T("EffName"));
                        String epath = xmlChildEff->getStringAttribute(T("EffPath"));

/*
                        if(etype == DevType_VstPlugin)
                        {
                            File f(epath);

                            if(thread != NULL)
                            {
                                thread->setStatusMessage(T("Loading effect: ") + f.getFileName());
                            }
                        }
                        else
                        {
                            if(thread != NULL)
                            {
                                thread->setStatusMessage(T("Loading effect: ") + ename);
                            }
                        }

                        Eff* eff = NULL;
                        if(etype != EffType_VSTPlugin)
                        {
                            eff = _MMixer->AddEffectByType(etype, mchan);
                        }
                        else
                        {
                            eff = _MMixer->AddVstEffectByPath((const char*)epath, mchan);
                        }

                        if(eff != NULL)
                        {
                            eff->Load(xmlChildEff);
                        }
*/
                    }
                }
            }

            // Load all patterns

            if(thread != NULL)
            {
                thread->setStatusMessage(T("Loading patterns..."));
            }

            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Pattern"))
            {
               // int origindex = xmlChild->getIntAttribute(T("PattIndex"));

                bool isauto = xmlChild->getBoolAttribute(T("IsAuto"));

                if(isauto)
                {
                    Instrument* instr = MInstrPanel->getInstrByIndex(xmlChild->getIntAttribute(T("InstrOwnerIndex"), -1));

                    ///
                }
                else
                {
                    char pattname[MAX_NAME_LENGTH];

                    xmlChild->getStringAttribute(T("Name")).copyToBuffer(pattname, MAX_NAME_LENGTH);
                    Pattern* patt = new Pattern(pattname, -1, -1, -1, -1, true);
                    
                    //patt->momIndex = origindex;

                    patternList.push_front(patt);

                    //GetPatternNameImage(patt);

                    patt->setBasePattern(patt);

                    XmlElement* xmlDerived = NULL;

                    forEachXmlChildElementWithTagName(*xmlChild, xmlDerived, T("Derived"))
                    {
                        float stick = (float)xmlDerived->getDoubleAttribute(T("StartTick"));
                        float etick = (float)xmlDerived->getDoubleAttribute(T("EndTick"));

                        float ticklength = (float)xmlDerived->getDoubleAttribute(T("TickLength"));

                        int trackline = xmlDerived->getIntAttribute(T("TrackLine"));

                        Element* el = NULL;

                        Pattern* ptder = new Pattern(pattname, stick, etick, trackline, trackline, false);

                        ptder->load(xmlDerived);

                        patt->addInstance(ptder);

                        //AddElement(ptder);
                    }
                }
            }

            // And load all elements, of course

            if(thread != NULL)
            {
                thread->setStatusMessage(T("Loading elements..."));
            }

            loadElementsFromNode(xmlMainNode, NULL);

            //MInstrPanel->setCurrInstr(NULL);
        }
    }

    newProj = false;

    setName(chosenFile.getFileNameWithoutExtension());
    projectPath = chosenFile.getFullPathName();

    MHistory->wipeEntireHistory();

    loading = false;

    Vu_ShowCoolFalldown();

    return retval;
}

void Project36::saveProjectData(File chosenFile)
{
    XmlElement  xmlProjectMain(T("ChaoticProject"));

    xmlProjectMain.setAttribute(T("CMMVersion"), rVer);

    //XmlElement  *xmlProjectHeader = new XmlElement(T("Module"));
    //xmlProjectHeader->setAttribute(T("Name"), this->name);
    //xmlProjectMain.setAttribute(T("ProjectName"), PrjData.projname);

    xmlProjectMain.setAttribute(T("BPM"), MTransp->getBeatsPerMinute());
    xmlProjectMain.setAttribute(T("TPB"), MTransp->getTicksPerBeat());
    xmlProjectMain.setAttribute(T("BPB"), MTransp->getBeatsPerBar());
    xmlProjectMain.setAttribute(T("Octave"), Octave);

   // XmlElement* mXML = MasterVol->Save();
    // xmlProjectMain.addChildElement(mXML);


    // Save instruments
    /*
    for(Instrument* i : MInstrPanel->getInstrs())
    {
        XmlElement* xmlInstr = new XmlElement(T("Instrument"));
        i->save(xmlInstr);
        xmlProjectMain.addChildElement(xmlInstr);
        XmlElement* xmlMChan = new XmlElement(T("MixChannel"));
        //i->mixChannel->save(xmlMChan);
        xmlProjectMain.addChildElement(xmlMChan);
    }*/

    //
    // Save sends and master

    // Save patterns, elements

    for(Element* el : MPattern->getElems())
    {
        if(!el->isdel() && el->getType() != El_Pattern)
        {
            XmlElement* xmlElem = new XmlElement(T("Element"));

            el->save(xmlElem);

            xmlProjectMain.addChildElement(xmlElem);
        }
    }

    xmlProjectMain.writeToFile(chosenFile.withFileExtension("d36"), String::empty);

    setName(chosenFile.getFileNameWithoutExtension());

    resetChange();

    MWindow->updateTitle();
}

bool Project36::saveProject(bool as)
{
    SetCurrentDirectory(WorkDirectory);

    if(as == true || newProj == true)
    {
        String str = T(".\\Projects");

        str += "\\";

        str += getName();

        FileChooser fc(T("Save project as..."),
                       File(str),
                       "*.cmm",
                       true);
        
        if(fc.browseForFileToSave (true))
        {
            File chosenFile = fc.getResult();

            newProj = false;

            projectPath = chosenFile.getFullPathName();
            saveProjectData(chosenFile);
            chosenFile = chosenFile.withFileExtension("cmm");
            sortLastSessions(&chosenFile);

            MObject->redraw();

            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        File chosenFile = File(projectPath);

        saveProjectData(chosenFile);
        sortLastSessions(&chosenFile);

        MObject->redraw();

        return false;
    }
}

// returns true if need to skip saving

bool Project36::askAndSave()
{
    if(isChanged())
    {
        std::string choice = MWindow->showAlertBox("Save changes?", "Yes", "No", "Cancel");
        
        if(choice == "Yes")
        {
            return saveProject(newProj == true);
        }
        else if (choice == "No")
        {
            return false;;
        }
        else if (choice == "Cancel")
        {
            return true;
        }
    }

    return false;
}

void Project36::loadProject(File* f)
{
    if(!askAndSave())
    {
        if(f == NULL)
        {
            SetCurrentDirectory(WorkDirectory);

            FileChooser fc (T("Choose a file to open"), File(T(".\\Projects")), "*.cmm", true);
            
            if(fc.browseForFileToOpen())
            {
                deleteProject();

                File chosenFile = fc.getResult();

                LoadThread lthread(chosenFile);

                lthread.runThread();

                //loadProjectData(chosenFile);
                sortLastSessions(f);
            }
        }
        else
        {
            deleteProject();

            LoadThread lthread(*f);

            lthread.runThread();

            //loadProjectData(*f, NULL);

            sortLastSessions(f);
        }

        resetChange();

        MWindow->updateTitle();

        MObject->redraw();
    }
}

void Project36::newProject()
{
    if(!askAndSave())
    {
        deleteProject();

        newProj = true;

        MObject->redraw();
    }
}

void Project36::saveSettings()
{
    XmlElement xmlSettings(T("DAW36 Settings"));

    //xmlSettings.setAttribute(T("MakePatternsFat"), (int)MakePatternsFat);
    xmlSettings.setAttribute(T("BufferSize"), MAudio->getBufferSize());
    xmlSettings.setAttribute(T("Interpolation"), (int)Config_DefaultInterpolation);

    XmlElement* xmlRenderSettings = new XmlElement(T("RenderSettings"));

    xmlRenderSettings->setAttribute(T("Format"), (int)Render_Config.format);
    xmlRenderSettings->setAttribute(T("Quality"), (int)Render_Config.quality);
    xmlRenderSettings->setAttribute(T("Q1"), Render_Config.q1);
    xmlRenderSettings->setAttribute(T("Q2"), Render_Config.q2);
    xmlRenderSettings->setAttribute(T("InBuffSize"), (int)Render_Config.inbuff_len);
    xmlRenderSettings->setAttribute(T("OutDir"), Render_Config.output_dir);
    xmlRenderSettings->setAttribute(T("Interpolation"), (int)Config_RenderInterpolation);

    xmlSettings.addChildElement(xmlRenderSettings);

    XmlElement* xmlLastSession;

    for(int si = 0; si < numLastSessions; si++)
    {
        xmlLastSession = new XmlElement(T("Session") + String(si));

        xmlLastSession->setAttribute(T("FilePath"), lastSessions[si]->getFullPathName());

        xmlSettings.addChildElement(xmlLastSession);
    }

#ifdef USE_JUCE_AUDIO

    XmlElement* xmlAudioSettings = JAudManager->createStateXml();

    if(xmlAudioSettings != NULL)
    {
       xmlSettings.addChildElement(xmlAudioSettings);
    }

#endif

    String  sFilePath(".\\settings.xml");
    File    sFile(sFilePath);

    xmlSettings.writeToFile(sFile, String::empty);
}

void Project36::loadSavedSettings()
{
    File sFile(T(".\\settings.xml"));
    
    if(sFile.existsAsFile())
    {
        XmlDocument sDoc(sFile);

        XmlElement* xmlSettings = sDoc.getDocumentElement();

        //MakePatternsFat = xmlSettings->getBoolAttribute(T("MakePatternsFat"), MakePatternsFat);

        Config_DefaultInterpolation = xmlSettings->getIntAttribute(T("Interpolation"), Config_DefaultInterpolation);
        MAudio->setBufferSize(xmlSettings->getIntAttribute(T("BufferSize"), MAudio->getBufferSize()));


        XmlElement* xmlRender = xmlSettings->getChildByName(T("RenderSettings"));

        if(xmlRender != NULL)
        {
            // Read renderer settings

            Render_Config.format = (Render_Format)xmlRender->getIntAttribute(T("Format"), Render_Config.format);
            Render_Config.quality = xmlRender->getIntAttribute(T("Quality"), Render_Config.quality);
            Render_Config.q1 = xmlRender->getIntAttribute(T("Q1"), Render_Config.q1);
            Render_Config.q2 = xmlRender->getIntAttribute(T("Q2"), Render_Config.q2);
            Render_Config.inbuff_len = xmlRender->getIntAttribute(T("InBuffLen"), Render_Config.inbuff_len);
            Render_Config.output_dir = xmlRender->getStringAttribute(T("OutDir"), Render_Config.output_dir);

            Config_RenderInterpolation = xmlRender->getIntAttribute(T("Interpolation"), Config_RenderInterpolation);
    
            Render_.SetConfig(&Render_Config);
        }

        MProject.initSessions(xmlSettings);

        xmlAudioSettings = xmlSettings->getChildByName(T("DEVICESETUP"));
    }
}

//};