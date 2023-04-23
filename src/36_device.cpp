//
//



#include "36.h"
#include "36_device.h"
#include "36_devwin.h"
#include "36_params.h"
#include "36_project.h"
#include "36_button.h"
#include "36_config.h"
#include "36_browser.h"
#include "36_mixchannel.h"
#include "36_instrpanel.h"
#include "36_edit.h"
#include "36_events_triggers.h"
#include "36_utils.h"


#include <direct.h>



Device36::Device36()
{
    muteCount = 0;
    uniqueId = -1;
    devIdx = -1;
    rampCounterV = 0;
    cfsV = 0;
    rampCount = 512;
    vol = NULL;
    pan = NULL;
    enabled = NULL;
    container = NULL;
    envVol = NULL;
    envelopes = NULL;
    guiWindow = NULL;
    selfPattern = NULL;
    selfNote = NULL;
    vu = NULL;
    lastNoteLength = 4;
    lastNoteVol = 1;
    lastNotePan = 0;
    lastNoteVal = BaseNote;

    memset(outBuff, 0, MAX_BUFF_SIZE * 2);
    memset(tempBuff, 0, MAX_BUFF_SIZE * 2);

    addParam(enabled = new Parameter("ENABLED", true));

    enabled->setAutoPlaced(false);
}

Device36::~Device36()
{
    while(params.size() > 0)
    {
        Parameter* param = params.front();

        params.remove(param);

        delete param;
    }

    removeElements();
}

void Device36::addBasicParamSet()
{
    addParam(vol = new Parameter("VOL", Param_Vol));
    addParam(pan = new Parameter("PAN", Param_Pan));

    vol->setAutoPlaced(false);
    pan->setAutoPlaced(false);
}

void Device36::removeElements()
{
restart:

    for (Note* note : notes)
    {
        notes.remove(note);
        delete note;
        goto restart;
    }

    /*
    for(Element* el : MPattern->getElems())
    {
        if(el->getDevice() == this)
        {
            delete el;
            goto restart;
        }
    }

    if (selfNote)
        delete selfNote;
    
    if (selfPattern)
        delete selfPattern;*/
}

// Create self-pattern + note, for previewing
//
void Device36::createSelfPattern()
{
    if(selfPattern == NULL)
    {
        selfPattern = new Pattern("self", 0.0f, 16.0f, 0, 0, true);
        selfPattern->setPattern(selfPattern);
        selfPattern->addInstance(selfPattern);

        MProject.patternList.push_front(selfPattern);
    }

    if(selfNote == NULL)
    {
        selfNote = CreateNote(0, 0, this, BaseNote, 4, 1, 0, selfPattern);

        selfNote->propagateTriggers(selfPattern);
    }

    selfPattern->recalc();
}

void Device36::deletePresets()
{
    presets.clear();
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

void Device36::saveState(XmlElement & xmlParentNode, char* preset_name, bool global)
{
    XmlElement  *xmlStateNode = new XmlElement(T("Module"));

    xmlStateNode->setAttribute(T("Name"), objName.data());
    xmlStateNode->setAttribute(T("ID"), uniqueId);

    //xmlEffectHeader->setAttribute(T("Type"), this->devType);

    if (preset_name != NULL)
    {
        xmlStateNode->setAttribute(T("PresetEntry"), preset_name);
    }
    else
    {
        xmlStateNode->setAttribute(T("PresetEntry"), "preset.name");
    }

    for(Parameter* param : params)
    {
        //XmlElement * xmlParam = (global == true ? param->save() : param->save4Preset());
        //xmlStateNode->addChildElement(xmlParam);
    }

    //Now let the child-class a chance to save anything it wants as a custom tag

    saveCustomState(*xmlStateNode);

    xmlParentNode.addChildElement(xmlStateNode);
}

// BRIEF:
//    Loads parameters of the effect with values from XML node passed in.
//
// [in]
//     xmlStateNode - Reference to a state node (where to get parameter values)
// [out]
//    none
void Device36::restoreState(XmlElement & xmlStateNode, bool global)
{
    Device36*   pChildEffect = NULL;
    Parameter*      param;

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
            restoreCustomState(*xmlChildNode);
        }

        xmlChildNode = xmlChildNode->getNextElement();
    }
}

//    Saves state of the module in XML document (preset file) and adds the preset into the list of
//    available resets, which the effect holds.
//
// IN:  preset_name  - Name which should be used for the preset being created
//
// OUT: A new preset-file is created in file system and corresponding preset item is added into effect's preset list
//
void Device36::savePreset()
{
    AlertWindow w (T("Save preset"), T("Enter new preset devName:"), AlertWindow::QuestionIcon);

    w.setSize(132, 55);
    char name[MAX_NAME_LENGTH];

    strcpy(name, "preset1");

    w.addTextEditor (T("PresetName"), name, T(""));
    w.addButton (T("OK"), 1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton (T("Cancel"), 0, KeyPress (KeyPress::escapeKey, 0, 0));

    if(w.runModalLoop() != 0) // is they picked 'OK'
    {
        String nmstr = w.getTextEditorContents(T("PresetName"));

        char preset_name[25];

        nmstr.copyToBuffer(preset_name, 25);

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
        sprintf_s(path, MAX_PATH_LENGTH - 1, "%s", PATH_PRESETS);
        
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
        
        sprintf_s(path, MAX_PATH_LENGTH - 1, "%s%s%s", PATH_PRESETS, preset_name, ".cxml\0");
        
        XmlElement  xmlPresetMain(T("MPreset"));
        
        //xmlPresetMain.setAttribute(T("FormatVersion"), MAIN_PROJECT_VERSION);
        
        saveState(xmlPresetMain, preset_name);
        
        String  sFilePath(path);
        File    myFile(sFilePath);
        
        xmlPresetMain.writeToFile(myFile, String::empty);
        
        presets.push_back(preset_name);
    }
}

bool Device36::setPreset(std::string pname)
{
    forceStop();

    String  sFilePath("stub: construct correct path here");

    File myFile(sFilePath);

    XmlDocument myPreset(myFile);

    XmlElement* xmlMainNode = myPreset.getDocumentElement();

    if(xmlMainNode == NULL)
    {
        MWindow->showAlertBox("Can't open preset file " + pname);
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

                    restoreState(*xmlMasterHeader);

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

long Device36::getNumPresets()
{
    return presets.size();
}

void Device36::getPresetName(long index, char *name)
{
    if (index < presets.size() && name != NULL)
    {
        strcpy(name, presets[index].data());
    }
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

    if (parent)
    {
        parent->redraw();
    }
}

void Device36::handleMouseWheel(InputEvent & ev)
{
    if (parent)
    {
        parent->handleMouseWheel(ev);
    }
}

void Device36::handleMouseDrag(InputEvent& ev)
{
    if (parent)
    {
        parent->handleMouseDrag(ev);
    }
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
void Device36::deClick(Trigger* tg, long num_frames, long buff_frame)
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

// Stops all active triggers

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

    long actual = processTrigger(tgfrom, rampCount, rampCount, 0);

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
    float volVal = vol != NULL ? vol->getOutVal() : 1;
    float lastVal = vol != NULL ? vol->lastValue : 1;

    if (vol != NULL)
    {
        if ( vol->lastValue == -1)
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

            if(vol != NULL && rampCounterV == 0)
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

    if (vu)
    {
        vu->setValues(lMax, rMax);
    }
}

void Device36::processDSP(float* in_buff, float* out_buff, int num_frames)
{
    // Stub for DSP effects
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

                actual = processTrigger(tg, framesToProcess, framesRemaining, buffFrame);
            }

            if (in_buff != NULL)
            {
                if(enabled->getBoolValue())
                {
                    processDSP(&in_buff[buffFrame*2], &outBuff[buffFrame*2], framesToProcess);

                    // Copy output back to input for the next effect to process

                    if(muteCount > 0)
                    {
                        // Exiting muted state, gradually add processed data to input data

                        long tc = buffFrame;
                        float aa;

                        while(tc < framesToProcess)
                        {
                            // aa from 0 up to 1

                            aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                            outBuff[tc*2] = in_buff[tc*2]*(1.f - aa) + outBuff[tc*2]*aa;
                            outBuff[tc*2 + 1] = in_buff[tc*2 + 1]*(1.f - aa) + outBuff[tc*2 + 1]*aa;

                            tc++;

                            if(muteCount > 0)
                            {
                                muteCount--;
                            }
                        }
                    }
                }
                else
                {
                    if(muteCount < DECLICK_COUNT)
                    {
                        // Entering muted state, gradually remove processed data from input data

                        processDSP(&in_buff[buffFrame*2], &outBuff[buffFrame*2], framesToProcess);

                        long tc = buffFrame;
                        float aa;

                        while(tc < framesToProcess && muteCount < DECLICK_COUNT)
                        {
                            // aa from 1 down to 0

                            aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;

                            outBuff[tc*2] = in_buff[tc*2]*(1.f - aa) + outBuff[tc*2]*aa;
                            outBuff[tc*2 + 1] = in_buff[tc*2 + 1]*(1.f - aa) + outBuff[tc*2 + 1]*aa;

                            tc++;

                            muteCount++;
                        }
                    }
                    else
                    {
                        memcpy(&outBuff[buffFrame*2], &in_buff[buffFrame*2], sizeof(float) * framesToProcess * 2);
                    }
                }
            }

            framesRemaining -= framesToProcess;
            buffFrame += framesToProcess;
            mbframe += framesToProcess;
        }
    }

    fillOutputBuffer(out_buff, num_frames, 0, mix_buff_frame);
}

void Device36::preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe)
{
/*
    if(tg->broken)
    {
        // On break or mute, we could leave filling for two or more times, if there're not enough num_frames
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
*/

    // Check off conditions

    if(!isEnabled() /*|| !(SoloInstr == NULL || SoloInstr == this)*/)
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

long Device36::processTrigger(Trigger * tg, long num_frames, long remaining, long buff_frame)
{
    endFrame = 0;

    long actual_num_frames = handleTrigger(tg, num_frames, buff_frame);

    if(actual_num_frames > 0)
    {
        deClick(tg, actual_num_frames, buff_frame);

        volBase = 1; // tg->tgPatt->vol->outval; // Pattern static paramSet
        volBase *= DAW_INVERTED_VOL_RANGE;
        volBase *= tg->volBase;

        pan0 = tg->panBase;
        pan1 = 0;
        pan2 = 0;                    // tg->tgPatt->pan->outval;     // Pattern's local panning
        pan3 = pan->getOutVal();     // Instrument's panning

        // postProcessTrigger(tg, actual_num_frames, buff_frame, mix_buff_frame, remaining - actual_num_frames);

        long tc0 = buff_frame*2;

        float panSum = (((pan0*(1 - c_abs(pan1)) + pan1)*(1 - c_abs(pan2)) + pan2)*(1 - c_abs(pan3)) + pan3);

        int ai = int((PI_F*(panSum + 1)/4)/wt_angletoindex);

        float volL = wt_cosine[ai];
        float volR = wt_sine[ai];

        for(long cc = 0; cc < actual_num_frames; cc++)
        {
            outBuff[tc0] += tempBuff[tc0]*volBase*volL;
            outBuff[tc0 + 1] += tempBuff[tc0 + 1]*volBase*volR;

            tc0++;
            tc0++;
        }
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

//
// Post-process data, generated per trigger. 
// Apply all high-level params, envelopes.
//
//
void Device36::postProcessTrigger(Trigger* tg, long num_frames, long buff_frame)
{
    deClick(tg, num_frames, buff_frame);

    long tc0 = buff_frame*2;

    for(long cc = 0; cc < num_frames; cc++)
    {
        // pan0 is unchanged
        // then to pattern/env

        // pan2 is unchanged
        // then to instrument/env

        float pan = (((pan0*(1 - c_abs(pan1)) + pan1)*(1 - c_abs(pan2)) + pan2)*(1 - c_abs(pan3)) + pan3);

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

        float volL = wt_cosine[ai];
        float volR = wt_sine[ai];

        outBuff[tc0] += tempBuff[tc0]*volBase*volL;
        outBuff[tc0 + 1] += tempBuff[tc0 + 1]*volBase*volR;

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

SubWindow* Device36::createWindow()
{
    SubWindow* win =  MObject->addWindow((WinObject*)new DevParamObject(this));

    //int xb = window->getLastEvent().mouseX + 20;
    //int yb = window->getLastEvent().mouseY - guiWindow->getHeight()/2;

    win->setBounds(MObject->getLastEvent().mouseX + 40, win->getY(), win->getWidth(), win->getHeight());

    return win;
}


