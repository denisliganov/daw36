
#pragma once

#include "36_globals.h"
#include "36_vst.h"
#include "36_window.h"




static Array <VstComponent*> ActiveVstWindows;

class VstComponent   : //public AudioProcessorEditor,
                            public WinObject,
                            public Timer
{
public:

    VstComponent(Vst2Plugin *pPlug)
    {
        vstPlugin = pPlug;
        isOpen = false;
        wasShowing = false;
        pluginRefusesToResize = false;
        pluginWantsKeys = false;
        alreadyInside = false;
        recursiveResize = false;

        sizeCheckCount = 0;
        pluginHWND = 0;

        movementWatcher = new CompMovementWatcher (this);

        //activeVSTWindows.add (this);

        setSize (1, 1);
        setOpaque (true);

        //setVisible (true);
        //setBufferedToImage(true);
    }

    ~VstComponent()
    {
        deleteAndZero (movementWatcher);
        closePluginWindow();
        ActiveVstWindows.removeValue (this);
        //plugin.editorBeingDeleted (this);
    }

    void componentMovedOrResized()
    {
        if (recursiveResize)
            return;

        Component* const topComp = getTopLevelComponent();

        if (topComp->getPeer() != 0)
        {
            int x = 0, y = 0;
            relativePositionToOtherComponent (topComp, x, y);

            recursiveResize = true;

            if (pluginHWND != 0)
                MoveWindow (pluginHWND, x, y, getWidth(), getHeight(), TRUE);

            recursiveResize = false;
        }
    }

    void componentVisibilityChanged()
    {
        const bool isShowingNow = isShowing();

        if (wasShowing != isShowingNow)
        {
            wasShowing = isShowingNow;

            if (isShowingNow)
                openPluginWindow();
            else
                closePluginWindow();
        }

        componentMovedOrResized();
    }

    void componentPeerChanged()
    {
        closePluginWindow();
        openPluginWindow();
    }

    bool keyStateChanged()
    {
        return pluginWantsKeys;
    }

    bool keyPressed (const KeyPress&)
    {
        return pluginWantsKeys;
    }

    void paint (Graphics& g)
    {
        if (isOpen)
        {
            ComponentPeer* const peer = getPeer();

            if (peer != 0)
            {
                //peer->addMaskedRegion (getScreenX() - peer->getScreenX(),
                //                       getScreenY() - peer->getScreenY(),
                //                       getWidth(), getHeight());
            }
        }
        else
        {
            g.fillAll (Colours::black);
        }
    }

    void timerCallback()
    {
        if (--sizeCheckCount <= 0)
        {
            sizeCheckCount = 10;

            checkPluginWindowSize();
        }

        try
        {
            static bool reentrant = false;

            if (! reentrant)
            {
                reentrant = true;
                //_plugin->EditIdle();
                dispatch (effEditIdle, 0, 0, 0, 0);
                reentrant = false;
            }
        }
        catch (...)
        {}
    }

    void mouseDown (const MouseEvent& e)
    {
       (void) e;

        toFront (true);
    }

    void broughtToFront()
    {
        ActiveVstWindows.removeValue (this);
        ActiveVstWindows.add (this);
    }

private:

    Vst2Plugin * vstPlugin;

    bool        isOpen, wasShowing, recursiveResize;
    bool        pluginWantsKeys, pluginRefusesToResize, alreadyInside;

    HWND        pluginHWND;
    void*       originalWndProc;
    int         sizeCheckCount;

    void openPluginWindow()
    {
        if (isOpen || getWindowHandle() == 0)
            return;

        isOpen = true;

        ERect* rect = 0;
        //plugin.pEffect
        dispatch (effEditGetRect, 0, 0, &rect, 0);
        dispatch (effEditOpen, 0, 0, getWindowHandle(), 0);

        // do this before and after like in the steinberg example
        dispatch (effEditGetRect, 0, 0, &rect, 0);
        dispatch (effGetProgram, 0, 0, 0, 0); // also in steinberg code

        // Install keyboard hooks
        pluginWantsKeys = (dispatch (effKeysRequired, 0, 0, 0, 0) == 0);


        originalWndProc = 0;
        pluginHWND = GetWindow ((HWND) getWindowHandle(), GW_CHILD);

        if (pluginHWND == 0)
        {
            isOpen = false;
            setSize (300, 150);
            return;
        }

        #pragma warning (push)
        #pragma warning (disable: 4244)

        originalWndProc = (void*) GetWindowLongPtr (pluginHWND, GWL_WNDPROC);

        //if (! pluginWantsKeys)
        //    SetWindowLongPtr (pluginHWND, GWL_WNDPROC, (LONG_PTR) vstHookWndProc);

        #pragma warning (pop)

        int w, h;
        RECT r;
        GetWindowRect (pluginHWND, &r);
        w = r.right - r.left;
        h = r.bottom - r.top;

        if (rect != 0)
        {
            const int rw = rect->right - rect->left;
            const int rh = rect->bottom - rect->top;

            if ((rw > 50 && rh > 50 && rw < 2000 && rh < 2000 && rw != w && rh != h)
                || ((w == 0 && rw > 0) || (h == 0 && rh > 0)))
            {
                // very dodgy logic to decide which size is right.

                if (abs (rw - w) > 350 || abs (rh - h) > 350)
                {
                    SetWindowPos (pluginHWND, 0,
                                  0, 0, rw, rh,
                                  SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

                    GetWindowRect (pluginHWND, &r);

                    w = r.right - r.left;
                    h = r.bottom - r.top;

                    pluginRefusesToResize = (w != rw) || (h != rh);

                    w = rw;
                    h = rh;
                }
            }
        }

        // double-check it's not too tiny
        w = jmax (w, 32);
        h = jmax (h, 32);

        setSize (w, h);

        checkPluginWindowSize();

        startTimer (18 + JUCE_NAMESPACE::Random::getSystemRandom().nextInt (5));
        repaint();
    }

    void closePluginWindow()
    {
        if (isOpen)
        {
            isOpen = false;

            dispatch (effEditClose, 0, 0, 0, 0);

#pragma warning (push)
#pragma warning (disable: 4244)

            if (pluginHWND != 0 && IsWindow (pluginHWND)) SetWindowLongPtr (pluginHWND, GWL_WNDPROC, (LONG_PTR) originalWndProc);

#pragma warning (pop)

            stopTimer();

            if (pluginHWND != 0 && IsWindow (pluginHWND))
            {
                DestroyWindow (pluginHWND);
            }

            pluginHWND = 0;
        }
    }

    void checkPluginWindowSize() throw()
    {
        RECT r;

        GetWindowRect (pluginHWND, &r);
        const int w = r.right - r.left;
        const int h = r.bottom - r.top;

        if (isShowing() && w > 0 && h > 0 && (w != getWidth() || h != getHeight()) && ! pluginRefusesToResize)
        {
            setSize (w, h);
            sizeCheckCount = 0;
        }
    }

    class CompMovementWatcher  : public ComponentMovementWatcher
    {
public:

        CompMovementWatcher (VstComponent* const owner_)
            : ComponentMovementWatcher (owner_),
              owner (owner_)
        {
        }

        void componentMovedOrResized (bool /*wasMoved*/, bool /*wasResized*/)
        {
            owner->componentMovedOrResized();
        }

        void componentPeerChanged()
        {
            owner->componentPeerChanged();
        }

        void componentVisibilityChanged (Component&)
        {
            owner->componentVisibilityChanged();
        }

private:

        VstComponent* const owner;
    };

    CompMovementWatcher* movementWatcher;

    int dispatch (const int opcode, const int index, const int value, void* const ptr, float opt)
    {
        return vstPlugin->vstDispatch (opcode, index, value, ptr, opt);
    }

    // hooks to get keyboard events from VST windows..

    static LRESULT CALLBACK vstHookWndProc (HWND hW, UINT message, WPARAM wParam, LPARAM lParam)
    {
        for (int i = ActiveVstWindows.size(); --i >= 0;)
        {
            const VstComponent* const w = (const VstComponent*) ActiveVstWindows.getUnchecked (i);

            if (w->pluginHWND == hW)
            {
                if (message == WM_CHAR
                    || message == WM_KEYDOWN
                    || message == WM_SYSKEYDOWN
                    || message == WM_KEYUP
                    || message == WM_SYSKEYUP
                    /*|| message == WM_APPCOMMAND*/)
                {
                    SendMessage ((HWND) w->getTopLevelComponent()->getWindowHandle(),
                                 message, wParam, lParam);
                }

                return CallWindowProc ((WNDPROC) (w->originalWndProc),
                                       (HWND) w->pluginHWND,
                                       message,
                                       wParam,
                                       lParam);
            }
        }

        return DefWindowProc (hW, message, wParam, lParam);
    }

};



