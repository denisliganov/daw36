//
//



#include "36_device.h"
#include "36_params.h"
#include "36_paramvol.h"
#include "36_parampan.h"
#include "36_slider.h"
#include "36_button.h"
#include "36_config.h"
#include "36_browser.h"
#include "36_instr.h"
#include "36_mixchannel.h"
#include "36_ctrlpanel.h"
#include "36_instrpanel.h"
#include "36_brwentry.h"
#include "36_edit.h"
#include "36_events_triggers.h"
#include "36_utils.h"


#include <direct.h>



Device36::Device36()
{
    internal = true;
    previewOnly = false;
    muteparam = soloparam = false;

    uniqueId = -1;

    devIdx = -1;
    rampCounterV = 0;
    cfsV = 0;
    rampCount = 512;

    envVol = NULL;

    lastNoteLength = 4;
    lastNoteVol = 1;
    lastNotePan = 0;
    lastNoteVal = BaseNote;

    addParam(vol = new ParamVol("VOL"));
    addParam(pan = new ParamPan("PAN"));

    currPreset = NULL;
    envelopes = NULL;
    guiWindow = NULL;

    currPresetName = "Untitled";
}

Device36::~Device36()
{
    while(params.size() > 0)
    {
        Param* param = params.front();

        params.remove(param);

        delete param;
    }

    //deletePresets();

    if(guiWindow)
    {
        guiWindow->setOpen(false);
        guiWindow->closeButtonPressed();
        window->deleteWindow(guiWindow);
    }

    removeElements();
}


void Device36::removeElements()
{
    // If some envelopes were extracted, disable them

restart:

    for(Element* el : MPattern->getElems())
    {
        if(el->getDevice() == this)
        {
            delete el;

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

    for(Param* param : params)
    {
        //XmlElement * xmlParam = (global == true ? param->save() : param->save4Preset());
        //xmlStateNode->addChildElement(xmlParam);
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
    Device36*   pChildEffect = NULL;
    Param*      param;

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
                    //param->load(xmlChildNode);
                }
                else
                {
                    //param->load4Preset(xmlChildNode);
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

bool Device36::setPresetByName(std::string pname)
{
    for(BrwEntry* pe : presets)
    {
        if(pe->getObjName() == pname)
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
        MWindow->showAlertBox("Can't open preset file " + preset->getObjName());
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

BrwEntry* Device36::getPresetByIndex(long index)
{
    for(BrwEntry* pe : presets)
    {
        if(pe->prindex == index) return pe;
    }

    return NULL;
}

BrwEntry* Device36::getPresetByName(std::string preset_name)
{
    for(BrwEntry* pe : presets)
    {
        if (pe->getObjName() == preset_name)
        {
            return pe;
        }
    }

    return NULL;
}

void Device36::showWindow(bool show)
{
    if(guiWindow == NULL)
    {
        guiWindow = createWindow();
    }

    if(guiWindow != NULL)
    {
        if (show)
        {
            guiWindow->setOpen(true);
        }
        else
        {
            // aeffEditClose();
            guiWindow->setOpen(false);
        }
    }

    redraw();
}

bool Device36::isWindowVisible()
{
    return (guiWindow != NULL && guiWindow->isOpen());
}

void Device36::handleWindowClosed()
{
    redraw();
}

void Device36::activateTrigger(Trigger* tg)
{
    Note* note = (Note*)tg->el;

    tg->outsync = false;
    tg->tgworking = true;
    tg->muted = false;
    tg->broken = false;
    tg->framePhase = 0;
    tg->auCount = 0;
    tg->volBase = note->vol->getOutVal();
    tg->panBase = note->pan->getOutVal();

    tg->setState(TS_Sustain);

    tg->noteVal = note->getNoteValue();
    tg->freq = note->freq;

    tg->wt_pos = 0;

    activeTriggers.remove(tg);
    activeTriggers.push_back(tg);
}

void Device36::addNote(Note * note)
{
    std::list<Note*>::iterator it = notes.begin();

    while(1)
    {
        if (it == notes.end())
        {
            notes.insert(it, note);
            break;
        }

        Note* n = *it;

        if(n->gettick() > note->gettick())
        {
            notes.insert(it, note);
            break;
        }
        else if (n->gettick() == note->gettick())
        {
            if(n->getNoteValue() < note->getNoteValue())
            {
                notes.insert(it, note);
                break;
            }
        }

        it++;
    }
}

void Device36::deactivateTrigger(Trigger* tg)
{
    activeTriggers.remove(tg);

    tg->tgworking = false;
}

// Perform fadeout or fadein antialiasing
//
// [IN]     stuff
//
void Device36::deClick(Trigger* tg, long num_frames, long buff_frame, long mix_buff_frame, long buff_remaining)
{
    long tc0;

    float mul;

    if(tg->aaOUT)
    {
        int nc = 0;

        tc0 = buff_frame*2;

        while(nc < num_frames)
        {
            if(tg->aaCount > 0)
            {
                mul = (float)tg->aaCount/DECLICK_COUNT;

                tempBuff[tc0] *= mul;
                tempBuff[tc0 + 1] *= mul;

                tc0++; tc0++;

                tg->aaCount--;
            }
            else
            {
                tempBuff[tc0] = 0;
                tempBuff[tc0 + 1] = 0;

                tc0++; tc0++;
            }

            nc++;
        }

        if(tg->aaCount == 0)
        {
            tg->aaOUT = false;
        }
    }
    else if(tg->aaIN)
    {
        int nc = 0;

        tc0 = buff_frame*2;

        while(nc < num_frames && tg->aaCount > 0)
        {
            mul = (float)(DECLICK_COUNT - tg->aaCount)/DECLICK_COUNT;

            tempBuff[tc0] *= mul;
            tempBuff[tc0 + 1] *= mul;

            tc0++;
            tc0++;
            nc++;

            tg->aaCount--;
        }

        if(tg->aaCount == 0)
        {
            tg->aaIN = false;
        }
    }

    // Check if finished

    bool aaU = false;
    int aaStart;

    if(tg->tgState == TS_SoftFinish)
    {
        // Finished case

        aaU = true;

        if(tg->auCount == 0)
        {
            aaStart = endFrame - DECLICK_COUNT;

            if(aaStart < 0)
            {
                aaStart = 0;
            }
        }
        else
        {
            tg->setState(TS_Finished);

            aaStart = 0;
        }
    }

    // Finish declick processing

    if(aaU == true)
    {
        jassert(aaStart >= 0);

        if(aaStart >= 0)
        {
            tc0 = buff_frame*2 + aaStart*2;

            while(aaStart < num_frames)
            {
                if(tg->auCount < DECLICK_COUNT)
                {
                    tg->auCount++;

                    mul = float(DECLICK_COUNT - tg->auCount)/DECLICK_COUNT;

                    tempBuff[tc0] *= mul;
                    tempBuff[tc0 + 1] *= mul;

                    tc0++;
                    tc0++;
                }
                else
                {
                    tempBuff[tc0] = 0;
                    tempBuff[tc0 + 1] = 0;

                    tc0++;
                    tc0++;
                }

                aaStart++;
            }
        }
    }
}

void Device36::forceStop()
{
restart:

    for(Trigger* tg :activeTriggers)
    {
        tg->stop();

        goto restart;
    }
}

//
// This function helps to declick when we're forcing note removal due to lack of free voice slots
//
void Device36::fadeBetweenTriggers(Trigger* tgfrom, Trigger* tgto)
{
    memset(outBuff, 0, rampCount*sizeof(float)*2);
    memset(tempBuff, 0, rampCount*sizeof(float)*2);
    memset(tgto->auxbuff, 0, rampCount*sizeof(float)*2);

    long actual = workTrigger(tgfrom, rampCount, rampCount, 0, 0);

    for(int ic = 0; ic < actual; ic++)
    {
        outBuff[ic*2] *= float(actual - ic)/actual;
        outBuff[ic*2 + 1] *= float(actual - ic)/actual;
    }

    memcpy(tgto->auxbuff, outBuff, actual*sizeof(float)*2);

    tgto->lcount = (float)rampCount;
}

void Device36::fillOutputBuffer(float* out_buff, long num_frames, long buff_frame, long mix_buff_frame)
{
    float volVal = vol->getOutVal();

    if(vol->lastValue == -1)
    {
        vol->setLastVal(vol->getOutVal());
    }
    else if(vol->lastValue != vol->getOutVal())
    {
        if(rampCounterV == 0)
        {
            cfsV = float(vol->getOutVal() - vol->lastValue)/DECLICK_COUNT;

            volVal = vol->lastValue;

            rampCounterV = DECLICK_COUNT;
        }
        else
        {
            volVal = vol->lastValue + (DECLICK_COUNT - rampCounterV)*cfsV;
        }
    }
    else if(rampCounterV > 0) // (paramSet->vol->lastval == paramSet->vol->outval)
    {
        rampCounterV = 0;
        cfsV = 0;
    }

    float lMax, rMax, outL, outR;
    lMax = rMax = 0;

    long tc0 = buff_frame*2;
    long tc = mix_buff_frame*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        if(rampCounterV > 0)
        {
            volVal += cfsV;

            rampCounterV--;

            if(rampCounterV == 0)
            {
                vol->setLastVal(vol->getOutVal());
            }
        }

        outL = outBuff[tc0++]*volVal;
        outR = outBuff[tc0++]*volVal;

        out_buff[tc++] = outL;
        out_buff[tc++] = outR;

        if(c_abs(outL) > lMax)
        {
            lMax = outL;
        }

        if(c_abs(outR) > rMax)
        {
            rMax = outR;
        }
    }

    // Update VU

    //ivu->setValues(lMax, rMax);
}

void Device36::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    
}

void Device36::process(float* in_buff, float* out_buff, int num_frames)
{
    if(envelopes == NULL && (bypass == false || muteCount < DECLICK_COUNT))
    {
        processDSP(in_buff, out_buff, num_frames);
    }
    else if(envelopes != NULL)
    {
        long framesToProcess;
        long buffFrame = 0;
        long framesRemaining = num_frames;

        while(framesRemaining > 0)
        {
            if(framesRemaining > BUFF_PROCESSING_CHUNK_SIZE)
            {
                framesToProcess = BUFF_PROCESSING_CHUNK_SIZE;
            }
            else
            {
                framesToProcess = framesRemaining;
            }

            /*
            tgenv = envelopes;
            while(tgenv != NULL)
            {
                env = (Envelope*)tgenv->el;
                if(buffframe >= env->last_buffframe)
                {
                    param = env->param;
                    param->SetValueFromEnvelope(env->buffoutval[buffframe], env);
                }
                tgenv = tgenv->group_prev;
            }
            */

            if(bypass == false || muteCount < DECLICK_COUNT)
            {
                processDSP(&in_buff[buffFrame*2], &out_buff[buffFrame*2], framesToProcess);
            }

            framesRemaining -= framesToProcess;
            buffFrame += framesToProcess;
        }
    }

    if(bypass == true && muteCount >= DECLICK_COUNT)
    {
        memcpy(out_buff, in_buff, num_frames*2*sizeof(float));
    }
}

void Device36::generateData(float* in_buff, float* out_buff, long num_frames, long mix_buff_frame)
{
    long        framesRemaining;
    long        framesToProcess;
    long        actual;
    long        buffFrame;
    long        mbframe;

    memset(outBuff, 0, num_frames*sizeof(float)*2);
    //memset(outBuff, 0, MAX_BUFF_SIZE*2);

    framesRemaining = num_frames;
    buffFrame = 0;
    mbframe = mix_buff_frame;


/*  if(envelopes == NULL && (bypass == false || muteCount < DECLICK_COUNT))
    {
        processDSP(in_buff, outBuff, num_frames);
    }
    else*/
    {
        while(framesRemaining > 0)
        {
            if(framesRemaining > BUFF_PROCESSING_CHUNK_SIZE)
            {
                framesToProcess = BUFF_PROCESSING_CHUNK_SIZE;
            }
            else
            {
                framesToProcess = framesRemaining;
            }

            /*
            // Process envelopes for this instrument
            Trigger* tgenv = envelopes;

            // Rewind to the very first, to provide correct envelopes overriding
            while(tgenv != NULL && tgenv->group_prev != NULL) 
            {
                tgenv = tgenv->group_prev;
            }

            // Now process them all

            while(tgenv != NULL)
            {
                env = (Envelope*)tgenv->el;
                if(env->newbuff && buffframe >= env->last_buffframe)
                {
                    param = env->param;
                    param->SetValueFromEnvelope(env->buff[mixbuffframe + buffframe], env);
                }

                tgenv = tgenv->group_next;
            }
            */


            for (auto itr = activeTriggers.begin(); itr != activeTriggers.end();)
            {
                // Clear buffer to avoid using obsolete data

                memset(tempBuff, 0, num_frames*sizeof(float)*2);

                Trigger* tg = *itr;

                itr++;

                actual = workTrigger(tg, framesToProcess, framesRemaining, buffFrame, mbframe);
            }

            if(bypass == false || muteCount < DECLICK_COUNT)
            {
                if (in_buff != NULL)
                {
                    processDSP(&in_buff[buffFrame*2], &outBuff[buffFrame*2], framesToProcess);
                }
            }

            framesRemaining -= framesToProcess;
            buffFrame += framesToProcess;
            mbframe += framesToProcess;
        }
    }

    fillOutputBuffer(out_buff, num_frames, 0, mix_buff_frame);
}

long Device36::workTrigger(Trigger * tg, long num_frames, long remaining, long buff_frame, long mix_buff_frame)
{
    pan0 = pan1 = pan2 = pan3 = 0;

    endFrame = 0;

    /// Init volume base. Volume base consist of volumes, that don't change during the whole filling session.
    /// it's then multiplied to dynamic tg->vol_val value in postProcessTrigger.

    volbase = 1; // tg->tgPatt->vol->outval; // Pattern static paramSet

    volbase *= DAW_INVERTED_VOL_RANGE;

    // Init pans
    pan1 = 0;
    pan2 = 0; // tg->tgPatt->pan->outval;     // Pattern's local panning
    pan3 = pan->getOutVal();     // Instrument's panning


    long actual_num_frames = processTrigger(tg, num_frames, buff_frame);

    if(actual_num_frames > 0)
    {
        postProcessTrigger(tg, actual_num_frames, buff_frame, mix_buff_frame, remaining - actual_num_frames);
    }

    if(tg->lcount > 0)
    {
        for(int ic = 0; ic < actual_num_frames; ic++)
        {
            outBuff[(buff_frame + ic)*2] += tg->auxbuff[int(rampCount - tg->lcount)*2];
            outBuff[(buff_frame + ic)*2 + 1] += tg->auxbuff[int(rampCount - tg->lcount)*2 + 1];

            tg->lcount--;

            if(tg->lcount == 0)
            {
                break;
            }
        }
    }

    // If trigger was finished during processing, deactivate it here

    if(tg->tgState == TS_Finished)
    {
        tg->stop();
    }

    return actual_num_frames;
}

void Device36::preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe)
{
    if(tg->broken)
    {
        // Per break or mute we could leave filling for two or more times, if there're not enough num_frames
        // to cover ANTIALIASING_FRAMES number

        if(tg->aaFilledCount >= DECLICK_COUNT)
        {
           *fill = false;
           *skip = true;

            tg->setState(TS_Finished);
        }
        else
        {
            tg->aaFilledCount += num_frames;
        }
    }

    // Check conditions for muting
    if(muteparam || !(SoloInstr == NULL || SoloInstr == this))
    {
        // If note just begun then there's nothing to declick. Set aaFilledCount to full for immediate muting

        if(tg->framePhase == 0)
        {
            tg->muted = true;
            tg->aaFilledCount = DECLICK_COUNT;
        }

        if(tg->muted == false)
        {
            // If note was already playig, then need to continue until DECLICK_COUNT number of frames are filled

            tg->muted = true;
            tg->aaOUT = true;
            tg->aaCount = DECLICK_COUNT;
            tg->aaFilledCount = num_frames;
        }
        else
        {
            if(tg->aaFilledCount >= DECLICK_COUNT)
            {
               *fill = false;
            }
            else
            {
                tg->aaFilledCount += num_frames;
            }
        }
    }
    else if(*fill == true)
    {
        if(tg->muted)
        {
            tg->muted = false;
            tg->aaIN = true;
            tg->aaCount = DECLICK_COUNT;
        }
    }
}

// Post-process data, generated per trigger. 
// Apply all high-level params, envelopes and fill corresponding mixcell.
//
// [IN]
//    tg            - trigger being processed.
//    num_frames    - number of frames to process.
//    curr_frame    - global timing frame number.
//    buffframe     - global buffering offset.
//
void Device36::postProcessTrigger(Trigger* tg, long num_frames, long buff_frame, long mix_buff_frame, long buff_remaining)
{
    float       vol, pan;
    float       volL, volR;
    long        tc, tc0;


    deClick(tg, num_frames, buff_frame, mix_buff_frame, buff_remaining);

    vol = tg->vol_val*volbase;

    pan0 = tg->pan_val;

    tc = mix_buff_frame*2;
    tc0 = buff_frame*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        // pan0 is unchanged
        // then to pattern/env

        // pan2 is unchanged
        // then to instrument/env

        pan = (((pan0*(1 - c_abs(pan1)) + pan1)*(1 - c_abs(pan2)) + pan2)*(1 - c_abs(pan3)) + pan3);

        //tg->lastpan = pan;
        ////PanLinearRule(pan, &volL, &volR);
        //pan = 0;
        //volL = volR = 1;
        //if(pan > 0)
        //    volL -= pan;
        //else if(pan < 0)
        //    volR += pan;
        //PanConstantRule(pan, &volL, &volR);

        int ai = int((PI_F*(pan + 1)/4)/wt_angletoindex);

        volL = wt_cosine[ai];
        volR = wt_sine[ai];

        outBuff[tc0] += tempBuff[tc0]*vol*volL;
        outBuff[tc0 + 1] += tempBuff[tc0 + 1]*vol*volR;

        tc0++;
        tc0++;
    }
}

void Device36::removeNote(Note * note)
{
    notes.remove(note);
}

void Device36::reinsertNote(Note * note)
{
    notes.remove(note);

    addNote(note);
}

void Device36::setLastParams(float last_length,float last_vol,float last_pan, int last_val)
{
    lastNoteLength = last_length;
    lastNoteVol = last_vol;
    lastNotePan = last_pan;
    lastNoteVal = last_val;
}





