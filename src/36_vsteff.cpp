

#include <direct.h>

#include "36_vsteff.h"
#include "36_draw.h"
#include "36_mixer.h"
#include "36_button.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_browser.h"
#include "36_project.h"
#include "36_window.h"
#include "36_paramswin.h"
#include "36_vstwin.h"



VstEffect::VstEffect(char* path)
{
    internal = false;
    isLoading = true;

    objId = "eff.vst";

    presetPath = ".\\Data\\Presets\\";

    guibutt = NULL;

    // While loading some VST plugins current working directory somehow gets changed
    // Thus we need to remember it and restore in the end of initialization, hehehe

    char workdir[MAX_PATH_LENGTH] = {0};

    int drive = _getdrive();

    _getdcwd(drive, workdir, MAX_PATH_LENGTH - 1 );

    // Pass NULL if you want to see OpenFile dialog

    vst2 = VstHost->loadModuleFromFile(path);

    if (vst2 != NULL)
    {
        uniqueId = vst2->aeff->uniqueID;

        if (path)
            filePath = path;
        else
            filePath = vst2->vstpath;

        objName = vst2->objName;

        guibutt = new Button36(true);

        //guibutt->SetTitle("Edit");
        //guibutt->SetHint("Show editor window");

        addObject((Control*)(guibutt));
        extractParams();
        presetPath = "Effects\\VST\\";

        updatePresets();
    }

    //  pPlug->pEffect->EffSuspend();
    // Restore previously saved working directory

    _chdir(workdir);

    isLoading = false;
}

VstEffect::~VstEffect()
{
    if (vst2 != NULL)
    {
        VstHost->removeModule(vst2);
    }
}

void VstEffect::drawSelf(Graphics & g)
{
    setc(g, 0xff508080);
    fillx(g, 0, 0, width, height);

    setc(g, (uint32)0x30FFFFFF);
    rectx(g, 0, 0, width, height);

    if (guiWindow && guiWindow->isOpen())
    {
        uint32 color = 0xffFF9930;
        uint32 clrDecr = 0x80000000;

        for (int c = 0; c < 4; c++)
        {
            setc(g, (uint32)color);

            rectx(g, 0 + c, 0 + c, width - c*2, height - c*2);

            color -= clrDecr;
            clrDecr /=2;
        }
    }

    setc(g, .0f);
    txtfit(g, FontBold, vst2->objName, 3, 11, 91);

    setc(g, 0xffCFEFFF);
    //gSetColor((0xff46FFB4));

    txtfit(g, FontBold, vst2->objName, 2, 10, 91);
}

VstEffect* VstEffect::clone(MixChannel* mc)
{
    VstEffect* clone = new VstEffect((char*)filePath.data());
    MemoryBlock m;

    vst2->getStateInformation(m);
    clone->vst2->setStateInformation(m.getData(), m.getSize());

/*
    // Set the current preset equal to its parent one

    if (this->CurrentPreset != NULL)
    {
        clone->SetPreset(this->CurrentPreset->name, this->CurrentPreset->native);
    }

    // Now go through the list of parameters and tune them up to the parent effect

    Parameter* pParam = this->firstParam;
    Parameter* pCloneParam = clone->firstParam;

    while((pParam != NULL) && (pCloneParam != NULL))
    {
        clone->ParamUpdate(pParam);
        pCloneParam->SetNormalValue(pParam->val);
        pParam = pParam->next;
        pCloneParam = pCloneParam->next;
    }
*/

    return clone;
}

void VstEffect::updatePresets()
{
    vst2->updatePresets();

    // sync lists 
    presets = vst2->presets;
}

void VstEffect::extractParams()
{
    vst2->extractParams();
}

void VstEffect::processData(float * in_buff,float * out_buff,int num_frames)
{
    // pVSTCollector->AcquireSema();
    // pPlug->pEffect->EffResume();

    vst2->processData(in_buff, out_buff, num_frames);

    // pPlug->pEffect->EffSuspend();
    // pVSTCollector->ReleaseSema();
}

void VstEffect::processEvents(VstEvents *pEvents)
{
    vst2->processEvents(pEvents);
}

void VstEffect::handleParamUpdate(Parameter* param)
{
    vst2->handleParamUpdate(param);
}

bool VstEffect::setPresetByName(char* name)
{
    return vst2->setPresetByName(name);
}

bool VstEffect::setPresetByIndex(long index)
{
    return vst2->setPresetByIndex(index);
}

void VstEffect::setBPM(float bpm)
{
    //vstplug->setBPM(bpm);
}

void VstEffect::setBufferSize(unsigned int bufferSize)
{
    vst2->setBufferSize(bufferSize);
}

void VstEffect::setSampleRate(float sampleRate)
{
    vst2->setSampleRate(sampleRate);
}

void VstEffect::reset()
{
    vst2->reset();
}

bool VstEffect::onUpdateDisplay()
{
    if (isLoading == true || !MProject.isLoading())
    {
        //This effect or current project is loading

        return false;
    }

    updatePresets();

    // Update pointer to current active preset

    BrwEntry* updPreset = getPreset(vst2->getProgram());

    if (currPreset != updPreset)  currPreset = updPreset;

    window->redraw();

    return true;
}

void VstEffect::save(XmlElement * xmlEff)
{
    Eff::save(xmlEff);

    vst2->save(xmlEff);
}

void VstEffect::load(XmlElement * xmlEff)
{
    setParamLock(true);

    Eff::load(xmlEff);

    setParamLock(false);

    vst2->load(xmlEff);
}

void VstEffect::handleMouseDown(InputEvent& ev)
{
    if(ev.keyFlags == 0 && ev.doubleClick)
    {
        showWindow(true);
    }
}

SubWindow* VstEffect::createWindow()
{
    if(vst2->hasGui())
    {
        return window->addWindow(new VstComponent(vst2, this));
    }
    else
    {
        return window->addWindow(new ParamObject());
    }
}



