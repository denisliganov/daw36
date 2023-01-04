

#pragma once


#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"


#include <list>


// namespace M {


class InstrPanel : public Device36
{
friend Audio36;

public:

            Mixer*          mixr;

            Slider36*       masterVolSlider;
            ParamBox*       masterVolBox;
            Knob*           masterVolKnob;
            Parameter*      masterVolume;

            Button36*       sizeUp;
            Button36*       sizeDown;
            Button36*       colorUp;
            Button36*       colorDown;

            Button36*       masterFX;
            Button36*       send1FX;
            Button36*       send2FX;
            Button36*       send3FX;
            Button36*       btShowFX;
            Button36*       btHideFX;

            int             instrOffset;
            Scroller*       scroller;
            Gobj*           dropObj;
            float           fullSpan;
            float           visibleSpan;
            MixChannel*     currMixChannel;
            bool            fxShowing;

            std::list<Instrument*>              instrs;
            std::list<Instrument*>::iterator    currInstr;

            InstrPanel(Mixer* mixer);
            Instrument*     loadInstrFromBrowser(BrwEntry* fdi);
            void            deleteInstrument(Instrument* i);
            void            cloneInstrument(Instrument* i);
            Instrument*     getInstrByIndex(int index);
            void            editAutopattern(Instrument* instr);
            void            mapObjects();
            void            drawSelf(Graphics& g);
            void            addInstrument(Instrument* i, Instrument* objAfter = NULL);
            VstInstr*       loadVst(const char* path, VstInstr* otherVst);
            VstInstr*       addVst(const char* path, VstInstr* vst);
            Sample*         loadSample(const char* path);
            Sample*         addSample(const char* path, bool temporaryForPreview = false);
            int             getFullHeight();
            int             getInstrNum();
            void            resetAll();
            void            generateAll(long num_frames, long mixbuffframe);
            void            setSampleRate(float sampleRate);
            void            setBufferSize(unsigned bufferSize);
            Instrument*     getCurrInstr();
            void            setCurrInstr(Instrument* instr);
            void            placeBefore(Instrument* instr, Instrument* before);
            Instrument*     getInstrByAlias(std::string alstr);
            void            updateWaves();
            void            adjustOffset();
            int             getOffset() { return instrOffset; }
            void            setOffset(int offs);
            Instrument*     getInstrFromLine(int trkLine);
            void            setInstrHeight(int instr_height);
            void            showFX();
            void            hideFX();
            void            updateInstrNotePositions();

protected:

            bool            handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool            handleObjDrop(Gobj * obj,int mx,int my, unsigned int flags);
            void            colorizeInstruments();
            void            handleChildEvent(Gobj* obj, InputEvent& ev);
            void            handleMouseWheel(InputEvent& ev);
            void            updateInstrIndexes();
};

//};

