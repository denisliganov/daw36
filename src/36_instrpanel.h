

#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"
#include "36_scroller.h"


#include <list>




class InstrPanel : public ParamObject
{
friend Audio36;

public:
            InstrPanel(Mixer* mixer);
            ~InstrPanel();

            void                addInstrFromNewBrowser(BrwListEntry* ble);
            Instrument*         addInstrument(Device36* dev, Instrument* objAfter = NULL);
            Instrument*         addVst(const char* path, Vst2Module* vst);
            Sample*             addSample(const char* path, bool temporaryForPreview = false);
            void                cloneInstrument(Instrument* i);
            void                colorizeInstruments();
            void                deleteInstrument(Instrument* i);
            void                drawSelf(Graphics& g);
            Instrument*         getInstrByIndex(int index);
            int                 getNumInstrs();
            Instrument*         getInstrByAlias(std::string alstr);
            void                generateAll(long num_frames, long mixbuffframe);
            Instrument*         getInstrFromLine(int trkLine);
            Instrument*         getCurrInstr();
        std::list<Instrument*>& getInstrs() { return instrs; }
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool                handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void                handleChildEvent(Gobj* obj, InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                hideFX();
            bool                isFxShowing()   { return fxShowing; }
            Vst2Module*         loadVst(const char* path, Vst2Module* otherVst);
            Sample*             loadSample(const char* path);
            void                placeBefore(Instrument* instr, Instrument* before);
            void                resetAll();
            void                remap();
            void                setSampleRate(float sampleRate);
            void                setBufferSize(unsigned bufferSize);
            void                setCurrInstr(Instrument* instr);
            void                showFX();
            void                setInstrFromNewBrowser(BrwListEntry* ble, Instrument* instr);
            void                updateWaves();
            void                updateInstrIndexes();

            Button36*           btShowFX;
            Button36*           btHideFX;
            Button36*           allChannelsView;;

protected:
            MixChannel*         currMixChannel;
            Gobj*               dropObj;
            InstrHighlight*     instrHighlight;
            bool                fxShowing;
            Mixer*              mixr;
            Slider36*           masterVolSlider;
            ParamBox*           masterVolBox;
            Knob*               masterVolKnob;
            Parameter*          masterVolume;

            std::list<Instrument*>::iterator    currInstr;
            std::list<Instrument*>              instrs;
};


