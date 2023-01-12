//
//



#include "36_device.h"
#include "36_params.h"
#include "36_slider.h"
#include "36_button.h"
#include "36_browser.h"
#include "36_instr.h"
#include "36_ctrlpanel.h"
#include "36_mixer.h"
#include "36_instrpanel.h"
#include "36_juce_windows.h"
#include "36_brwentry.h"
#include "36_edit.h"
#include "36_events_triggers.h"

#include <direct.h>



Device36::Device36()
{
    internal = true;
    previewOnly = false;

    uniqueId = -1;

    currPreset = NULL;
    envelopes = NULL;
    guiWindow = NULL;

    paramLocked = false;

    currPresetName = "Untitled";
}

Device36::~Device36()
{
    while(params.size() > 0)
    {
        Parameter* param = params.front();

        params.remove(param);

        delete param;
    }

    deletePresets();

    if(guiWindow)
    {
        window->deleteWindow(guiWindow);
    }

    removeElements();
}


void Device36::removeElements()
{
    // If some envelopes were extracted from this effect, disable them

restart:

    for(Element* el : MPattern->elems)
    {
        if(el->dev == this)
        {
            Delete_Element(el);

            goto restart;
        }
    }
}

void Device36::deletePresets()
{
    while(presets.size() > 0)
    {
        BrwEntry* pe = presets.front();

        presets.remove(pe);

        delete pe;
    }
}

void Device36::scanForPresets()
{
    WIN32_FIND_DATA founddata                  = {0};
    HANDLE          shandle                    = INVALID_HANDLE_VALUE;
    char            temppath[MAX_PATH_LENGTH] = {0};
    char            filename[MAX_PATH_LENGTH]  = {0};

    deletePresets();

    if (presetPath[0] == 0)
    {
        return;
    }

    /* File with user-saved preset is named in this way: <FX name>'_'<preset name>'.cpf' */
    /* cpf - Chaotic Preset File */

    sprintf(temppath,"%s%s",this->presetPath.data(), "*.cxml\0");
    shandle = FindFirstFile(temppath, &founddata);

    if (shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(filename, "%s%s",this->presetPath.data(), founddata.cFileName);

            String  sFilePath(filename);
            File myFile(sFilePath);
            XmlDocument myPreset(myFile);
            XmlElement* xmlMainNode = myPreset.getDocumentElement();

            if(xmlMainNode != NULL)
            {
                /* Sanity check */

                if(xmlMainNode->hasTagName(T("ChaoticPreset"))/* && xmlMainNode->hasAttribute(T("FormatVersion"))*/)
                {
                    BrwEntry *preset = new BrwEntry((Device36*)this);
                    preset->prindex = presets.size();

                    XmlElement* xmlHeader = xmlMainNode->getChildByName(T("Module"));

                    if (xmlHeader != NULL)
                    {
                        String Str1 = xmlHeader->getStringAttribute(T("PresetEntry"));
                        
                        preset->setObjName((const char*)Str1);
                        //Str1.copyToBuffer((const char*)preset->name.data(), min(Str1.length(), 255));
                        
                        preset->path = filename;

                        if(preset->getObjName() == currPresetName)
                        {
                            currPreset = preset;
                        }

                        presets.push_back(preset);
                    }
                }
            }
        }while(FindNextFile(shandle, &founddata));

        FindClose(shandle);
    }
}

void Device36::addParam(Parameter* param)
{
    param->module = this;
    param->setEnvDirect(false);

    params.push_back(param);
}

void Device36::removeParam(Parameter* param)
{
    params.remove(param);

    delete param;
}

void Device36::addParamWithControl(Parameter* param, std::string oname, Control* ctrl)
{
    addParam(param);

    if(ctrl == NULL)
    {
        if(oname == "")
        {
            if(param->type == Param_Bool)
            {
                oname = "tg.eff";
            }
            else
            {
                oname = "sl.eff";
            }
        }

        if(oname == "sl.eff")
        {
            Slider36* slider = new Slider36(false);
            ctrl = (Control*)slider;
        }
        else if(oname == "tg.eff")
        {
            Button36* button = new Button36(true);
            ctrl = (Control*)button;
        }

        //ctrl->height = 8; //rand()%3 + 20;
       // ctrl->height += rand()%3;
    }

    //ctrl->setObjName(oname);

    param->addControl(ctrl);

    addObject(ctrl);

    handleParamUpdate(param);
}

// BRIEF:
//    Dumps parameters data of the effect and all its children into XML. The method branches a
//    child-node from the parent node passed in, and fills it up with its parameters, name and so on.
//    This method provides recursive tree traversing approach.
//[in]
//    xmlParentNode - Reference to a parent node (where to branch from)
//    pPresetName   - name of the preset, which should be used in the child-node (optional)
//[out]
//    State of the effect (this) is added into parent xml node

void Device36::saveStateData(XmlElement & xmlParentNode, char* preset_name, bool global)
{
    XmlElement  *xmlStateNode = new XmlElement(T("Module"));

    xmlStateNode->setAttribute(T("Name"), objName.data());
    xmlStateNode->setAttribute(T("ID"), uniqueId);

    //xmlEffectHeader->setAttribute(T("Type"), this->devType);

    if (preset_name != NULL)
    {
        xmlStateNode->setAttribute(T("PresetEntry"), preset_name);
    }
    else if (currPreset != NULL)
    {
        xmlStateNode->setAttribute(T("PresetEntry"), currPreset->getObjName().data());
    }
    else
    {
        xmlStateNode->setAttribute(T("PresetEntry"), "default");
    }

    for(Parameter* param : params)
    {
        XmlElement * xmlParam = (global == true ? param->save() : param->save4Preset());
        xmlStateNode->addChildElement(xmlParam);
    }

    //Now let the child-class a chance to save anything it wants as a custom tag

    saveCustomStateData(*xmlStateNode);

    xmlParentNode.addChildElement(xmlStateNode);
}

// BRIEF:
//    Loads parameters of the effect with values from XML node passed in.
//
// [in]
//     xmlStateNode - Reference to a state node (where to get parameter values)
// [out]
//    none
void Device36::restoreStateData(XmlElement & xmlStateNode, bool global)
{
    Device36* pChildEffect = NULL;
    Parameter*   param;

    char         szName[MAX_NAME_LENGTH];
    XmlElement*  xmlChildNode = xmlStateNode.getFirstChildElement();

    //traverse XML tree and load parameters
    //When we meet "module" sub-node, pass it down to a proper child

    while (xmlChildNode != NULL)
    {
        // "Parameter" node we handle right here, it's ours to process

        if(xmlChildNode->hasTagName(T("Parameter")) == true)
        {
            //int idx = xmlChildNode->getIntAttribute(T("devIdx"));
            //param = this->getParamByIndex(idx);

            String name = xmlChildNode->getStringAttribute(T("name"));
            name.copyToBuffer(szName, min(name.length(), MAX_NAME_LENGTH));

            param = getParamByName(szName);

            if(param != NULL)
            {
                if(global)
                {
                    param->load(xmlChildNode);
                }
                else
                {
                    param->load4Preset(xmlChildNode);
                }
            }
        }
        else    //if sub-node has custom tag, we need to give a child-class a chance to process it
        {
            restoreCustomStateData(*xmlChildNode);
        }

        xmlChildNode = xmlChildNode->getNextElement();
    }
}

//    Saves state of the effect in XML document (preset file) and adds the preset into the list of
//    available resets, which the effect holds.
//
// IN:  preset_name  - Name which should be used for the preset being created
//
// OUT: A new preset-file is created in file system and corresponding preset item is added into effect's preset list
//
void Device36::savePresetAs(char * preset_name)
{
    BrwEntry          *preset                   = NULL;
    char              path[MAX_PATH_LENGTH]     = {0};
    char              cur_path[MAX_PATH_LENGTH] = {0};
    char              working_dir[MAX_PATH_LENGTH] = {};
    int               preset_index              = 0;
    char*             currpos = NULL;
    int               result = 0;
    int               length;
    int               drive = _getdrive();

    //Get current working directory
    _getdcwd(drive, working_dir, MAX_PATH_LENGTH - 1);

    //save the preset path into path variable
    sprintf_s(path, MAX_PATH_LENGTH - 1, "%s", presetPath.data());

    //let's save the length of the working directory, we gonna use it later
    length = strlen(working_dir);

    //concatenate working dir and preset path, now we have an absolute path to preset location 
    //we ignore leading '.' in the path variable, hence copying from an address of the second symbol
    strcat_s(working_dir, MAX_PATH_LENGTH - 1, &(path[1]));

    //sanity check, if we can't create a dir with this name, then it does already exist 
    //or requires additional parent dirs to be created first
    //result = _mkdir(working_dir);
    result = GetFileAttributes((LPCSTR)working_dir);

    if (result == INVALID_FILE_ATTRIBUTES)
    {
        //set initial position in the absolute path to the end of working directory
        //we gonna go through the rest of the path and check whether the path exists
        currpos = &(working_dir[length + 1]);

        //find next/next durectory to examine
        while ((currpos = strstr(currpos, "\\")) != NULL)
        {
            //length of current processing directory
            length = currpos - working_dir;
            strncpy_s(cur_path, MAX_PATH_LENGTH -1, working_dir, length);

            //try to create the dir

            _mkdir(cur_path);
            memset(cur_path, 0, length);

            ++currpos;
        }
    }

    sprintf_s(path, MAX_PATH_LENGTH - 1, "%s%s%s", presetPath.data(), preset_name, ".cxml\0");

    XmlElement  xmlPresetMain(T("MPreset"));

    //xmlPresetMain.setAttribute(T("FormatVersion"), MAIN_PROJECT_VERSION);

    saveStateData(xmlPresetMain, preset_name);

    String  sFilePath(path);
    File    myFile(sFilePath);

    xmlPresetMain.writeToFile(myFile, String::empty);

    preset = new BrwEntry((Device36*)this);

    // Num is 0-based so its value is equal to the devIdx of the next preset

    preset->prindex = presets.size();
    preset->path = path;
    preset->setObjName(preset_name);

    presets.push_back(preset);

    currPreset = preset;

    currPresetName = preset->getObjName();
}

long Device36::getPresetIndex(char* name)
{
    for(BrwEntry* pe : presets)
    {
        if(pe->getObjName() == name)
        {
            return pe->prindex;
        }
    }

    return -1;
}

bool Device36::setPresetByName(char * name)
{
    for(BrwEntry* pe : presets)
    {
        if(pe->getObjName() == name)
        {
            return setPresetByName(pe);
        }
    }

    return false;
}

bool Device36::setPresetByName(BrwEntry* preset)
{
    forceStop();

    String  sFilePath(preset->path.data());
    File myFile(sFilePath);
    XmlDocument myPreset(myFile);
    XmlElement* xmlMainNode = myPreset.getDocumentElement();

    if(xmlMainNode == NULL)
    {
        MWindow->showAlertBox("Can't open preset file");
    }
    else
    {
        if(xmlMainNode->hasTagName(T("ChaoticPreset"))/* && xmlMainNode->hasAttribute(T("FormatVersion"))*/)
        {
            /*
            if (MAIN_PROJECT_VERSION != xmlMainNode->getIntAttribute(T("FormatVersion")))
            {
                DawWindow->showAlertBox("Format version doesn't match. Preset could be loaded incorrectly.");
            }*/

            try
            {
                //First child is always a master plugin/effect
                XmlElement* xmlMasterHeader = xmlMainNode->getFirstChildElement();

                //Check whether the preset is for right plugin
                if(uniqueId != xmlMasterHeader->getDoubleAttribute(T("ID")))
                {
                    MWindow->showAlertBox("PresetEntry and plugin doesn't match");
                }
                else
                {
                    //Start recursive traversing of XML tree and loading of the preset
                    restoreStateData(*xmlMasterHeader);

                    currPresetName = preset->getObjName();

                    return true;
                }
            }
            catch(...)
            {
                MWindow->showAlertBox("Cannot parse preset file");
            }
        }
        else
        {
            MWindow->showAlertBox("Wrong preset format");
        }
    }

    return false;
}

Parameter* Device36::getParamByName(char *param_name)
{
    for(Parameter* param : params)
    {
        if(_stricmp(param->getName().data(), param_name) == 0)
        {
            return param;
        }
    }

    return NULL;
}

Parameter* Device36::getParamByIndex(int index)
{
    for(Parameter* param : params)
    {
        if(param->index == index)  return param;
    }

    return NULL;
}

void Device36::savePreset()
{
    AlertWindow w (T("Save preset"), T("Enter new preset devName:"), AlertWindow::QuestionIcon);

    w.setSize(132, 55);
    char name[MAX_NAME_LENGTH];

    if(currPresetName.size() == 0)
    {
        strcpy(name, "preset1");
    }
    else
    {
        strcpy(name, currPresetName.data());
    }

    w.addTextEditor (T("PresetName"), name, T(""));
    w.addButton (T("OK"), 1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton (T("Cancel"), 0, KeyPress (KeyPress::escapeKey, 0, 0));

    if(w.runModalLoop() != 0) // is they picked 'OK'
    {
        String nmstr = w.getTextEditorContents(T("PresetName"));
        char name[25];

        nmstr.copyToBuffer(name, 25);
        savePresetAs(name);

        if(MBrowser->browsingMode == Browse_Presets)  MBrowser->update();
    }
}

long Device36::getNumPresets()
{
    return presets.size();
}

void Device36::getPresetName(long index, char *name)
{
    if (((index >=0) && (index <= (long)presets.size())) && (name != NULL))
    {
        BrwEntry* preset;

        // If index in DLL's preset range then get it straight from plugin 

        if (index <= (long)presets.size())  preset = presets.front();

        for (BrwEntry* pe : presets)
        {
            if (pe->prindex == index)
            {
                strcpy(name, pe->getObjName().data());
                break;
            }
        }
    }
}

BrwEntry* Device36::getPreset(long index)
{
    for(BrwEntry* pe : presets)
        if(pe->prindex == index) return pe;

    return NULL;
}

BrwEntry* Device36::getPreset(char* name)
{
    for(BrwEntry* pe : presets)
        if (pe->getObjName() == name)  
            return pe;

    return NULL;
}

void Device36::enqueueParamEnvelope(Trigger* tg)
{
    tg->tgworking = true;

    if(envelopes != NULL)  envelopes->group_next = tg;

    tg->group_prev = envelopes;
    tg->group_next = NULL;

    envelopes = tg;

    Parameter* param = ((Envelope*)tg->el)->param;
    tg->prev_value = (param->value - param->offset)/param->range;

    // New envelopes unblock the param ability to be changed by envelope
    param->unblockEnvAffect();
}

void Device36::dequeueParamEnvelope(Trigger* tg)
{
    if(envelopes == tg)  envelopes = tg->group_prev;
    
    if(tg->group_prev != NULL)  tg->group_prev->group_next = tg->group_next;
    if(tg->group_next != NULL)  tg->group_next->group_prev = tg->group_prev;

    tg->group_prev = NULL;
    tg->group_next = NULL;
}

void Device36::showWindow(bool show)
{
    if(guiWindow == NULL)
    {
        guiWindow = createWindow();
        guiWindow->setBounds(window->getLastEvent().mouseX + 20, window->getLastEvent().mouseY + 20, guiWindow->getWidth(), guiWindow->getHeight());
    }

    if(guiWindow != NULL)
    {
        if (show)
        {
            guiWindow->setVisibility(true);
        }
        else
        {
            // aeffEditClose();
            guiWindow->setVisibility(false);
        }
    }
}


bool Device36::isWindowVisible()
{
    return (guiWindow != NULL && guiWindow->isVisible());
}

