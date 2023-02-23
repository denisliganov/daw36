

#pragma once


#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"


#include <list>
#include <string>



Eff* CreateEffect(std::string effname);


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
            bool                muteparam;
            MixChannel*         mchanout;
            int                 mutecount;
            bool                master;
            Button36*           mutetoggle;
            float               outbuff[MAX_BUFF_SIZE*2];
            Slider36*           panslider;
            Knob*               panKnob;
            Parameter*          panParam;
            bool                soloparam;
            Button36*           solotoggle;
            ParamVol*           volParam;
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



