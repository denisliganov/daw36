
#pragma once


#include <mutex>
#include <list>

#include "36_globals.h"
#include "36_objects.h"
#include "36_text.h"


class JuceComponent;
class JuceListener;
class Hintbox;
class SubWindow;
class WinObject;





class CommonWindow  : public DocumentWindow
{
protected:

        WinObject*          winObject;

public:

        CommonWindow() : DocumentWindow(T(""), Colours::white, 0, true) {}
        WinObject*      getWinObject() { return winObject; }
};

class MainWindow  : public CommonWindow
{
protected:

        std::list<SubWindow*>   childs;

        int xMouseDrag;
        int yMouseDrag;


public:

        MainWindow(JuceComponent* winComp);
        ~MainWindow();


        void        updateTitle();
        void        minimizeChilds();
        void        maximizeChilds();
        void        showMenuWindow(WinObject* comp, int x, int y);
        bool        isContextMenuActive();
        void        deleteContextMenu();
        std::string showAlertBox(std::string message, std::string bt1 = "OK", std::string bt2 = "", std::string bt3 = "");
        SubWindow*  createChildWindowFromComponent(Component* comp, int x = -1, int y = -1);
        SubWindow*  createChildWindowFromWinObject(WinObject* comp, int x = -1, int y = -1);
        void        deleteChildWindow(SubWindow* cw);
        void        setFocusToChild(SubWindow* sw);
        void        mouseDoubleClick(const MouseEvent& e);
        void        closeButtonPressed();
        void        mouseDown(const MouseEvent& e);
        bool        keyPressed(const KeyPress& key);
        void        activeWindowStatusChanged();
        void        broughtToFront();
        void        paint(Graphics& g);
        void        minimisationStateChanged (bool isNowMinimised);
        void        lookAndFeelChanged();
        void        resized();
        void        mouseDrag(const MouseEvent & e);
        bool        keyStateChanged(bool isKeyDown);
        bool        isKeyDown(int keyCode);
};

class WinButton  : public Button
{
protected:

        std::string     btName;

public:

        WinButton(const String& name) throw() : Button (name) { btName = name; }

        void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown);
};

class SubWindow : public CommonWindow
{
friend  MainWindow;

protected:

        bool                updFocus;
        bool                shown;
        Component*          cc;
        WinButton*          closeButton;
        MainWindow*         parentWindow;
        int                 xcoffs;
        int                 ycoffs;
        uint32              color;

        void        broughtToFront();
        void        parentHierarchyChanged();
        void        mouseDown(const MouseEvent& e);
        void        paint(Graphics& g);
        void        buttonClicked(Button* butt);
        void        resized();
        void        lookAndFeelChanged();
        void        closeButtonPressed();
        int         getDesktopWindowStyleFlags() const;

public:

        SubWindow(bool title_bar = true);
        SubWindow(Component* comp, bool title_bar = true);
        SubWindow(WinObject* wo, bool title_bar = true);
        ~SubWindow();

        bool    isShown()   { return shown; }
        void    setVisibility(bool vis);
        void    updateTitle();
        void    setColor(uint32 clr) { color = clr; }

};


typedef enum CursorType
{
    Cursor_Arrow,
    Cursor_LeftRight,
    Cursor_UpDown,
    Cursor_Copy,
    Cursor_Clone,
    Cursor_Brush,
    Cursor_Select,
    Cursor_Slide,
    Cursor_Null,
}CursorType;



class JuceListener : public Component, public Timer
{
friend  JuceComponent;

protected:

            JuceComponent*  mainComp;
            WinObject*      winObject;

            unsigned    getFlags(const MouseEvent& e);
            void        mouseUp(const MouseEvent& e);
            void        mouseDown(const MouseEvent& e);
            void        mouseMove(const MouseEvent& e);
            void        mouseDrag(const MouseEvent& e);
            void        mouseExit(const MouseEvent& e);
            void        mouseEnter(const MouseEvent& e);
            void        mouseWheelMove(const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);
            void        redrawPerAction();
            void        timerCallback();
            void        paint(Graphics& g);
    virtual void        modifierKeysChanged (const ModifierKeys& modifiers);
            bool        iamOnMenu();

public:

        JuceListener(JuceComponent* main_comp);

        //bool keyPressed(const KeyPress &key);
};

class JuceComponent : public Component
{
friend  JuceListener;
friend  Gobj;
friend  WinObject;

protected:

            JuceListener*           listen;
            WinObject*              winObject;

public:

            JuceComponent(WinObject* win);
            ~JuceComponent();

            void        paint(Graphics& g);
            void        resized();
            void        buttonClicked(Button* button);
            void        repaintObject(Gobj* obj);
            void        makeSnapshot(Image** img, int x, int y, int w, int h, Image* otherimg = NULL);
            void        addRepaint(int x, int y, int w, int h);
            void        addChangedObject(Gobj* obj);
            void        setCursor(CursorType cur_type);
    virtual void        handleWindowResize(int wx, int wh) {}
            WinObject&  getWinObject() { return *winObject; }
};


class Hintbox : public DocumentWindow, public Timer
{
protected:

        Gobj*       activeObj;
        String      text;
        int         x;
        int         y;
        int         tx;
        int         ty;
        bool        blocked;
        FontId      font;
        WinObject*  winObject;

        void        mouseEnter(const MouseEvent& e);
        void        timerCallback();
        void        paint(Graphics& g);
        int         getDesktopWindowStyleFlags() const;

public:


        Hintbox(WinObject* wobj);
        ~Hintbox() {};

        void        setText(String & newtext, int text_x, int text_y);
        void        restart(Gobj* obj, int xc, int yc);
        void        stop();
        bool        isBlocked() { return blocked; }
};

class WinObject : public Gobj, public JuceComponent
{
friend  Gobj;
friend  JuceComponent;
friend  JuceListener;
friend  SubWindow;
friend  MainWindow;
friend  Hintbox;
friend  ScrollTimer;

protected:

            HANDLE                  guiMutex;
            std::list<Gobj*>        changedObjects;
            std::list<Gobj*>        highlights;
            std::list<Rect*>        repaints;
            std::list<Gobj*>        vus;

            CommonWindow*       holderWindow;
            DragAndDrop*        drag;
            Gobj*               lastActiveObj;
            Gobj*               activeObj;
            Hintbox*            hintBox;
            Gobj*               hintObj;
            InputEvent          lastEvent;
            int                 dragDistance;

    virtual void                updActiveObject(InputEvent& ev);
            void                showMenu(ContextMenu* menu, int x, int y);
            void                dragDrop(int mouse_x, int mouse_y, unsigned int flags);
            void                updateHint(InputEvent& ev);
            void                setWidthHeight(int wnew,int hnew);

            void                handleMouseEnter(InputEvent& ev);
            void                handleMouseLeave(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseMove(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
    virtual void                handleClose() {}

public:

            WinObject();
            ~WinObject();
            void                deleteWindow(SubWindow* sw);
            SubWindow*          addWindow(WinObject* comp);
            SubWindow*          addLegacyWindow(Component* comp);
            bool                canDrag(Gobj* obj);
            void                dragAdd(Gobj* drag_obj, int mx, int my);
            void                setCursor(CursorType cur_type);
            void                setMousePosition(int x, int y);
            Gobj*               getActiveObj() { return activeObj; }
            void                refreshActiveObject();
            InputEvent&         getLastEvent() { return lastEvent; }
            Image*              createSnapshot(int x, int y, int width, int height);
            void                registerObject(Gobj* obj);
            void                unregisterObject(Gobj* obj);
            bool                isDragging();
};


