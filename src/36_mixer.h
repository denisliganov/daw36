

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
            MixChannel();
            MixChannel(Instrument* i);
            ~MixChannel();

            std::string         chanTitle;
            int                 contentheight; 
            Gobj*               dropObj;
            std::list<Eff*>     effs;
            Instrument*         instr;
            float               inbuff[MAX_BUFF_SIZE*2];
            BoolParam*          muteparam;
            MixChannel*         mchanout;
            int                 mutecount;
            bool                master;
            Button36*           mutetoggle;
            float               outbuff[MAX_BUFF_SIZE*2];
            Slider36*           panslider;
            Knob*               panKnob;
            Parameter*          panParam;
            BoolParam*          soloparam;
            SendSingle          sends[NUM_SENDS];
            Button36*           solotoggle;
            VolParam*           volParam;
            ChanVU*             vu;
            Slider36*           volslider;
            Knob*               volKnob;
            int                 voffs;

            void                activateEffectMenuItem(Eff* eff, std::string mi);
            void                activateMenuItem(std::string mi);
            void                addEffect(Eff* eff);
            Eff*                addEffectFromBrowser(BrwEntry* de);
            ContextMenu*        createContextMenu();
            ContextMenu*        createContextMenuForEffect(Eff* eff);
            void                drawSelf(Graphics& g);
            void                doSend(float * sendbuff, float amount, int num_frames);
            void                deleteEffect(Eff* eff);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj,int mx,int my);
            bool                handleObjDrop(Gobj * obj,int mx,int my,unsigned flags);
            void                init(Instrument* i);
            void                load(XmlElement* xmlChanNode);
            void                process(int num_frames, float* outbuff);
            void                placeEffectBefore(Eff* eff, Eff* before);
            void                remap();
            void                reset();
            void                removeEffect(Eff* eff);
            void                save(XmlElement* xmlChanNode);
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
};


class Mixer : public Gobj
{
public:
            Mixer();
            ~Mixer();

            Eff*            currentEffect;
            MixChannel*     masterChannel;
            MixChannel*     sendChannel[NUM_SENDS];

            MixChannel*     addMixChannel(Instrument* instr);
            void            cleanBuffers(int num_frames);
            void            drawSelf(Graphics& g);
            Eff*            getCurrentEffect() { return currentEffect; }
            void            handleChildEvent(Gobj * obj, InputEvent& ev);
            void            init();
            void            mixAll(int num_frames);
            void            remap();
            void            resetAll();
            void            setCurrentEffect(Eff* eff);
            void            updateChannelIndexes();
};

Eff* CreateEffect(std::string effname);


