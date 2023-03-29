
#pragma once



#include "36_globals.h"
#include "36_config.h"
#include "36_objects.h"






class EnableButton;
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
            Button36*           guiButton;
            std::string         instrAlias;
            InstrVU*            ivu;
            MixChannel*         mixChannel;
            EnableButton*       muteButt;
            ParamBox*           panBox;
            ParamBox*           volBox;
            Knob*               volKnob;
            Knob*               panKnob;

            void                addMixChannel();
            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
virtual Instrument*             clone();
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            std::string         getAlias()      {return instrAlias;};
            Device36*           getDevice()     { return device; };
            void                deleteDevice();
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
            void                remap();
            void                setDevice(Device36* dev);
            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);
            void                setIndex(int idx);
    virtual void                save(XmlElement* instrNode);

};


