
#pragma once



#include "36_globals.h"
#include "36_config.h"
#include "36_objects.h"






class EnableButton;
class GuiButt;




class Instr : public Gobj
{
friend MixChannel;
friend InstrPanel;
friend Grid;

public:
            Instr(Device36* dev);
            ~Instr();

            void                addMixChannel();
            void                activateMenuItem(std::string item);
            ContextMenu*        createContextMenu();
    virtual Instr*              clone();
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            std::string         getAlias()      {return instrAlias;};
            Device36*           getDevice()     { return device; };
            int                 getIndex()      {   return index; }
            void                deleteDevice();
        std::list <Element*>    getNotesFromRange(float tick_offset, float lastVisibleTick);
            MixChannel*         getMixChannel() { return mixChannel; }
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
    virtual void                load(XmlElement* instrNode);
            bool                isMaster()  { return master; }
            void                preview(int note = BaseNote);
            void                remap();
            void                setDevice(Device36* dev);
            void                setBufferSize(unsigned bufferSize);
            void                setSampleRate(float sampleRate);
            void                setIndex(int idx);
    virtual void                save(XmlElement* instrNode);
            void                setAlias(std::string alias);
            void                setMixChannel(MixChannel* mchan);

private:
            Device36*           device;
            Button36*           guiButton;
            std::string         instrAlias;
            InstrVU*            ivu;
            MixChannel*         mixChannel;
            EnableButton*       muteButt;
            bool                master;
            Knob*               volKnob;
            Knob*               panKnob;
            int                 index;
};


