

#pragma once


#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"
#include "36_scroller.h"

#include <list>
#include <string>



Eff* CreateEffect(std::string effname);


class SendControl : public Control
{
public:
            SendControl(std::string name, bool levelCtrl);
            MixChannel*         getChannel()    {   return sendChannel;  }
            void                handleMouseWheel(InputEvent& ev) {}
            void                handleMouseDown(InputEvent& ev) {}
            void                handleMouseDrag(InputEvent& ev) {}

private:

            void                drawSelf(Graphics & g);
            void                remap();

            int                 chIndex;
            Parameter*          sendLevel;
            MixChannel*         sendChannel;
            Knob*               sendKnob;
};

class MixChannel : public Scrolled
{
friend  Audio36;
friend  ChanVU;
friend  Eff;
friend  Mixer;
friend  InstrPanel;


public:
            MixChannel();
            MixChannel(Instrument* i);
            ~MixChannel();

            void                activateEffectMenuItem(Eff* eff, std::string mi);
            void                activateMenuItem(std::string mi);
            void                addEffect(Eff* eff);
            Eff*                addEffectFromBrowser(BrwListEntry* de);
            ContextMenu*        createContextMenu();
            ContextMenu*        createContextMenuForEffect(Eff* eff);
            void                drawSelf(Graphics& g);
            void                doSend(float * sendbuff, float amount, int num_frames);
            void                deleteEffect(Eff* eff);
            Instrument*         getInstr()  { return instr; }
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

private:

            std::string         chanTitle;
            Gobj*               dropObj;
            std::list<Eff*>     effs;
            float               inbuff[MAX_BUFF_SIZE*2];
            Instrument*         instr;
            MixChannel*         mchanout;
            bool                master;
            int                 muteCount;
            Button36*           mutetoggle;
            float               outbuff[MAX_BUFF_SIZE*2];
            Knob*               panKnob;
            Parameter*          panParam;
            bool                soloparam;
            Button36*           solotoggle;
            SendControl*        send1;
            SendControl*        send2;
            SendControl*        send3;
            SendControl*        send4;
            SendControl*        sendOut;
            Parameter*          volParam;
            ChanVU*             vu;
            Knob*               volKnob;

};



