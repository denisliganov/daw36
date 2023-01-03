


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

void KeyHandler::advanceView(float dtick, int dline)
{
    Grid* grid = MGrid;

    if(dtick != 0)
    {
        grid->setTickOffset(grid->getTickOffset() + dtick);
    }

    if(dline != 0)
    {
        grid->setVerticalOffset(grid->getVertOffset() + dline*grid->getLineHeight());
    }

    grid->redraw(false);
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

    handleKeyOrCharPressed(k != 0 ? k : key_code, c, flags);
}

void KeyHandler::handleKeyOrCharPressed(unsigned key, char character, unsigned flags)
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
                MGrid->selectDeselectAll(true);
            }
            else if(key == 'C')
            {
                MGrid->doAction(GridAction_Copy);
            }
            else if(key == 'X')
            {
                MGrid->doAction(GridAction_Cut);
            }
            else if(key == 'V')
            {
                MGrid->doAction(GridAction_Paste);
            }
            else if(key == 'Z')
            {
                _MHistory->undo();
        
                MGrid->redraw(true);
            }
            else if(key == 'Y')
            {
                _MHistory->redo();
        
                MGrid->redraw(true);
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
                }
                break;

                case key_right:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(float(MTransp->getTicksPerBeat()), 0);
                    }
                }
                break;

                case key_up:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(0, -2);
                    }
                }
                break;

                case key_down:
                {
                    if(flags && kbd_shift)
                    {
                        advanceView(0, 2);
                    }
                }break;

                case key_f1:
                {
                    //
                }
                break;

                case key_tab:
                {
                    if (MGrid->getDisplayMode() == GridDisplayMode_Steps)
                    {
                        MGrid->setDisplayMode(GridDisplayMode_Bars);
                    }
                    else if (MGrid->getDisplayMode() == GridDisplayMode_Bars)
                    {
                        MGrid->setDisplayMode(GridDisplayMode_Steps);
                    }

                    MGrid->redraw(true);
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
                    MGrid->doAction(GridAction_Delete);
                }
                break;

                default:
                {
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
                    }
                }
                break;
            }
        }
    }
    else
    {
        MGrid->handleModifierKeys(flags);
    }

    _MHistory->newGroup();

    MGrid->updateChangedElements();
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

        Grid* grid = MGrid;

        Note* note = grid->activeNote;

        if(note == NULL)
        {
            note = grid->putNote(grid->alignTick, grid->alignLine, note_val);

            grid->setActiveElement(note);
        }
        else
        {
            note->setNoteValue(note_val);
        }

        note->recalculate();

        note->preview(-1, true);

        grid->redraw(true);

    }
    else
    {
        keyNoteMap[key].pressed = false;

        MAudio->releasePreviewByNote(keyNoteMap[key].noteValue);

        keyNoteMap[key].noteValue = 0;
    }
}

