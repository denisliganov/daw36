

#pragma once


#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"
#include "36_scroller.h"

#include <list>
#include <string>



Eff* CreateEffect(std::string effname);


class MixChannel : public Scrolled
{
friend  Mixer;
friend  ChanVU;
friend  Eff;

public:
            MixChannel();
            MixChannel(Instrument* i);
            ~MixChannel();

            std::string         chanTitle;
            Gobj*               dropObj;
            std::list<Eff*>     effs;
            Instrument*         instr;
            MixChannel*         mchanout;
            bool                master;
            Button36*           mutetoggle;
            Knob*               panKnob;
            Parameter*          panParam;
            bool                soloparam;
            Button36*           solotoggle;
            Parameter*          volParam;
            ChanVU*             vu;
            Knob*               volKnob;

            int                 muteCount;

            float               inbuff[MAX_BUFF_SIZE*2];
            float               outbuff[MAX_BUFF_SIZE*2];

            void                activateEffectMenuItem(Eff* eff, std::string mi);
            void                activateMenuItem(std::string mi);
            void                addEffect(Eff* eff);
            Eff*                addEffectFromBrowser(BrwListEntry* de);
            ContextMenu*        createContextMenu();
            ContextMenu*        createContextMenuForEffect(Eff* eff);
            void                drawSelf(Graphics& g);
            void                doSend(float * sendbuff, float amount, int num_frames);
            void                deleteEffect(Eff* eff);
            void                handleChildEvent(Gobj * obj,InputEvent & ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            bool                handleObjDrag(DragAndDrop& drag, Gobj * obj, int mx, int my);
            bool                handleObjDrop(Gobj * obj, int mx, int my, unsigned flags);
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



