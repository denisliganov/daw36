

#pragma once


#include "36_note.h"





class Pattern : public Note
{
public:

            bool            ranged;
            bool            muted;
            bool            looped;
            bool            playing;
            tframe          startFrame;
            tframe          currFrame;
            tframe          frameCount;
            tframe          endFrame;
            double          playTick;

            Instrument*     instr;
            Pattern*        ptBase;

            std::list<Element*>         elems;
            std::list<Pattern*>         instances;

            std::list<Event*>           events;
            std::list<Event*>::iterator pendingEvent;


            Pattern(char* nm, float tk1, float tk2, int tr1, int tr2, bool tracks);
            ~Pattern();
            void            init(char* nm, bool tracks);
        Pattern*            clone(float new_tick, int new_trackline);
        Pattern*            clone();
            void            copyParams(Pattern* newPatt);
            void            save(XmlElement* xmlNode);
            void            load(XmlElement* xmlNode);
            void            move(float dtick, int dtrack);
            bool            checkVisible(Grid* grid);
         Element*           checkElement(float tick, int trknum);
            void            deleteAllElements(bool flush, bool preventundo);
            void            recalculate();
            void            addElement(Element* el);
            void            addInstance(Pattern* child);
            void            removeInstance(Pattern* child);
            void            removeElement(Element* el);
            long            getLastElementFrame();
            void            setNewName(const char* name);
    virtual void            handleMouseUp(InputEvent& ev);
            void            drawOnGrid(Graphics& g, Grid* grid);
            double          getPlayTick();
            void            setPlayTick(double tick);
            void            setPatt(Pattern* pPt);
            bool            isLooped();
            void            setLoop();
            bool            isPlaying();
            void            resetPosition();
            void            activate();
            void            setBounds(long start, long end);
            void            tickFrame(long nc, long dcount, long fpb);
            void            setFrame(long frame);
            long            getFrame();
            long            getGlobalFrame();
            void            setGlobalFrame(long frame);
            long            getGlobalEndFrame();
            long            getGlobalStartFrame();
            void            deactivate();
            void            resetLoop();
            void            queueEvent();
            void            adjustBounds();
            void            removeTrigger(Trigger* tg);
            void            placeTrigger(Trigger* tg);
            void            preInitTriggers(long frame, bool activate_env, bool paraminit = true);
            void            updateEvents();
            long            requeueEvent(bool change);
            void            getSmallestCountDown(long* count);
            void            processEnvelopes(long buffframe, long num_frames, long curr_frame);
};



