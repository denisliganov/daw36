
#pragma once

#include "36_globals.h"
#include "36_effects.h"
#include "36_vst.h"




class VstEffect : public Eff
{
public:

            VstEffect(char* filePath = NULL);
    virtual ~VstEffect();

            Vst2Plugin*       vst2;
            Button36*         guibutt;

            VstEffect*  clone(MixChannel* mc);
            void        drawSelf(Graphics& g);
            void        processData(float* in_buff, float* out_buff, int num_frames);
            void        processEvents(VstEvents *pEvents);
            void        extractParams();
            void        updatePresets();
            void        handleParamUpdate(Parameter* param = NULL);
            bool        onUpdateDisplay();
            void        setBPM(float bpm);
            void        setBufferSize(unsigned int bufferSize);
            void        setSampleRate(float smpRate);
            void        showEditor(bool show);
            void        save(XmlElement* xmlEff);
            void        load(XmlElement* xmlEff);
            void        reset();
            void        handleMouseDown(InputEvent& ev);
            bool        setPresetByName(char* objName);
            bool        setPresetByIndex(long devIdx);

            SubWindow*   VstEffect::createWindow();
};



