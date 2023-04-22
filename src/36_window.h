
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
public:
            CommonWindow() : DocumentWindow(T(""), Colours::white, 0, true) {}

            void                closeButtonPressed();
            WinObject*          getWinObject();

protected:

            WinObject*          winObject;
};

class MainWindow  : public CommonWindow
{
public:

            MainWindow(JuceComponent* winComp);
            ~MainWindow();

            void                activeWindowStatusChanged();
            void                broughtToFront();
            SubWindow*          createChildWindowFromComponent(Component* comp, int x = -1, int y = -1);
            SubWindow*          createChildWindowFromWinObject(WinObject* comp, int x = -1, int y = -1, bool title_bar = true);
            void                deleteChildWindow(SubWindow* cw);
            void                deleteContextMenu();
            void                minimizeChilds();
            void                maximizeChilds();
            bool                isContextMenuActive();
            bool                isKeyDown(int keyCode);
            bool                keyPressed(const KeyPress& key);
            bool                keyStateChanged(bool isKeyDown);
            void                lookAndFeelChanged();
            void                mouseDrag(const MouseEvent & e);
            void                mouseDoubleClick(const MouseEvent& e);
            void                mouseDown(const MouseEvent& e);
            void                minimisationStateChanged (bool isNowMinimised);
            void                paint(Graphics& g);
            void                resized();
            void                showMenuWindow(WinObject* comp, int x, int y);
            std::string         showAlertBox(std::string message, std::string bt1 = "OK", std::string bt2 = "", std::string bt3 = "");
            void                setFocusToChild(SubWindow* sw);
            void                updateTitle();
            void                focusLost(FocusChangeType cause);
            void                focusGained(FocusChangeType cause);

protected:

            std::list<SubWindow*>   childs;

            int                 xMouseDrag;
            int                 yMouseDrag;
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

public:

            SubWindow(bool title_bar = true);
            SubWindow(Component* comp, bool title_bar = true);
            SubWindow(WinObject* wo, bool title_bar = true);
            ~SubWindow();

            bool                isOpen()   { return open; }
            void                setOpen(bool vis);
            void                setColor(uint32 clr) { color = clr; }
            void                closeButtonPressed();
            void                mouseDown(const MouseEvent& e);

protected:

            uint32              color;
            Component*          cc;
            WinButton*          closeButton;
            bool                open;
            MainWindow*         parentWindow;
            int                 xcoffs;
            int                 ycoffs;
            bool                updFocus;

            void                broughtToFront();
            int                 getDesktopWindowStyleFlags() const;
            void                lookAndFeelChanged();
            void                parentHierarchyChanged();
            void                paint(Graphics& g);
            void                resized();
            void                updateTitle();
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
friend  WinObject;

public:

            JuceListener(JuceComponent* main_comp);

            //bool keyPressed(const KeyPress &key);

protected:

            JuceComponent*      mainComp;
            WinObject*          winObject;
            const MouseEvent*         lastMouseEvent;

            unsigned            getFlags(const MouseEvent& e);
            bool                iamOnMenu();
            void                mouseUp(const MouseEvent& e);
            void                mouseDown(const MouseEvent& e);
            void                mouseMove(const MouseEvent& e);
            void                mouseDrag(const MouseEvent& e);
            void                mouseExit(const MouseEvent& e);
            void                mouseEnter(const MouseEvent& e);
            void                mouseWheelMove(const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);
            virtual void        modifierKeysChanged(const ModifierKeys& modifiers);
            void                paint(Graphics& g);
            void                redrawPerAction();
            void                timerCallback();
};

class JuceComponent : public Component
{
friend  JuceListener;
friend  Gobj;
friend  WinObject;

protected:

            JuceListener*       listen;
            WinObject*          winObject;

public:

            JuceComponent(WinObject* win);
            ~JuceComponent();

            void                addRepaint(int x, int y, int w, int h);
            bool                addChangedObject(Gobj* obj);
            void                buttonClicked(Button* button);
            WinObject&          getWinObject() { return *winObject; }
    virtual void                handleWindowResize(int wx, int wh) {}
            void                makeSnapshot(Image** img, int x, int y, int w, int h, Image* otherimg = NULL);
            void                paint(Graphics& g);
            void                resized();
            void                repaintObject(Gobj* obj);
            void                setCursor(CursorType cur_type);
};


class Hintbox : public DocumentWindow, public Timer
{
public:

            Hintbox(WinObject* wobj);
            ~Hintbox() {};

            void                setText(std::string new_text, int xc, int yc);
            void                restart(Gobj* obj, int xc, int yc);
            void                stop();
            bool                isBlocked() { return blocked; }

protected:

            Gobj*               activeObj;
            std::string         text;
            int                 x;
            int                 y;
            int                 tx;
            int                 ty;
            bool                blocked;
            FontId              font;
            WinObject*          winObject;

            int                 getDesktopWindowStyleFlags() const;
            void                mouseEnter(const MouseEvent& e);
            void                paint(Graphics& g);
            void                timerCallback();
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

public:

            WinObject();
            ~WinObject();
            SubWindow*          addWindow(WinObject* comp);
            SubWindow*          addLegacyWindow(Component* comp);
            Image*              createSnapshot(int x, int y, int width, int height);
            void                deleteWindow(SubWindow* sw);
            Gobj*               getActiveObj() { return activeObj; }
            InputEvent&         getLastEvent() { return lastEvent; }
            const MouseEvent*   getLastEvent1() { return listen->lastMouseEvent; }
    virtual void                handleClose() {}
            void                handleMouseEnter(InputEvent& ev);
            void                handleMouseLeave(InputEvent& ev);
            void                handleMouseWheel(InputEvent& ev);
            void                handleMouseMove(InputEvent& ev);
            void                handleMouseUp(InputEvent& ev);
            void                handleMouseDown(InputEvent& ev);
            void                handleMouseDrag(InputEvent& ev);
            void                refreshActiveObject();
            void                registerObject(Gobj* obj);
            void                setCursor(CursorType cur_type);
            void                setMousePosition(int x, int y);
            void                showMenu(ContextMenu* menu, int x, int y);
            void                setWidthHeight(int wnew,int hnew);
            void                unregisterObject(Gobj* obj);

protected:

            Gobj*               activeObj;
            int                 dragDistance;
            CommonWindow*       holderWindow;
            Hintbox*            hintBox;
            Gobj*               hintObj;
            InputEvent          lastEvent;

            std::list<Gobj*>    changedObjects;
            std::list<Gobj*>    touchedObjs;        // List of child objects covered by mouse
            HANDLE              guiMutex;
            std::list<Gobj*>    highlights;
            std::list<Rect*>    repaints;
            std::list<Gobj*>    vus;
    
    virtual void                updActiveObject(InputEvent& ev);
            void                updateHint(InputEvent& ev);
};


