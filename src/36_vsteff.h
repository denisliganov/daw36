
#pragma once

#include "36_globals.h"
#include "36_effects.h"
#include "36_vst.h"




class VstEffect : public Eff
{
public:

            VstEffect(char* filePath = NULL);
    virtual ~VstEffect();

            VstEffect*          clone(MixChannel* mc);
            SubWindow*          createWindow();
            void                drawSelf(Graphics& g);
            void                extractParams();
            std::list<Param*>   getParams() { return vst2->getParams(); }
            void                handleParamUpdate(Param* param = NULL);
            void                handleMouseDown(InputEvent& ev);
            void                load(XmlElement* xmlEff);
            bool                onUpdateDisplay();
            void                processData(float* in_buff, float* out_buff, int num_frames);
            void                processEvents(VstEvents *pEvents);
            void                reset();
            void                setBPM(float bpm);
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float smpRate);
            void                showEditor(bool show);
            void                save(XmlElement* xmlEff);
            bool                setPresetByName(char* objName);
            bool                setPresetByIndex(long devIdx);
            void                updatePresets();

private:

            Vst2Plugin*         vst2;
};



