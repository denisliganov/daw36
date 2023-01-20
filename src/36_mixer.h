

#pragma once


#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"


#include <list>
#include <string>


typedef struct SendSingle
{
    Parameter*  amount;
    Knob*       r_amount;
}SendSingle;

class MixNode : public Gobj
{
protected:

            float               buffer[MAX_BUFF_SIZE*2];

            std::list<MixNode*> inNodes;
            MixNode*            outNode;

public:
            MixNode();
};

class MixChannel : public Device36
{
friend  Mixer;
friend  ChanVU;
friend  Eff;

public:

            std::string     chanTitle;
            std::list<Eff*> effs;

            Instrument*     instr;
            ChanVU*         vu;
            Slider36*       volslider;
            Slider36*       panslider;
            Knob*           volKnob;
            Knob*           panKnob;
            Button36*       solotoggle;
            Button36*       mutetoggle;
            Parameter*      volParam;
            Parameter*      panParam;
            BoolParam*      muteparam;
            BoolParam*      soloparam;
            int             mutecount;
            bool            master;
            int             voffs;
            int             contentheight; 
            Gobj*           dropObj;

            SendSingle      sends[NUM_SENDS];

            float           inbuff[MAX_BUFF_SIZE*2];
            float           outbuff[MAX_BUFF_SIZE*2];

            MixChannel*     mchanout;


            MixChannel();
            MixChannel(Instrument* i);
            ~MixChannel();
            void            init(Instrument* i);
            void            addEffect(Eff* eff);
            void            removeEffect(Eff* eff);
            void            process(int num_frames, float* outbuff);
            void            doSend(float * sendbuff, float amount, int num_frames);
            void            save(XmlElement* xmlChanNode);
            void            load(XmlElement* xmlChanNode);
            void            remap();
            ContextMenu*    createContextMenu();
            ContextMenu*    createContextMenuForEffect(Eff* eff);
            void            activateEffectMenuItem(Eff* eff, std::string mi);
            void            activateContextMenuItem(std::string mi);
            Eff*            addEffectFromBrowser(BrwEntry* de);
            void            drawself(Graphics& g);
            void            handleMouseWheel(InputEvent& ev);
            void            handleMouseDown(InputEvent& ev);
            void            handleMouseUp(InputEvent& ev);
            void            setBufferSize(unsigned int bufferSize);
            void            setSampleRate(float sampleRate);
            void            placeEffectBefore(Eff* eff, Eff* before);
            bool            handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool            handleObjDrop(Gobj * obj,int mx,int my,unsigned flags);
            void            deleteEffect(Eff* eff);
            void            reset();
};


class Mixer : public Gobj
{
protected:

            Eff*            currentEffect;
            Scroller*       scroller;
            int             masterSectionWidth;

public:

            MixChannel*     masterChannel;
            MixChannel*     sendChannel[NUM_SENDS];

            Mixer();
            ~Mixer();
            void            init();
            void            mixAll(int num_frames);
            void            cleanBuffers(int num_frames);
            void            setCurrentEffect(Eff* eff);
            void            remap();
            void            drawself(Graphics& g);
            void            resetAll();
            void            handleChildEvent(Gobj * obj, InputEvent& ev);
            void            setOffset(int offs);
            int             getOffset()                 { return xOffset; }
            void            updateChannelIndexes();
            int             getInstrChannelsRange();
            Eff*            getCurrentEffect() { return currentEffect; }
            MixChannel*     addMixChannel(Instrument* instr);

private:

            int             xOffset;
};

Eff* CreateEffect(std::string effname);


