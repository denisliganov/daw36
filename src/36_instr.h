
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

public:

            Instrument();
            ~Instrument();

            std::list<Trigger*> activeTriggers;
            Button36*           butt;
            float               cfsV;
            float               cfsP;
            float               dataBuff[MAX_BUFF_SIZE*2];     // Initial data
            long                endframe;  // last frame to fill
            Envelope*           envVol;
            bool                fill;
            float               inBuff[MAX_BUFF_SIZE*2];       // Data after separate DSP
            InstrVU*            ivu;
            float               locPan;
            std::string         instrAlias;
            float               lastNoteLength;
            float               lastNoteVol;
            float               lastNotePan;
            int                 lastNoteVal;
            MixChannel*         mixChannel;
            EnableButton*       muteButt;
            BoolParam*          muteparam;
            Slider36*           mvol;
            Slider36*           mpan;
            std::list<Note*>    notes;
            float               outBuff[MAX_BUFF_SIZE*2];      // Output after postprocessing
            float               pan0, pan1, pan2, pan3;
            Button36*           previewButt;
            Parameter*          pan;
            ParamBox*           panBox;
            Knob*               panKnob;
            int                 rampCount;
            float               rampCounterV;
            float               rampCounterP;
            bool                skip;
            Pattern*            selfPattern;
            Note*               selfNote;
            SoloButton*         soloButt;
            BoolParam*          soloparam;
            InstrType           type;
            Parameter*          vol;
            Knob*               volKnob;
            ParamBox*           volBox;
            float               volbase;    // Base environmental volume
            long                venvphase;

            void                addMixChannel();
            void                activatemenuitem(std::string item);
            void                addNote(Note* note);
            virtual void        activateTrigger(Trigger* tg);
    virtual void                createSelfPattern();
            ContextMenu*        createmenu();
virtual Instrument*             clone();
            void                drawself(Graphics& g);
            void                drawover(Graphics & g);
        virtual void            deactivateTrigger(Trigger* tg);
        virtual void            deClick(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
            void                forceStop();
            void                flowTriggers(Trigger* tgfrom, Trigger* tgto);
    virtual void                fillMixChannel(long num_frames, long buffframe, long mixbuffframe);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            std::string         getAlias()                          {return instrAlias;};
            float               getLastNoteLength()                 { return lastNoteLength; }
            std::list <Element*>   getNotesFromRange(float tick_offset, float lastVisibleTick);
    virtual void                generateData(long num_frames = 0, long buffframe = 0);
    virtual void                load(XmlElement* instrNode);
    virtual void                mute(Trigger* tg) {};
        Instrument*             makeClone(Instrument * instr);
            void                preview(int note = BaseNote);
    virtual void                preProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe = 0);
    virtual long                processTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0) {return 0;};
    virtual void                postProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
            void                remap();
            void                removeNote(Note* note);
            void                reinsertNote(Note* note);
            void                setLastParams(float last_length,float last_vol,float last_pan, int last_val);
            void                setIndex(int idx);
    virtual void                staticInit(Trigger* tg, long num_frames);
    virtual void                save(XmlElement* instrNode);
            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);
            void                updNotePositions();
            virtual long        workTrigger(Trigger* tg, long num_frames, long remaining, long buffframe, long mixbuffframe);

};

//}

