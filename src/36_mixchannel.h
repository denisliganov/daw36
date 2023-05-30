

#pragma once


#include "36_globals.h"
#include "36_device.h"
#include "36_config.h"
#include "36_scroller.h"

#include <list>
#include <string>



class ChanOutToggle;
class SendKnob;
class MixDropHighlight;

Eff* CreateEffect(std::string effname);


class MixChannel : public Device36
{
friend  Audio36;
friend  ChanVU;
friend  Eff;
friend  InstrPanel;


public:
            MixChannel();
            MixChannel(Instr* i);
            ~MixChannel();

            void                activateEffectMenuItem(Eff* eff, std::string mi);
            void                activateMenuItem(std::string mi);
            void                addEffect(Eff* eff);
            Eff*                addEffectFromBrowser(BrwListEntry* de);
            void                addSend(MixChannel* mchan);
            ContextMenu*        createContextMenu();
            ContextMenu*        createContextMenuForEffect(Eff* eff);
            bool                canAcceptInputFrom(MixChannel* other_chan);
            void                decreaseMixCounter()      { mixCount--; jassert(mixCount >= 0); }
            void                delSend(MixChannel* mchan);
            void                drawSelf(Graphics& g);
            void                drawOverChildren(Graphics & g);
            void                doSend(float * sendbuff, float amount, int num_frames);
            void                deleteEffect(Eff* eff);
            Instr*         getInstr()  { return instr; }
            int                 getMixCounter()       { return mixCount; }
            void                handleChildEvent(Gobj * obj,InputEvent & ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleObjDrag(bool reset, Gobj * obj, int mx, int my);
            void                handleObjDrop(Gobj * obj, int mx, int my, unsigned flags);
            void                handleParamUpdate(Parameter * param);
            void                init(Instr* i);
            bool                isProcessed()           { return processed; }
            void                increaseMixCounter()      { mixCount++; }
            void                load(XmlElement* xmlChanNode);
            void                processChannel(int num_frames);
            void                process(int num_frames);
            void                placeEffectBefore(Eff* eff, Eff* before);
            void                prepareForMixing();
            void                remap();
            void                reset();
            void                removeEffect(Eff* eff);
            void                save(XmlElement* xmlChanNode);
            void                setBufferSize(unsigned int bufferSize);
            void                setSampleRate(float sampleRate);
            void                setInstrument(Instr* i);
            void                setOutChannel(MixChannel* mc);
            void                updateSends();


private:

            bool                processed;
            std::string         chanTitle;
            Gobj*               dropObj;
            MixDropHighlight*  dropHighlight;
            std::list<Eff*>     effs;
            Instr*         instr;
            MixChannel*         mchanout;
            int                 mixCount;
            Button36*           mutetoggle;
            ChanOutToggle*      outTg;
            Knob*               panKnob;
    std::list<SendKnob*>        sendsActive;
    std::list<MixChannel*>      sends;
            //ChanVU*             vu;
            Knob*               volKnob;
            Scroller*           vscr;

};



