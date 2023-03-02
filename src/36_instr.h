
#pragma once


#include "sndfile.h"

#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"



//namespace M {


typedef enum InstrType
{
    Instr_Sample,
    Instr_Generator,
    Instr_Synth,
    Instr_SoundFont,
    Instr_VstPlugin
}InstrType;




class PreviewButt;
class EnableButton;
class SoloButton;
class GuiButt;





class Instrument : public Device36
{
friend MixChannel;
friend InstrPanel;
friend Grid;

public:
            Instrument();
            ~Instrument();

            EnableButton*       muteButt;
            Button36*           guiButton;
            InstrType           type;
            SoloButton*         soloButt;
            std::string         instrAlias;
            Pattern*            selfPattern;
            Note*               selfNote;
            InstrVU*            ivu;
            Knob*               volKnob;
            ParamBox*           volBox;
            ParamBox*           panBox;


            void                addMixChannel();
            void                activateMenuItem(std::string item);
    virtual void                createSelfPattern();
            ContextMenu*        createContextMenu();
virtual Instrument*             clone();
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            std::string         getAlias()                          {return instrAlias;};
            float               getLastNoteLength()                 { return lastNoteLength; }
        std::list <Element*>    getNotesFromRange(float tick_offset, float lastVisibleTick);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
    virtual void                load(XmlElement* instrNode);
            void                preview(int note = BaseNote);

            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);

            void                remap();
            void                setIndex(int idx);
    virtual void                save(XmlElement* instrNode);

};

//}

