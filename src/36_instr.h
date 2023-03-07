
#pragma once



#include "36_globals.h"
#include "36_config.h"
#include "36_objects.h"






class PreviewButt;
class EnableButton;
class SoloButton;
class GuiButt;




class Instrument : public Gobj
{
friend MixChannel;
friend InstrPanel;
friend Grid;

public:
            Instrument(Device36* dev);
            ~Instrument();

            Device36*           device;

            std::string         instrAlias;

            MixChannel*         mixChannel;

            EnableButton*       muteButt;
            Button36*           guiButton;
            SoloButton*         soloButt;
            InstrVU*            ivu;
            Knob*               volKnob;
            ParamBox*           volBox;
            ParamBox*           panBox;
            Button36*           previewButton;


            void                addMixChannel();
            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
virtual Instrument*             clone();
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            std::string         getAlias()      {return instrAlias;};
            Device36*           getDevice()     { return device; };
        std::list <Element*>    getNotesFromRange(float tick_offset, float lastVisibleTick);
            MixChannel*         getMixChannel() { return mixChannel; }
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            bool                isDummy();
    virtual void                load(XmlElement* instrNode);
            void                preview(int note = BaseNote);

            void                setDevice(Device36* dev);
            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);

            void                remap();
            void                setIndex(int idx);
    virtual void                save(XmlElement* instrNode);

};


