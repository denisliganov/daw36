
#pragma once

#include "36_element.h"
#include "36_grid.h"



class Note : public Element
{
friend  Grid;
friend  Lane;
friend  Instrument;
friend  Pattern;

protected:

            int             noteValue;
            Instrument*     instr;
            int             yBase;
            int             volHeight;

            int             barStart;
            int             barDraw;

            float           yPositionAdjust;

            void            propagateTriggers(Pattern* pt);
            void            unpropagateTriggers(Pattern* pt);
            void            move(float dtick, int dtrack);
            void            save(XmlElement* xmlNode);
            void            load(XmlElement* xmlNode);
            void            handleMouseUp(InputEvent& ev);
            void            handleMouseDown(InputEvent& ev);
            void            drawOnGrid(Graphics& g, Grid* grid);
            void            calcCoords(Grid* grid);
            void            calcNoteCoords(Grid* grid, float tick, int line, int* xc, int* yc, int* w, int* h);
    virtual void            calcNoteFreq();

public:

            Parameter*      vol;
            Parameter*      pan;
            float           freq;

            Note();
            Note(Instrument* i, int note_val);
            const Note& operator= (const Note& other);
    virtual ~Note();
    virtual Note*           clone(Instrument* newInstr = NULL);
            Element*        clone();
            void            preview(int note = -1, bool update_instr = false);
            void            releasePreview();
            bool            isNote() {return true;}
            void            setNoteValue(int note_value);
            int             getNoteValue() { return noteValue; }
      Instrument*           getInstr() { return instr; }
            void            setInstr(Instrument* i) { instr = i; }
            Parameter*      getParamByDisplayMode(GridDisplayMode mode);
            void            recalculate();
            void            setPos(float tick,int line);
};


class SampleNote : public Note
{
friend Sample;

protected:

            Sample*         sample;
            long            leftmostFrame;
            long            rightmostFrame;
            bool            reversed;
            float           dataStep;
            long            sampleFrameLength;


            void            drawOnGrid(Graphics& g, Grid* grid);

public:

            SampleNote(Sample* smp, int note_val);
            //const SampleNote& operator= (const SampleNote&);

            SampleNote*     clone(Instrument* new_instr);
            void            calcNoteFreq();
            bool            IsOutOfBounds(double* cursor);
            void            updateSampleBounds();
            bool            initCursor(double* cursor);
            void            save(XmlElement* xmlNode);
            void            load(XmlElement* xmlNode);
            void            setTickLength(float tick_length);
            void            recalculate();
            void            setTickDelta(float tick_delta);
};

