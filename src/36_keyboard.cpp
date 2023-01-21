


#include "36_globals.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_pattern.h"
#include "36_note.h"
#include "36_ctrlpanel.h"
#include "36_instrpanel.h"
#include "36_instr.h"
#include "36_menu.h"
#include "36_edit.h"
#include "36_button.h"
#include "36_draw.h"
#include "36_project.h"
#include "36_juce_components.h"
#include "36_history.h"
#include "36.h"
#include "36_utils.h"
#include "36_transport.h"
#include "36_configwin.h"
#include "36_textinput.h"
#include "36_numbox.h"
#include "36_window.h"
#include "36_audio_dev.h"
#include "36_snapmenu.h"



KeyHandler::KeyHandler()
{
    initKeymap();
}

void KeyHandler::initKeymap()
{
    memset(keyMap, 0, sizeof(keyMap));

    keyMap[VK_BACK]       = key_backspace;
    keyMap[VK_TAB]        = key_tab;
    keyMap[VK_CLEAR]      = key_clear;
    keyMap[VK_RETURN]     = key_return;
    keyMap[VK_ESCAPE]     = key_escape;
    keyMap[VK_PAUSE]      = key_pause;
    keyMap[VK_CLEAR]      = key_clear;
    keyMap[VK_NUMPAD0]    = key_kp0;
    keyMap[VK_NUMPAD1]    = key_kp1;
    keyMap[VK_NUMPAD2]    = key_kp2;
    keyMap[VK_NUMPAD3]    = key_kp3;
    keyMap[VK_NUMPAD4]    = key_kp4;
    keyMap[VK_NUMPAD5]    = key_kp5;
    keyMap[VK_NUMPAD6]    = key_kp6;
    keyMap[VK_NUMPAD7]    = key_kp7;
    keyMap[VK_NUMPAD8]    = key_kp8;
    keyMap[VK_NUMPAD9]    = key_kp9;
    keyMap[VK_DECIMAL]    = key_kp_period;
    keyMap[VK_DIVIDE]     = key_kp_divide;
    keyMap[VK_MULTIPLY]   = key_kp_multiply;
    keyMap[VK_SUBTRACT]   = key_kp_minus;
    keyMap[VK_ADD]        = key_kp_plus;
    keyMap[VK_UP]         = key_up;
    keyMap[VK_DOWN]       = key_down;
    keyMap[VK_RIGHT]      = key_right;
    keyMap[VK_LEFT]       = key_left;
    keyMap[VK_INSERT]     = key_insert;
    keyMap[VK_DELETE]     = key_delete;
    keyMap[VK_HOME]       = key_home;
    keyMap[VK_END]        = key_end;
    keyMap[VK_PRIOR]      = key_page_up;
    keyMap[VK_NEXT]       = key_page_down;
    keyMap[VK_F1]         = key_f1;
    keyMap[VK_F2]         = key_f2;
    keyMap[VK_F3]         = key_f3;
    keyMap[VK_F4]         = key_f4;
    keyMap[VK_F5]         = key_f5;
    keyMap[VK_F6]         = key_f6;
    keyMap[VK_F7]         = key_f7;
    keyMap[VK_F8]         = key_f8;
    keyMap[VK_F9]         = key_f9;
    keyMap[VK_F10]        = key_f10;
    keyMap[VK_F11]        = key_f11;
    keyMap[VK_F12]        = key_f12;
    keyMap[VK_F13]        = key_f13;
    keyMap[VK_F14]        = key_f14;
    keyMap[VK_F15]        = key_f15;
    keyMap[VK_NUMLOCK]    = key_numlock;
    keyMap[VK_CAPITAL]    = key_capslock;
    keyMap[VK_SCROLL]     = key_scrollock;

    for (int k = 0; k < KEYNUM; k++)
    {
        keyNoteMap[k].noteValue = 0;
        keyNoteMap[k].pressed = false;
    }
}

int KeyHandler::mapKeyToNote(int key)
{
    int oct = MCtrllPanel->octaveBox->getValue();

    int note = -1000;

    int base0 = oct * 12 - 12;
    int base1 = oct * 12;

    switch(key)
    {
        // Upper row:

        case 0x51:
            note = base1 + 0;
            break;
        case 0x32:
            note = base1 + 1;
            break;
        case 0x57:
            note = base1 + 2;
            break;
        case 0x33:
            note = base1 + 3;
            break;
        case 0x45:
            note = base1 + 4;
            break;
        case 0x52:
            note = base1 + 5;
            break;
        case 0x35:
            note = base1 + 6;
            break;
        case 0x54:
            note = base1 + 7;
            break;
        case 0x36:
            note = base1 + 8;
            break;
        case 0x59:
            note = base1 + 9;
            break;
        case 0x37:
            note = base1 + 10;
            break;
        case 0x55:
            note = base1 + 11;
            break;
        case 0x49:
            note = base1 + 12;
            break;
        case 0x39:
            note = base1 + 13;
            break;
        case 0x4F:
            note = base1 + 14;
            break;
        case 0x30:
            note = base1 + 15;
            break;
        case 0x50:
            note = base1 + 16;
            break;

        // Lower row:

        case 0x5A:
            note = base0 + 0;
            break;
        case 0x53:
            note = base0 + 1;
            break;
        case 0x58:
            note = base0 + 2;
            break;
        case 0x44:
            note = base0 + 3;
            break;
        case 0x43:
            note = base0 + 4;
            break;
        case 0x56:
            note = base0 + 5;
            break;
        case 0x47:
            note = base0 + 6;
            break;
        case 0x42:
            note = base0 + 7;
            break;
        case 0x48:
            note = base0 + 8;
            break;
        case 0x4E:
            note = base0 + 9;
            break;
        case 0x4A:
            note = base0 + 10;
            break;
        case 0x4D:
            note = base0 + 11;
            break;
        case 0x2C:
            note = base0 + 12;
            break;
        case 0x4C:
            note = base0 + 13;
            break;
        case 0x2E:
            note = base0 + 14;
            break;
        case 0x3B:
            note = base0 + 15;
            break;
    }

    return note;
}

void KeyHandler::handleKeyPressed(char key_code, char c, unsigned flags)
{
    unsigned k = (key_code > 255) ? 0 : keyMap[key_code];

    MTextCursor->handleKeyOrCharPressed(k != 0 ? k : key_code, c, flags);
}

void KeyHandler::handleKeyStateChange(bool key_down)
{
    if (!key_down)
    {
        for (int k = 0; k < KEYNUM; k++)
        {
            if (keyNoteMap[k].pressed && !MWindow->isKeyDown(k))
            {
                keyNoteMap[k].pressed = false;

                if (keyNoteMap[k].noteValue > 0)
                {
                    handleNoteKey(k, keyNoteMap[k].noteValue, false);
                }
            }
        }
    }

    MGrid->updateChangedElements();
}

void KeyHandler::handleNoteKey(int key, int note_val, bool press)
{
    if (press)
    {
        keyNoteMap[key].pressed = true;
        keyNoteMap[key].noteValue = note_val;

        Note* note = MGrid->actnote;

        if(note == NULL)
        {
            note = MGrid->putnote(MTextCursor->getTick(), MTextCursor->getLine(), note_val);

            MGrid->setactivelem(note);
        }
        else
        {
            note->setnote(note_val);
        }

        note->recalc();

        note->preview(-1, true);

        MGrid->redraw(true);

    }
    else
    {
        keyNoteMap[key].pressed = false;

        MAudio->releasePreviewByNote(keyNoteMap[key].noteValue);

        keyNoteMap[key].noteValue = 0;
    }
}



TextCursor::TextCursor()
{
    grid = NULL;

    tick = 0;
    line = 0;
}

void TextCursor::setPos(float newTick, int newLine)
{
    setVisible(false);

    tick = newTick;
    line = newLine;

    updPos();

    redraw();
}

void TextCursor::updPos()
{
    grid = (Grid*)parent;

    if (grid)
    {
        if (grid->getDisplayMode() == GridDisplayMode_Bars)
        {
            if (grid->mode == GridMode_Default)
            {
                int x = grid->getXfromTick(tick) - grid->getX1();
                int y = grid->getYfromLine(line) - int(grid->getlh()) - grid->getY1() + 1;

                setCoords2(x, y, x, y + int(grid->getlh()) - 1);

                setVisible(true);
            }
        }

        grid->updbounds();
    }
}

void TextCursor::drawself(Graphics& g)
{
    fill(g, 1.f);
}

float TextCursor::getTick()
{
    return tick;
}

int TextCursor::getLine()
{
    return line;
}

void TextCursor::handleKeyOrCharPressed(unsigned key, char character, unsigned flags)
{
    if (key != 0)
    {
        if(key == 0x20)
        {
            TogglePlayback();

            MCtrllPanel->btPlay->toggle();
        }
        else if(flags & kbd_ctrl)
        {
            if(key == 'A')
            {
                grid->selall(true);
            }
            else if(key == 'C')
            {
                grid->action(GridAction_Copy);
            }
            else if(key == 'X')
            {
                grid->action(GridAction_Cut);
            }
            else if(key == 'V')
            {
                grid->action(GridAction_Paste);
            }
            else if(key == 'Z')
            {
                MHistory->undo();

                grid->redraw(true);
            }
            else if(key == 'Y')
            {
                MHistory->redo();

                grid->redraw(true);
            }

            if(key == 'O')
            {
                MProject.loadProject(0);
            }
            else if(key == 'S')
            {
                if(flags & kbd_shift)
                {
                    MProject.saveProject(true);
                }
                else
                {
                    MProject.saveProject(false);
                }
            }
        }        
        else
        {
            switch(key)
            {
                case key_escape:
                {
                    // 
                }
                break;
            
                case key_left:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(-float(MTransp->getTicksPerBeat()), 0);
                    }
                    else
                    {
                        setPos(tick - MCtrllPanel->getSnapMenu().getSnapSize(), line);
                    }
                }
                break;
            
                case key_right:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(float(MTransp->getTicksPerBeat()), 0);
                    }
                    else
                    {
                        setPos(tick + MCtrllPanel->getSnapMenu().getSnapSize(), line);
                    }
                }
                break;
            
                case key_up:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(0, -2);
                    }
                    else
                    {
                        setPos(tick, line - 1);
                    }
                }
                break;
            
                case key_down:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(0, 2);
                    }
                    else
                    {
                        setPos(tick, line + 1);
                    }
                }break;
            
                case key_f1:
                {
                    //
                }
                break;
            
                case key_tab:
                {
                    if (grid->getDisplayMode() == GridDisplayMode_Steps)
                    {
                        grid->setDisplayMode(GridDisplayMode_Bars);
                    }
                    else if (grid->getDisplayMode() == GridDisplayMode_Bars)
                    {
                        grid->setDisplayMode(GridDisplayMode_Steps);
                    }
            
                    grid->redraw(true);
                }
                break;
            
                case key_f2:
                {
                    SubWindow* wc = MObject->addWindow(new ConfigObject());
            
                    wc->setVisibility(true);
            
                    wc = MObject->addLegacyWindow(new ConfigComponent());
            
                    wc->setVisibility(true);
            
                    wc = MObject->addLegacyWindow(new RenderComponent());
            
                    wc->setVisibility(true);
            
                    wc = MObject->addLegacyWindow(new AboutComponent());
            
                    wc->setVisibility(true);
            
                    wc = MObject->addLegacyWindow(new HelpComponent());
            
                    wc->setVisibility(true);
            
                    wc = MObject->addLegacyWindow(new SampleComponent());
            
                    wc->setVisibility(true);
                }
                break;
            
                case key_delete:
                {
                    grid->action(GridAction_Delete);
                }
                break;
            
                default:
                {
                    handleChar(character);
                    /*
                    for (int k = 0; k < KEYNUM; k++)
                    {
                        if (!keyNoteMap[k].pressed && MWindow->isKeyDown(k))
                        {
                            keyNoteMap[k].pressed = true;
            
                            int noteVal = mapKeyToNote(k);
            
                            if (noteVal > 0)
                            {
                                handleNoteKey(k, noteVal, true);
                            }
                        } 
                    }*/
                }
                break;
            }
        }
    }
    else
    {
        grid->handleModifierKeys(flags);
    }

    MHistory->newGroup();

    grid->updateChangedElements();
}

void TextCursor::advanceView(float dtick, int dline)
{
    if(dtick != 0)
    {
        grid->sethoffs(grid->vscr->getoffs() + dtick);
    }

    if(dline != 0)
    {
        grid->vscr->setoffs(grid->vscr->getoffs() + dline*grid->getlh());
    }

    grid->redraw(false);
}

void TextCursor::handleChar(char c)
{
    char al[2] = {};

    al[0] = c;

    Instrument* i = MInstrPanel->getInstrByAlias(al);

    if (i != NULL)
    {
        MInstrPanel->setcurr(i);

        Note* note = grid->putnote(getTick(), getLine(), -1);

        grid->setactivelem(note);

        note->recalc();

        note->preview(-1, true);

        grid->redraw(true);
    }
}


