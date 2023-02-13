
#pragma once

#include "36_element.h"
#include "36_grid.h"



class Note : public Element
{
friend  Grid;
friend  Lane;
friend  Instrument;
friend  Pattern;

public:

            Note();
            Note(Instrument* i, int note_val);
    virtual ~Note();
    virtual Note*               clone(Instrument* newInstr = NULL);
            Element*            clone();
            void                calcForGrid(Grid* grid);
            void                drawOnGrid(Graphics& g, Grid* grid);
            int                 getNoteValue() { return noteValue; }
            VolParam*           getVol() {return vol;}
            Parameter*          getPan() {return pan;}
       Instrument*              getInstr() { return instr; }
            float               getFreq() { return freq; }
       Parameter*               getParamByDisplayMode(GridDisplayMode mode);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            bool                isNote() {return true;}
            void                preview(int note = -1, bool update_instr = false);
            void                releasePreview();
            void                recalc();
            void                setInstr(Instrument* i) { instr = i; }
            void                setNote(int note_value);
            void                setPos(float tick,int line);

protected:

            int                 barStart;
            int                 barDraw;
            float               freq;
            Instrument*         instr;
            int                 noteValue;
            Parameter*          pan;
            VolParam*           vol;
            int                 yBase;

            void                propagateTriggers(Pattern* pt);
            void                unpropagateTriggers(Pattern* pt);
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
            SampleNote*         clone(Instrument* new_instr);
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

