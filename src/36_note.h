
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
            void            drwongrid(Graphics& g, Grid* grid);
            void            calcforgrid(Grid* grid);
    virtual void            calcfreq();

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
            void            setnote(int note_value);
            int             getNoteValue() { return noteValue; }
      Instrument*           getinstr() { return instr; }
            void            setInstr(Instrument* i) { instr = i; }
            Parameter*      getParamByDisplayMode(GridDisplayMode mode);
            void            recalc();
            void            setpos(float tick,int line);
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


            void            drwongrid(Graphics& g, Grid* grid);

public:

            SampleNote(Sample* smp, int note_val);
            //const SampleNote& operator= (const SampleNote&);

            SampleNote*     clone(Instrument* new_instr);
            void            calcfreq();
            bool            IsOutOfBounds(double* cursor);
            void            updateSampleBounds();
            bool            initCursor(double* cursor);
            void            save(XmlElement* xmlNode);
            void            load(XmlElement* xmlNode);
            void            setticklen(float tick_length);
            void            recalc();
            void            settickdelta(float tick_delta);
};

