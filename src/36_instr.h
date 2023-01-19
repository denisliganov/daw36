
#pragma once


#include "sndfile.h"

#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"
#include "36_numbox.h"


//namespace M {


typedef enum InstrType
{
    Instr_Sample,
    Instr_Generator,
    Instr_Synth,
    Instr_SoundFont,
    Instr_VstPlugin
}InstrType;




class GuiButton;
class EnableButton;
class SoloButton;
class PreviewButton;





class Instrument : public Device36
{
friend MixChannel;
friend InstrPanel;
friend Grid;

protected:

            InstrType           type;
            int                 line;
            Parameter*          vol;
            Parameter*          pan;
            BoolParam*          muteparam;
            BoolParam*          soloparam;
            Slider36*           mvol;
            Slider36*           mpan;
            Knob*               volKnob;
            Knob*               panKnob;
            InstrVU*            ivu;
            ParamBox*           volBox;
            ParamBox*           panBox;

            float               dataBuff[MAX_BUFF_SIZE*2];     // Initial data
            float               inBuff[MAX_BUFF_SIZE*2];       // Data after separate DSP
            float               outBuff[MAX_BUFF_SIZE*2];      // Output after postprocessing

            std::list<Note*>        notes;
            std::list<Trigger*>     activeTriggers;

            // Helpers for buffer filling:

            int                 rampCount;
            float               volbase;    // Base environmental volume
            long                venvphase;
            long                endframe;  // last frame to fill
            float               pan0, pan1, pan2, pan3;
            bool                fill;
            bool                skip;
            float               rampCounterV;
            float               cfsV;
            float               rampCounterP;
            float               cfsP;
            float               locPan;
            std::string         instrAlias;

public:

            MixChannel*         mixChannel;
            Envelope*           envVol;
            Pattern*            selfPattern;
            Note*               selfNote;
            Button36*           previewButt;
            EnableButton*       muteButt;
            SoloButton*         soloButt;
            Button36*           butt;

            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;

            Instrument();
            ~Instrument();
virtual Instrument*             clone();
        Instrument*             makeClone(Instrument * instr);
            std::string         getAlias()                          {return instrAlias;};
            void                mapObjects();
            void                preview(int note = BaseNote);
            float               getLastNoteLength()                 { return lastNoteLength; }
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                activateContextMenuItem(std::string item);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
//            void                handleMouseEnter(InputEvent & ev)  { redraw(); }
//            void                handleMouseLeave(InputEvent & ev)  { redraw(); }
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            void                forceStop();
            void                setIndex(int idx);
    virtual void                createSelfPattern();
    virtual void                activateTrigger(Trigger* tg);
    virtual void                deactivateTrigger(Trigger* tg);
    virtual void                generateData(long num_frames = 0, long buffframe = 0);
    virtual void                staticInit(Trigger* tg, long num_frames);
    virtual void                preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe = 0);
    virtual long                processTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0) {return 0;};
    virtual long                workTrigger(Trigger* tg, long num_frames, long remaining, long buffframe, long mixbuffframe);
    virtual void                postProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
    virtual void                fillMixChannel(long num_frames, long buffframe, long mixbuffframe);
    virtual void                deClick(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
    virtual void                mute(Trigger* tg) {};
    virtual void                save(XmlElement* instrNode);
    virtual void                load(XmlElement* instrNode);
            void                flowTriggers(Trigger* tgfrom, Trigger* tgto);
     ContextMenu*               createContextMenu();
            void                addMixChannel();
            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);
            void                addNote(Note* note);
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                setLine(int lineNum);
            int                 getLine();
            void                updNotePositions();
            std::list <Element*>   getNotesFromRange(float tick_offset, float lastVisibleTick);

};

//}

