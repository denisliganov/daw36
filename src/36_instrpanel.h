

#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"
#include "36_scroller.h"


#include <list>
#include <vector>




class DropHighlight;

class InstrPanel : public ParamObject
{
friend Audio36;

public:
            InstrPanel();
            ~InstrPanel();

            void                addInstrFromNewBrowser(BrwListEntry* ble);
            Instr*              addInstrument(Device36* dev, bool master = false);
            Vst2Plugin*         addVst(const char* path, Vst2Plugin* vst);
            Sample*             addSample(const char* path);
            void                cloneInstrument(Instr* i);
            void                colorizeInstruments();
            void                deleteInstrument(Instr* i);
            void                drawSelf(Graphics& g);
            Instr*              getInstrByIndex(int index);
            int                 getNumInstrs();
            Instr*              getInstrByAlias(std::string alstr);
            void                generateAll(long num_frames, long mixbuffframe);
            Instr*              getInstrFromLine(int trkLine);
            Instr*              getCurrInstr();
        std::vector<Instr*>&    getInstrs() { return instrs; }
            void                handleObjDrag(bool reset, Gobj * obj,int mx,int my);
            void                handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void                handleChildEvent(Gobj* obj, InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseLeave(InputEvent& ev);
            void                hideFX();
            bool                isFxShowing()   { return fxShowing; }
            Vst2Plugin*         loadVst(const char* path, Vst2Plugin* otherVst);
            Sample*             loadSample(const char* path);
            void                resetAll();
            void                remap();
            void                setSampleRate(float sampleRate);
            void                setBufferSize(unsigned bufferSize);
            void                setCurrInstr(Instr* instr);
            void                setCurrInstr(int instr_index);
            void                showFX();
            void                setInstrFromNewBrowser(BrwListEntry* ble, Instr* instr);
            void                updateWaves();
            void                ReIndexInstruments();

// Mix
            MixChannel*         addMixChannel(Instr* instr);
            void                cleanBuffers(int num_frames);
            MixChannel*         getMasterChannel()          { return masterChannel; }
            void                init();
            void                mixAll(int num_frames);
            void                removeMixChannel(Instr * instr);

private:

            Button36*           btShowFX;
            Button36*           btHideFX;
            Button36*           allChannelsView;;

            Gobj*               dropObj;
            InstrHighlight*     instrHighlight;
            DropHighlight*      dropHighlight;
            bool                fxShowing;
            Slider36*           masterVolSlider;
            Knob*               masterVolKnob;
            Parameter*          masterVolume;

    std::vector<Instr*>         instrs;
            Instr*              curr;

            MixChannel*         masterChannel;

    std::list<MixChannel*>      mixQueue;
};


