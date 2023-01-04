

#pragma once


#include "36_globals.h"
#include "36_objects.h"




typedef enum
{
    Cursor_Default,
    Cursor_ElemEdit
}CursMode;


typedef enum KeyFlags
{
    kbd_shift   = 4,
    kbd_ctrl    = 8,
    kbd_alt     = 16
}KeyFlags;


enum keys
{
    // ASCII set. Should be supported everywhere

    key_backspace      = 8,
    key_tab            = 9,
    key_clear          = 12,
    key_return         = 13,
    key_pause          = 19,
    key_escape         = 27,

    // Keypad 

    key_delete         = 127,
    key_kp0            = 256,
    key_kp1            = 257,
    key_kp2            = 258,
    key_kp3            = 259,
    key_kp4            = 260,
    key_kp5            = 261,
    key_kp6            = 262,
    key_kp7            = 263,
    key_kp8            = 264,
    key_kp9            = 265,
    key_kp_period      = 266,
    key_kp_divide      = 267,
    key_kp_multiply    = 268,
    key_kp_minus       = 269,
    key_kp_plus        = 270,
    key_kp_enter       = 271,
    key_kp_equals      = 272,

    // Arrow-keys and stuff

    key_up             = 273,
    key_down           = 274,
    key_right          = 275,
    key_left           = 276,
    key_insert         = 277,
    key_home           = 278,
    key_end            = 279,
    key_page_up        = 280,
    key_page_down      = 281,

    // Functional keys. You'd better avoid using
    // f11...f15 in your application if you want 
    // it to be portable

    key_f1             = 282,
    key_f2             = 283,
    key_f3             = 284,
    key_f4             = 285,
    key_f5             = 286,
    key_f6             = 287,
    key_f7             = 288,
    key_f8             = 289,
    key_f9             = 290,
    key_f10            = 291,
    key_f11            = 292,
    key_f12            = 293,
    key_f13            = 294,
    key_f14            = 295,
    key_f15            = 296,

    // The possibility of using these keys is 
    // very restricted. Actually it's guaranteed 
    // only in win32_api and win32_sdl implementations

    key_numlock        = 300,
    key_capslock       = 301,
    key_scrollock      = 302,

    // Phew!
    end_of_key_codes
};



#define KEYNUM      256
#define CHARNUM     128


struct KeyNoteMap
{
        bool            pressed;
        int             noteValue;
};


class KeyHandler : public Gobj
{
public:

        unsigned        keyMap[KEYNUM];
        KeyNoteMap      keyNoteMap[KEYNUM];

        KeyHandler();

        void            initKeymap();
        void            advanceView(float dtick, int dline);
        void            handleChar(char c);
        void            handleNoteKey(int key, int note_val, bool press);
        void            handleKeyPressed(char key_code, char ch, unsigned flags);
        void            handleKeyOrCharPressed(unsigned key, char character, unsigned flags);
        int             mapKeyToNote(int key);
        void            handleKeyStateChange(bool key_down);
};



