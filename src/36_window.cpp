
#include "Binarysrc/images.h"

#include "36_config.h"
#include "36_globals.h"
#include "36_window.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_config.h"
#include "36_vstwin.h"
#include "36_dragndrop.h"
#include "36_vu.h"
#include "36_controls.h"
#include "36_init.h"
#include "36_samplewin.h"
#include "36_menu.h"
#include "36_text.h"
#include "36_alertbox.h"
#include "36_dropbox.h"
#include "36_grid.h"
#include "36_ctrlpanel.h"
#include "36_button.h"




SubWindow*      MenuWindow = NULL;
SubWindow*      MsgBox = NULL;




void WinButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    if(!isButtonDown)
        gSetMonoColor(g, 0.4f);
    else
        gSetMonoColor(g, 0.6f);

    g.fillRect(0, 0, getWidth(), getHeight());

    int x = 10;
    int y = 3;
    int size = 6;

    gSetMonoColor(g, 1);

    if (btName == "close")
    {
        g.drawLine(getWidth()/2 - size/2, getHeight()/2 - size/2, getWidth()/2 + size/2, getHeight()/2 + size/2, 2);
        g.drawLine(getWidth()/2 - size/2, getHeight()/2 + size/2, getWidth()/2 + size/2, getHeight()/2 - size/2, 2);
    }
    else if (btName == "minimize")
    {
        g.drawLine(getWidth()/2 - size/2, getHeight()/2 + size/2 - 1, getWidth()/2 + size/2, getHeight()/2 + size/2 - 1, 2);
    }
    else if (btName == "maximize")
    {
        g.fillRect(getWidth()/2 - size/2, getHeight()/2 - size/2, size, size);
    }
}

void  CommonWindow::closeButtonPressed() 
{
    winObject->handleClose(); 
}

WinObject* CommonWindow::getWinObject() 
{ 
    return winObject; 
}

MainWindow::MainWindow(JuceComponent* winComp)
{
    setContentComponent(winComp, true, true);

    setTitleBarHeight(DAW_MAINWINDOW_TITLEBAR_HEIGHT);

    setTitleBarButtonsRequired(allButtons, false);

    setVisible(true);
    setFullScreen(true);
    setResizable(true, false);

    xMouseDrag = yMouseDrag = 0;

    winObject = (WinObject*)winComp;
    winObject->holderWindow = this;

    //setWantsKeyboardFocus(true);
    //setTitleBarHeight(0);

    const Rectangle main = Desktop::getInstance().getMainMonitorArea();

    lastNonFullScreenPos.setBounds(50, 50, main.getWidth()*0.8f, main.getHeight()*0.8f);

    getPeer()->grabFocus();
}

MainWindow::~MainWindow()
{
    // (the content component will be deleted automatically)

    if(MenuWindow != NULL)
    {
        delete MenuWindow;
    }

    while(childs.size() > 0)
    {
        deleteChildWindow(childs.front());
    }
}

void MainWindow::updateTitle()
{
    char name[MAX_NAME_LENGTH] = {0};

    strcpy(name, "M - ");

    strcat(name, MProject.getName());

    if(!MProject.getProjectPath().isEmpty())
    {
        strcat(name, ".cmm");
    }

    if(MProject.isChanged())
    {
        strcat(name, "*");
    }

    setName(name);
}

void MainWindow::minimisationStateChanged(bool isNowMinimised)
{
    if(isNowMinimised)
    {
        minimizeChilds();
    }
    else
    {
        maximizeChilds();
    }
}

void MainWindow::mouseDrag(const MouseEvent & ev)
{
    int x, y;
    Desktop::getMousePosition(x, y);

    int newX = x - xMouseDrag;
    int newY = y - yMouseDrag;

    int deltaX = newX - getX();
    int deltaY = newY - getY();

    if(isFullScreen())
    {
        //setFullScreen(false);
        setBounds(newX, newY, int(getWidth()*0.75f), int(getHeight()*0.75f));
    }
    else
    {
        setBounds(newX, newY, getWidth(), getHeight());
    }

    /*
    for(auto cw : childs)
    {
        cw->setBounds(cw->getX() + deltaX, cw->getY() + deltaY, cw->getWidth(), cw->getHeight());
    }*/

    //DocumentWindow::mouseDrag(ev);
}

void MainWindow::mouseDown(const MouseEvent& e)
{
    DocumentWindow::mouseDown(e);

    deleteContextMenu();

    broughtToFront();

    xMouseDrag = e.getMouseDownX();
    yMouseDrag = e.getMouseDownY();

    int x, y;

    Desktop::getMousePosition(x, y);

    if(isFullScreen() && (xMouseDrag > getWidth()*0.75f))
    {
        xMouseDrag = int(getWidth()*0.75f) - (getWidth() - xMouseDrag);
    }
}

void MainWindow::mouseDoubleClick (const MouseEvent& e)
{
    const int border = getBorderSize();

    if (e.x >= border && e.y >= border && e.x < getWidth() - border && e.y < border + getTitleBarHeight())
    {
        //getMaximiseButton()->triggerClick();

        if (isFullScreen() == true)
        {
            setFullScreen(false);
        }
        else if (isFullScreen() == false)
        {
            setFullScreen(true); 
        }
    }
}

bool MainWindow::keyPressed(const KeyPress& key)
{
    unsigned int flags = 0;

    ModifierKeys mods = key.getModifiers();

    if(mods.isCtrlDown())
    {
        flags |= kbd_ctrl;
    }

    if(mods.isShiftDown())
    {
        flags |= kbd_shift;
    }

    if(mods.isAltDown())
    {
        flags |= kbd_alt;
    }

    MKeys->handleKeyPressed((char)key.getKeyCode(), key.getTextCharacter(), flags);

    return true;
}

bool MainWindow::isKeyDown(int keyCode)
{
    return KeyPress::isKeyCurrentlyDown(keyCode);
}

bool MainWindow::keyStateChanged(bool isKeyDown)
{
    MKeys->handleKeyStateChange(isKeyDown);

    return true;
}

void MainWindow::activeWindowStatusChanged()
{
    DocumentWindow::activeWindowStatusChanged();

    bool active = isActiveWindow();
}

void MainWindow::broughtToFront()
{
    for(auto cw : childs)
    {
        if(cw->isOpen())
        {
            cw->updFocus = false;
            cw->toFront(false);
            cw->updFocus = true;
        }
    }

    getPeer()->grabFocus();
}

void MainWindow::maximizeChilds()
{
    for(auto cw : childs)
    {
        if(cw->isOpen())
        {
            cw->updFocus = false;
            cw->setVisible(true);
            cw->updFocus = true;
        }
    }
}

void MainWindow::minimizeChilds()
{
    for(auto cw : childs)
    {
        if(cw->isOpen())
        {
            cw->setVisible(false);
        }
    }
}

void MainWindow::deleteContextMenu()
{
    if(MenuWindow && MenuWindow->isVisible())
    {
        MenuWindow->setOpen(false);

        ContextMenu* menu = static_cast<ContextMenu*>(MenuWindow->getWinObject());

        menu->handleClose();

        //MenuWindow = NULL;
    }
}

void MainWindow::focusLost(FocusChangeType cause)
{
    //minimizeChilds();
}

void MainWindow::focusGained(FocusChangeType)
{
    //maximizeChilds();
}

void MainWindow::showMenuWindow(WinObject* comp, int x, int y)
{
    if(MenuWindow != NULL)
    {
        delete MenuWindow;

        MenuWindow = NULL;
    }

    MenuWindow = new SubWindow(comp, false);

    MenuWindow->setColor(0xff000000);
    MenuWindow->parentWindow = this;
    MenuWindow->setBounds(getX() + x, getY() + y + 20, comp->getWidth(), comp->getHeight());
    MenuWindow->setOpen(true);
}

bool MainWindow::isContextMenuActive()
{
    return (MenuWindow != NULL && MenuWindow->isVisible());
}

std::string MainWindow::showAlertBox(std::string message, std::string bt1, std::string bt2, std::string bt3)
{
    AlertBox* ab = new AlertBox(message, bt1, bt2, bt3);

    jassert(MsgBox == NULL);

    MsgBox = new SubWindow(ab, false);

    MsgBox->setColor(MenuColor);
    MsgBox->parentWindow = this;
    MsgBox->setBounds(getX() + getWidth()/2 - ab->getWidth()/2, getY() + getHeight()/2 - ab->getHeight()/2, ab->getWidth(), ab->getHeight());
    MsgBox->setOpen(true);
    MsgBox->runModalLoop();
    MsgBox->setOpen(false);

    std::string choice = ab->getChoice();

    delete MsgBox;

    MsgBox = NULL;

    return choice;
}

SubWindow* MainWindow::createChildWindowFromComponent(Component* comp, int x, int y)
{
    SubWindow* sw = new SubWindow(comp);

    sw->parentWindow = this;

    childs.push_back(sw);

    if(x == -1 || y == -1)
    {
        sw->setBounds(getWidth()/2 - comp->getWidth()/2, getHeight()/2 - comp->getHeight()/2, comp->getWidth(), comp->getHeight());
    }
    else
    {
        sw->setBounds(x, y, comp->getWidth(), comp->getHeight());
    }

    return sw;
}

SubWindow* MainWindow::createChildWindowFromWinObject(WinObject* wobj, int x, int y, bool title_bar)
{
    SubWindow* sw = new SubWindow(wobj, title_bar);

    sw->parentWindow = this;

    childs.push_back(sw);

    if(x == -1 || y == -1)
    {
        sw->setBounds(getWidth()/2 - wobj->getWidth()/2, getHeight()/2 - wobj->getHeight()/2, wobj->getWidth(), wobj->getHeight());
    }
    else
    {
        sw->setBounds(x, y, wobj->getWidth(), wobj->getHeight());
    }

    return sw;
}

void MainWindow::setFocusToChild(SubWindow* sw)
{
    childs.remove(sw);
    childs.push_back(sw);
}

void MainWindow::deleteChildWindow(SubWindow* sw)
{
    childs.remove(sw);

    delete sw;
}

void MainWindow::paint(Graphics& g)
{
    int titleHeight = getTitleBarHeight();

    int w = getWidth();
    int h = getHeight();

    // Dark border line
    gSetMonoColor(g, 0.2f);

    g.drawRect(0, 0, w, h);
    g.drawRect(2, 2, w - 4, h - 4);

    // Title bar
    gSetMonoColor(g, 0.25f);
    g.fillRect(1, 1, w - 2, titleHeight + getBorderThickness().getTop());
    //g.fillRect(3, titleHeight, w - 6, 3);

    // Bright border line
    gSetMonoColor(g, 0.3f);
    g.drawRect (1, 1, w - 2, h - 2);

    // Black divider betwen titlebar and window
    gSetMonoColor(g, 0.1f);
    g.fillRect(0, titleHeight + getBorderThickness().getTop() - 1, w, 1);

    gSetMonoColor(g, 1.f);

    g.setFont(*bld);
    g.drawSingleLineText(getName(), 6, 13);
}

void MainWindow::lookAndFeelChanged()
{
    for (int i = 0; i < 3; ++i)
        deleteAndZero (titleBarButtons[i]);

    if (! isUsingNativeTitleBar())
    {
        titleBarButtons[0] = new WinButton("minimize");
        titleBarButtons[1] = new WinButton("maximize");
        titleBarButtons[2] = new WinButton("close");

        for (int i = 0; i < 3; ++i)
        {
            if (titleBarButtons[i] != 0)
            {
                buttonListener.owner = this;

                titleBarButtons[i]->addButtonListener (&buttonListener);
                titleBarButtons[i]->setWantsKeyboardFocus (false);

                // (call the Component method directly to avoid the assertion in ResizableWindow)

                Component::addAndMakeVisible(titleBarButtons[i]);
                //titleBarButtons[i]->setBounds(0, 0, 18, 25);
            }
        }

        if (getCloseButton() != 0)
        {
            getCloseButton()->addShortcut(KeyPress (KeyPress::F4Key, ModifierKeys::altModifier, 0));
        }
    }

    activeWindowStatusChanged();

    ResizableWindow::lookAndFeelChanged();
}

void MainWindow::resized()
{
    ResizableWindow::resized();

    int titleHeight = getTitleBarHeight();

    BorderSize thick = getBorderThickness();

    for (int i = 0; i < 3; ++i)
    {
        if (titleBarButtons[i] != 0)
            titleBarButtons[i]->setBounds(getWidth() - 126 - 1 + i*50, 1, 25, 12);
    }
}

SubWindow::SubWindow(bool title_bar)
{
    cc = NULL;
    parentWindow = NULL;
    winObject = NULL;
    color = 0x0;

    if(title_bar)
    {
        //setTitleBarButtonsRequired(allButtons, false);

        buttonListener.owner = this;

        Component::addAndMakeVisible(closeButton = new WinButton("close"));

        titleBarButtons[2] = closeButton;

        closeButton->addShortcut(KeyPress (KeyPress::F4Key, ModifierKeys::altModifier, 0));
        closeButton->addButtonListener (&buttonListener);
        closeButton->setWantsKeyboardFocus (false);
    }

    xcoffs = 1;
    ycoffs = title_bar? 24 : 1;

    setDropShadowEnabled(false);

    setResizable(true, false);

    setOpen(false);

    updFocus = true;
}

SubWindow::SubWindow(WinObject* comp, bool title_bar)  : SubWindow(title_bar)
{
    cc = (Component*)comp;

    winObject = comp;

    winObject->holderWindow = this;

    Component::addAndMakeVisible(comp);

    setOpen(false);
}

SubWindow::SubWindow(Component* comp, bool title_bar)  : SubWindow(title_bar)
{
    cc = (Component*)comp;

    Component::addAndMakeVisible(comp);
}

SubWindow::~SubWindow()
{
    //removeChildComponent(cc);
    //removeChildComponent(closeButton);

    if(winObject)
        delete winObject;
    else if(cc)
        delete cc;
}

void SubWindow::broughtToFront()
{
    if(updFocus)
    {
        if(parentWindow != NULL && MenuWindow != this && MsgBox != this)
        {
            parentWindow->setFocusToChild(this);
        }
    }
}

void SubWindow::resized()
{
    ResizableWindow::resized();

    if(cc)
    {
        cc->setBounds(xcoffs, ycoffs, cc->getWidth(), cc->getHeight());

        setBounds(getX(), getY(), cc->getWidth() + 2, ycoffs + cc->getHeight() + 1);

        int btSize = 20;

        if(titleBarButtons[2])
        {
            titleBarButtons[2]->setBounds(getWidth() - 22, 2, btSize, btSize);
        }
    }
}

void SubWindow::setOpen(bool vis)
{
    open = vis;

    setVisible(vis);
}

void SubWindow::closeButtonPressed()
{
    exitModalState(0);

    setOpen(false);

    if(winObject)
    {
        winObject->handleClose();
    }
}

void SubWindow::paint(Graphics& g)
{
    int titleHeight = getTitleBarHeight();

    gSetMonoColor(g, 0.25f);
    g.fillRect(0, 0, getWidth(), getHeight());

    gSetMonoColor(g, 0.4f);
    g.drawRect(0, 0, getWidth(), getHeight());

    //drawGlassRect(g, (float)0, (float)0, getWidth(), 23, Colour(100, 100, 100), 0, 0, true, true, false, false);

    gSetMonoColor(g, 1.f);
    g.setFont(*bld);
    g.drawSingleLineText(winObject->getName(), 7, 16);
}

void SubWindow::lookAndFeelChanged()
{
    activeWindowStatusChanged();

    ResizableWindow::lookAndFeelChanged();
}

void SubWindow::updateTitle()
{
    //char name[256];
    //pPlug->GetDisplayName(name, 0);

    //char *prog = NULL;
    //this->pPlugin->GetProgramName(prog); // temporarily postpone preset name
    //strName += ": ";
    //strName += prog;

    //String strName(vst2->objTitle.data());
    //setName(strName);
}

int SubWindow::getDesktopWindowStyleFlags() const
{
    int flags = 0;

    //flags |= ComponentPeer::windowAppearsOnTaskbar;
    //flags |= ComponentPeer::windowIsSemiTransparent;

    return flags;
}

void SubWindow::parentHierarchyChanged()
{
    DocumentWindow::parentHierarchyChanged();
}

void SubWindow::mouseDown(const MouseEvent& e)
{
    DocumentWindow::mouseDown(e);

    MWindow->deleteContextMenu();
}


JuceListener::JuceListener(JuceComponent* main_comp)
{
    mainComp = main_comp;

    winObject = mainComp->winObject;

    setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
}

void JuceListener::modifierKeysChanged (const ModifierKeys& modifiers)
{
    unsigned int flags = 0;

    if(modifiers.isCtrlDown())
    {
        flags |= kbd_ctrl;
    }

    if(modifiers.isShiftDown())
    {
        flags |= kbd_shift;
    }

    if(modifiers.isAltDown())
    {
        flags |= kbd_alt;
    }

    MKeys->handleKeyPressed(0, 0, flags);
}

void JuceListener::timerCallback()
{
    WaitForSingleObject(mainComp->getWinObject().guiMutex, INFINITE);

    for(auto vuObj : mainComp->getWinObject().vus)
    {
        ChanVU* vu = dynamic_cast<ChanVU*>(vuObj);

        vu->tick();
    }

    ReleaseMutex(mainComp->getWinObject().guiMutex);

    redrawPerAction();
}

void JuceListener::redrawPerAction()
{
    WaitForSingleObject(mainComp->getWinObject().guiMutex, INFINITE);

    if(mainComp->getWinObject().isChanged())
    {
        // Repaint whole

        mainComp->repaint();

        mainComp->getWinObject().redraw(false);
    }
    else if (mainComp->bufferedImage != NULL)
    {
        // Repaint only changed objects

        for (Rect* r : mainComp->getWinObject().repaints)
        {
            repaint(r->x, r->y, r->w, r->h);
        }

        Graphics imG(*mainComp->bufferedImage);

        std::list<Gobj*>  co = mainComp->getWinObject().changedObjects;

        for(Gobj* obj : co)
        {
            if (obj->isShown() && obj->isChanged())
            {
                if(obj->getObjGroup() != ObjGroup_Highlight)
                {
                    obj->drawloop(imG);
                }
            }
        }
    }

    // Empty changed objects list
    mainComp->getWinObject().changedObjects.clear();

    // Empty repaint rectangles list
    while(mainComp->getWinObject().repaints.size() > 0)
    {
        Rect* rect = mainComp->getWinObject().repaints.front();

        mainComp->getWinObject().repaints.pop_front();

        delete rect;
    }

    ReleaseMutex(mainComp->getWinObject().guiMutex);
}

void JuceListener::paint(Graphics& g)
{
    // Draw highlights here

    for (Gobj* obj : mainComp->getWinObject().highlights)
    {
        if (obj->isShown())
        {
            obj->drawloop(g);
        }
    }
}

unsigned JuceListener::getFlags(const MouseEvent& e)
{
    unsigned int flags = 0;

    if(e.mods.isCtrlDown())
        flags |= kbd_ctrl;

    if(e.mods.isShiftDown())
        flags |= kbd_shift;

    if(e.mods.isAltDown())
        flags |= kbd_alt;

    return flags;
}

void JuceListener::mouseWheelMove(const MouseEvent& e, float wheelIncrementX, float wheelIncrementY)
{
    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX();
    inputEvent.mouseY = e.getMouseDownY();
    inputEvent.keyFlags = getFlags(e);
    inputEvent.wheelDelta = int(wheelIncrementY * 256.0f) / 90;

    mainComp->getWinObject().handleMouseWheel(inputEvent);

    redrawPerAction();
}

void JuceListener::mouseMove(const MouseEvent &e)
{
    if(MWindow->isContextMenuActive() && !iamOnMenu())
    {
        return;
    }

    static InputEvent prevEvent = {};

    InputEvent inputEvent = {};

    inputEvent.mouseX = e.getMouseDownX();
    inputEvent.mouseY = e.getMouseDownY();
    inputEvent.keyFlags = getFlags(e);
    inputEvent.leftClick = e.mods.isLeftButtonDown();
    inputEvent.rightClick = e.mods.isRightButtonDown();

    //inputEvent.distance = (int)sqrt((prevEvent.mouseX - inputEvent.mouseX)^2 + (prevEvent.mouseY - inputEvent.mouseY)^2);

    bool changed = (inputEvent.mouseX != prevEvent.mouseX || inputEvent.mouseY != prevEvent.mouseY);

    if (changed)
    {
        mainComp->getWinObject().handleMouseMove(inputEvent);

        redrawPerAction();
    }

    prevEvent = inputEvent;
}

void JuceListener::mouseDrag(const MouseEvent &e)
{
    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX() + e.getDistanceFromDragStartX();
    inputEvent.mouseY = e.getMouseDownY() + e.getDistanceFromDragStartY();
    inputEvent.keyFlags = getFlags(e);
    inputEvent.leftClick = e.mods.isLeftButtonDown();
    inputEvent.rightClick = e.mods.isRightButtonDown();

    mainComp->getWinObject().handleMouseDrag(inputEvent);

    redrawPerAction();
}


void JuceListener::mouseDown(const MouseEvent& e)
{
    lastMouseEvent = &e;

    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX();
    inputEvent.mouseY = e.getMouseDownY();
    inputEvent.keyFlags = getFlags(e);
    inputEvent.leftClick = e.mods.isLeftButtonDown();
    inputEvent.rightClick = e.mods.isRightButtonDown();
    inputEvent.doubleClick = e.getNumberOfClicks() == 2;
    inputEvent.clickDown = true;

    if(!iamOnMenu() && MWindow->isContextMenuActive())
    {
        mainComp->getWinObject().updActiveObject(inputEvent);

        MWindow->deleteContextMenu();
    }

    mainComp->getWinObject().handleMouseDown(inputEvent);

    if(iamOnMenu())
    {
        MWindow->deleteContextMenu();
    }
    else
    {
        redrawPerAction();
    }
}

void JuceListener::mouseUp(const MouseEvent& e)
{
    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX() + e.getDistanceFromDragStartX();
    inputEvent.mouseY = e.getMouseDownY() + e.getDistanceFromDragStartY();
    inputEvent.keyFlags = getFlags(e);
    inputEvent.leftClick = e.mods.isLeftButtonDown();
    inputEvent.rightClick = e.mods.isRightButtonDown();
    inputEvent.doubleClick = e.getNumberOfClicks() == 2;
    inputEvent.clickDown = false;

    mainComp->getWinObject().handleMouseUp(inputEvent);

    redrawPerAction();
}

void JuceListener::mouseExit(const MouseEvent &e)
{
    if(MWindow->isContextMenuActive() && !iamOnMenu())
    {
        return;
    }

    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX();
    inputEvent.mouseY = e.getMouseDownY();
    inputEvent.keyFlags = getFlags(e);

    mainComp->getWinObject().handleMouseLeave(inputEvent);

    redrawPerAction();
}

void JuceListener::mouseEnter(const MouseEvent &e)
{
    if(MWindow->isContextMenuActive() && !iamOnMenu())
    {
        return;
    }

    InputEvent inputEvent = {};
    inputEvent.mouseX = e.getMouseDownX();
    inputEvent.mouseY = e.getMouseDownY();
    inputEvent.keyFlags = getFlags(e);

    mainComp->getWinObject().handleMouseEnter(inputEvent);

    redrawPerAction();

    //    mainComp->getParentComponent()->getPeer()->grabFocus();
}

bool JuceListener::iamOnMenu()
{
    return (mainComp->getWinObject().holderWindow == MenuWindow);
}

JuceComponent::JuceComponent(WinObject* win)
{
    addAndMakeVisible(listen = new JuceListener(this));

    winObject = win;

    listen->startTimer(10);

    setOpaque(true);

    setBufferedToImage(true);

    setSize(100, 100);
}

JuceComponent::~JuceComponent()
{
    listen->stopTimer();

    deleteAndZero(listen);
}

void JuceComponent::paint(Graphics& g)
{
    winObject->drawloop(g);
}

void JuceComponent::repaintObject(Gobj* obj)
{
    listen->repaint(obj->getX1(), obj->getY1(), obj->getW(), obj->getH());
}

void JuceComponent::addRepaint(int x,int y,int w,int h)
{
    WaitForSingleObject(winObject->guiMutex, INFINITE);

    winObject->repaints.push_back(new Rect(x, y, w, h));

    ReleaseMutex(winObject->guiMutex);
}

bool JuceComponent::addChangedObject(Gobj* obj)
{
    WaitForSingleObject(winObject->guiMutex, INFINITE);

    std::list<Gobj*>  co = winObject->changedObjects;

    bool add = true;

restart:
    for(Gobj* o : co)
    {
        if(o->getParent() == obj)
        {
            co.remove(o);

            goto restart;
        }
        else if (o == obj->getParent())
        {
            add = false;
        }
    }

    winObject->changedObjects = co;

    if (add)
        winObject->changedObjects.push_back(obj);

    ReleaseMutex(winObject->guiMutex);

    return add;
}

void JuceComponent::resized()
{
    listen->setBounds(0, 0, getWidth(), getHeight());

    handleWindowResize(getWidth(), getHeight());

    repaint();
}

void JuceComponent::makeSnapshot(Image** img, int x, int y, int w, int h, Image* otherimg)
{
    if(bufferedImage != NULL && w > 0 && h > 0)
    {
        if(*img != NULL)
        {
            deleteAndZero(*img);
        }

       *img = new Image(flags.opaqueFlag ? Image::RGB : Image::ARGB, w, h, true);

        Graphics imageContext(**img);

        imageContext.setOrigin (-x, -y);

        if(otherimg == NULL)
        {
            imageContext.drawImageAt(bufferedImage, 0, 0);
        }
        else
        {
            imageContext.drawImageAt(otherimg, 0, 0);
        }
    }
}

void JuceComponent::setCursor(CursorType cur_type)
{
    switch (cur_type)
    {
        case Cursor_Arrow:
            listen->setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
            break;
        case Cursor_LeftRight:
            listen->setMouseCursor(MouseCursor(MouseCursor::LeftRightResizeCursor));
            break;
        case Cursor_UpDown:
            listen->setMouseCursor(MouseCursor(MouseCursor::UpDownResizeCursor));
            break;
        case Cursor_Copy:
            listen->setMouseCursor(*cursCopy);
            break;
        case Cursor_Clone:
            listen->setMouseCursor(*cursClone);
            break;
        case Cursor_Brush:
            listen->setMouseCursor(*cursBrush);
            break;
        case Cursor_Select:
            listen->setMouseCursor(*cursSelect);
            break;
        case Cursor_Slide:
            listen->setMouseCursor(*cursSlide);
            break;
        case Cursor_Null:
            listen->setMouseCursor(MouseCursor(MouseCursor::NoCursor));
            break;
    };
}


Hintbox::Hintbox(WinObject* wobj) : DocumentWindow(T("Hint"), Colours::white, DocumentWindow::closeButton, true)
{
    winObject = wobj;

    text = "Default";

    blocked = false;

    font = FontSmall;

    setAlwaysOnTop(true);
}

void Hintbox::timerCallback()
{
    if(blocked)
    {
        blocked = false;

        return;
    }

    if (MenuWindow != NULL && MenuWindow->isVisible())
    {
        return;
    }

    WaitForSingleObject(winObject->guiMutex, INFINITE);

    if (winObject->getActiveObj() != NULL && winObject->getActiveObj()->getHint() != "")
    {
        text = String(winObject->getActiveObj()->getHint().data());

        if(text.length() > 0)
        {
            setText(text, x, y);

            // Block redraw for 100 ms, to w/a occasional glitch

            blocked = true;

            startTimer(100);
        }
    }
    else
    {
        setVisible(false);
    }

    ReleaseMutex(winObject->guiMutex);
}

void Hintbox::setText(std::string new_text, int xc, int yc)
{
    text = new_text;

    int gap = 10;
    
    int tw = gGetTextWidth(font, (std::string)text);
    int th = gGetTextHeight(font);
    
    int w = tw + int(gap*1.5f);
    int h = th + gap;

    tx = w/2 - tw/2;
    ty = int(h/2 + th/2) - 2;

    setBounds(xc, yc, w, h);

    setVisible(true);

    repaint();
}

void Hintbox::restart(Gobj* obj, int xc, int yc)
{
    x = xc;
    y = yc;

    startTimer(555);
}

void Hintbox::stop()
{
    blocked = false;

    setVisible(false);
    stopTimer();
}

void Hintbox::paint(Graphics& g)
{
    gSetColor2(g, 0xffFFE080, .6f, 1);
    gFillRect(g, 0, 0, getWidth() - 1, getHeight() - 1);

    //gSetColor2(g, 0xffFFE080, .6f, 1);
    //gDrawRect(g, 0, 0, getWidth() - 1, getHeight() - 1);

    gSetColor2(g, 0xffFFE080, 1.f, 1);
    gText(g, font, text, tx, ty);
}

void Hintbox::mouseEnter(const MouseEvent &e)
{
    setVisible(false); // Avoid setting mouse on hint
}

int Hintbox::getDesktopWindowStyleFlags() const
{
    int flags = 0;

    flags |= ComponentPeer::windowIsSemiTransparent;

    return flags;
}


/////////////////////////////////////

WinObject::WinObject() : JuceComponent(this)
{
    activeObj = NULL;
    //lastActiveObj = NULL;
    holderWindow = NULL;
    hintObj = NULL;
    dragDistance = 0;

    hintBox = new Hintbox(this);

    Gobj::window = this; 

    setEnable(true);

    //addHighlight(drag = new DragAndDrop());

    guiMutex = CreateMutex(NULL, FALSE, NULL);
}

WinObject::~WinObject()
{
    // Delete child objects here, to prevent issue when ~Gobj destructor unregisters
    // objects being deleted and catches an error, since the internal lists won't be valid after this
    // destructor
    Gobj::deleteAllObjects();

    deleteAndZero(hintBox);
}

void WinObject::deleteWindow(SubWindow* sw)
{
    MWindow->deleteChildWindow(sw);
}

void WinObject::showMenu(ContextMenu* m, int x, int y)
{
    m->parent = (Gobj*)this;

    if(x + m->getW() > getW())
    {
        x = x - m->getW();
    }

    if(y + m->getH() > getH())
    {
        y = y - m->getH();
    }

    MWindow->showMenuWindow(m, x, y);
}

SubWindow* WinObject::addWindow(WinObject* wobj)
{
    wobj->hint = hint;
    wobj->parent = (Gobj*)this;

    return MWindow->createChildWindowFromWinObject(wobj);
}

SubWindow* WinObject::addLegacyWindow(Component* comp)
{
    return MWindow->createChildWindowFromComponent(comp);
}

void WinObject::setWidthHeight(int wnew,int hnew)
{
    Gobj::setCoords1(0, 0, wnew, hnew);

    setSize(wnew, hnew);
}

void WinObject::setCursor(CursorType cur_type)
{
    JuceComponent::setCursor(cur_type);
}

bool WinObject::canDrag(Gobj* obj)
{
    if (drag)
    {
        return drag->canDrag();
    }
    else
    {
        return false;
    }
}

void WinObject::dragAdd(Gobj * drag_obj, int mx, int my)
{
    drag->start(drag_obj, mx, my);
}

void WinObject::dragDrop(int mx, int my, unsigned int flags)
{
    drag->drop(mx, my, flags);
}

void WinObject::updateHint(InputEvent& ev)
{
    if (hintBox == NULL || getParentComponent() == NULL)
    {
        return;
    }

    int hintX = ev.mouseX + 20;
    int hintY = ev.mouseY;

    Rectangle r = getBounds();

    hintX += r.getX();
    hintY += r.getY();

    r = getParentComponent()->getBounds();

    hintX += r.getX();
    hintY += r.getY();

    if (activeObj && activeObj->getClickHint() != "")
    {
        if (ev.leftClick || ev.wheelDelta != 0)
        {
            hintBox->setText(activeObj->getClickHint(), hintX, hintY);
        }

        return;
    }

    if (ev.leftClick || ev.rightClick)
    {
        hintBox->stop();

        return;
    }

    if(!hintBox->isBlocked())
    {
        hintBox->setVisible(false);
        hintBox->stopTimer();

        if(activeObj)
        {
            hintBox->restart(activeObj, hintX, hintY);
        }
    }
}

void WinObject::updActiveObject(InputEvent& ev)
{
    Gobj* lastActiveObj = activeObj;

    activeObj = getLastTouchedObject(ev.mouseX, ev.mouseY);

    if (activeObj != lastActiveObj)
    {
        if (lastActiveObj != NULL && lastActiveObj != this)
        {
            lastActiveObj->setUnderMouse(false);

            lastActiveObj->handleMouseLeave(ev);
        }

        //lastActiveObj = activeObj;
    }

    if (activeObj == this)
    {
        activeObj = NULL;
    }
}

void WinObject::refreshActiveObject()
{
    updActiveObject(lastEvent);
}

void WinObject::handleMouseEnter(InputEvent& ev)
{
    if (MDragDrop && MDragDrop->isActive())
    {
       // addHighlight(MDragDrop);
    }

    updActiveObject(ev);

    lastEvent = ev;
}

void WinObject::handleMouseLeave(InputEvent& ev)
{
    if (MDragDrop && MDragDrop->getWindow() == this && MDragDrop->isActive())
    {
        //removeObject(MDragDrop);
    }

    updActiveObject(ev);

    updateHint(ev);

    lastEvent = ev;
}

void WinObject::handleMouseMove(InputEvent& ev)
{
    updActiveObject(ev);

    dragDistance = 0;

    if(activeObj != NULL)
    {
        activeObj->handleMouseMove(ev);
    }

    updateHint(ev);

    lastEvent = ev;
}

void WinObject::handleMouseWheel(InputEvent& ev)
{
    if (activeObj != NULL)
    {
        activeObj->handleMouseWheel(ev);
    }

    updateHint(ev);

    lastEvent = ev;
}

void WinObject::handleMouseUp(InputEvent& ev)
{
    if (MDragDrop && MDragDrop->isActive())
    {
        MDragDrop->drop(ev.mouseX, ev.mouseY, ev.keyFlags);
    }
    else if (activeObj != NULL)
    {
        activeObj->handleMouseUp(ev);

        if(ev.rightClick && dragDistance <= 2)
        {
            ContextMenu* m = activeObj->createContextMenu();

            if(m != NULL)
            {
                showMenu(m, ev.mouseX + 1, ev.mouseY + 1);
            }
        }
    }

    lastEvent = ev;

    //jassert(GPlaying == MCtrllPanel->btPlay->isPressed());
}

void WinObject::handleMouseDrag(InputEvent& ev)
{
    if (MDragDrop && MDragDrop->isActive())
    {
        Gobj* dropObj = getLastTouchedObject(ev.mouseX, ev.mouseY);

        MDragDrop->drag(dropObj, ev.mouseX, ev.mouseY);
    }
    else if(activeObj)
    {
        activeObj->handleMouseDrag(ev);
    }

    dragDistance += int(sqrt(abs(ev.mouseX - lastEvent.mouseX)^2 + abs(ev.mouseY - lastEvent.mouseY)^2));

    updateHint(ev);

    lastEvent = ev;
}

void WinObject::handleMouseDown(InputEvent& ev)
{
    dragDistance = 0;

    if (activeObj != NULL)
    {
        activeObj->handleMouseDown(ev);
    }

    updateHint(ev);

    lastEvent = ev;
}

void WinObject::setMousePosition(int x, int y)
{
    int cx = JuceComponent::getX();
    int cy = JuceComponent::getY();
    int wx = holderWindow->getX();
    int wy = holderWindow->getY();
    
    Desktop::setMousePosition(wx + cx + x, wy + cy + y);
}

Image* WinObject::createSnapshot(int x, int y, int w, int h)
{
    Image* image = NULL;

    if (bufferedImage != NULL)
    {
        makeSnapshot(&image, x, y, w, h, NULL);
    }
    else
    {
        image = createComponentSnapshot (Rectangle (x, y, getWidth(), getHeight()));
    }

    return image;
}

void WinObject::registerObject(Gobj * obj)
{
    WaitForSingleObject(guiMutex, INFINITE);
    
    if (obj->getObjGroup() == ObjGroup_Highlight)
    {
        highlights.push_back(obj);
    }
    
    if (obj->getObjGroup() == ObjGroup_VU)
    {
        vus.push_back(obj);
    }
    
    ReleaseMutex(getWinObject().guiMutex);
}

void WinObject::unregisterObject(Gobj * obj)
{
    WaitForSingleObject(guiMutex, INFINITE);

    if (changedObjects.size() > 0)
    {
        changedObjects.remove(obj);
    }

    if (highlights.size() > 0 &&obj->getObjGroup() == ObjGroup_Highlight)
    {
        highlights.remove(obj);
    }

    if (vus.size() > 0 && obj->getObjGroup() == ObjGroup_VU)
    {
        vus.remove(obj);
    }

    /*
    if(lastActiveObj == obj)
    {
        lastActiveObj = NULL;
    }*/
    
    if(activeObj == obj)
    {
        activeObj = NULL;
    }
    
    ReleaseMutex(getWinObject().guiMutex);
}

