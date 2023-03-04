
#pragma once

#include "36_element.h"
#include "36_grid.h"



class Note : public Element
{
friend  Grid;
friend  Lane;
friend  Device36;
friend  Pattern;

public:

            Note();
            Note(Device36* i, int note_val);
    virtual ~Note();
    virtual Note*               clone(Device36* newInstr = NULL);
            Element*            clone();
            void                calcForGrid(Grid* grid);
            void                drawOnGrid(Graphics& g, Grid* grid);
            int                 getNoteValue() { return noteValue; }
            ParamVol*           getVol() {return vol;}
            Parameter*          getPan() {return pan;}
         Device36*              getInstr() { return instr; }
            float               getFreq() { return freq; }
       Parameter*               getParamByDisplayMode(GridDisplayMode mode);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            bool                isNote() {return true;}
            void                preview(int note = -1, bool update_instr = false);
            void                releasePreview();
            void                recalc();
            void                setInstr(Device36* i) { instr = i; }
            void                setNote(int note_value);
            void                setPos(float tick,int line);
            void                propagateTriggers(Pattern* pt);
            void                unpropagateTriggers(Pattern* pt);
protected:

            int                 barStart;
            int                 barDraw;
            float               freq;
            Device36*           instr;
            int                 noteValue;
            Parameter*          pan;
            ParamVol*           vol;
            int                 yBase;

            void                move(float dtick, int dtrack);
            void                save(XmlElement* xmlNode);
            void                load(XmlElement* xmlNode);
    virtual void                calcFreq();
};


class SampleNote : public Note
{
friend Sample;

public:

            SampleNote(Sample* smp, int note_val);
            SampleNote*         clone(Device36* new_instr);
            void                calcFreq();
            void                drawOnGrid(Graphics& g, Grid* grid);
            bool                isOutOfBounds(double* cursor);
            bool                initCursor(double* cursor);
            void                load(XmlElement* xmlNode);
            void                recalc();
            void                setTickLength(float tick_length);
            void                setTickDelta(float tick_delta);
            void                save(XmlElement* xmlNode);
            void                updateSampleBounds();

protected:

            float               dataStep;
            long                leftmostFrame;
            long                rightmostFrame;
            bool                reversed;
            long                sampleFrameLength;
            Sample*             sample;
};

