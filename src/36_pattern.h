

#pragma once


#include "36_note.h"





class Pattern : public Note
{
public:

            Pattern(char* nm, float tk1, float tk2, int tr1, int tr2, bool tracks);
            ~Pattern();
            void                adjustBounds();
            void                addElement(Element* el);
            void                addInstance(Pattern* child);
            void                activate();
            Pattern*            clone(float new_tick, int new_trackline);
            Pattern*            clone();
            bool                checkVisible(Grid* grid);
            Element*            checkElement(float tick, int trknum);
            void                copyParams(Pattern* newPatt);
            void                deactivate();
            void                deleteAllElements(bool flush, bool preventundo);
            void                drawOnGrid(Graphics& g, Grid* grid);
            double              getPlayTick();
            void                getSmallestCountDown(long* count);
            long                getFrame();
            long                getGlobalFrame();
            long                getGlobalEndFrame();
            long                getGlobalStartFrame();
            long                getLastElementFrame();
         std::list<Element*>&   getElems()  { return elems; }
            Pattern*            getBasePattern()    { return ptBase; }
    virtual void                handleMouseUp(InputEvent& ev);
            void                init(char* nm, bool tracks);
            bool                isLooped();
            bool                isPlaying();
            bool                isMuted()      { return muted; }
            bool                isBounded()     { return ranged; }
            void                load(XmlElement* xmlNode);
            void                move(float dtick, int dtrack);
            void                processEnvelopes(long buffframe, long num_frames, long curr_frame);
            void                placeTrigger(Trigger* tg);
            void                preInitTriggers(long frame, bool activate_env, bool paraminit = true);
            void                removeTrigger(Trigger* tg);
            void                queueEvent();
            long                requeueEvent(bool change);
            void                recalc();
            void                removeInstance(Pattern* child);
            void                removeElement(Element* el);
            void                resetPosition();
            void                resetLoop();
            void                setGlobalFrame(long frame);
            void                setBounds(long start, long end);
            void                setNewName(const char* name);
            void                setPlayTick(double tick);
            void                setPatt(Pattern* pPt);
            void                save(XmlElement* xmlNode);
            void                setLoop();
            void                setFrame(long frame);
            void                setBasePattern(Pattern* pt);
            void                tickFrame(long nc, long dcount, long fpb);
            void                updateEvents();

protected:

            tframe              currFrame;
            tframe              endFrame;
            tframe              frameCount;
            Instrument*         instr;
            bool                muted;
            bool                looped;
            Pattern*            ptBase;
            bool                playing;
            double              playTick;
            bool                ranged;
            tframe              startFrame;

            std::list<Element*>         elems;
            std::list<Pattern*>         instances;
            std::list<Event*>           events;
            std::list<Event*>::iterator pendingEvent;


};



