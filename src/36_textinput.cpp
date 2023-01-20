//
//
//

#include "36_textinput.h"
#include "36_objects.h"
#include "36_utils.h"
#include "36_params.h"
#include "36_keyboard.h"
#include "36_juce_components.h"
#include "36_pattern.h"






TextString::TextString(const char* name, bool spc_allowed, Element* ow)
{
    memset(string, 0, MAX_NAME_LENGTH);

    if(name != NULL)
    {
        strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    //active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
    //element = ow;
}

TextString::TextString(const char* name, bool spc_allowed, Instrument* inst)
{
    memset(string, 0, MAX_NAME_LENGTH);

    if(name != NULL)
    {
        strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
}

TextString::TextString(const char* name, bool spc_allowed)
{
    memset(string, 0, MAX_NAME_LENGTH);

    if(name != NULL)
    {
        strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    //active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
}

TextString::~TextString()
{
}

void TextString::ProcessChar(char character)
{
    //if(active)
    {
        // if it's a letter/char
        if((curPos < MAX_NAME_LENGTH)&&(((character >= 0x41)&&(character <= 0x5A))||   // ABCD...
           ((character >= 0x61)&&(character <= 0x7A))||(character == 0x21)||   // abcd...
           ((character >= 0x30)&&(character <= 0x39))||   // 12345...
           (character == 0x5F)||    // Underline
           ((space_allowed == true)&&(character == 0x20))))   // space
        {
            if(curPos != maxPos || maxPos < maxlen)
            {
                string[curPos] = character;

                if(curPos == maxPos)
                {
                    maxPos++;
                }

                curPos++;
            }
        }
    }
}

void TextString::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_left:

            curPos--;

            if(curPos < 0)
            {
                curPos = 0;
                //go = -1;
            }
            break;

        case key_right:

            curPos++;

            if(curPos > maxPos)
            {
                curPos = maxPos;
                //go = 1;
            }
            break;

        case key_backspace:

            if(curPos > 0)
            {
                string[curPos - 1] = 0;

                curPos--;
                maxPos--;

                if(curPos < maxPos)
                {
                    PackZeros();
                }
            }
            break;

        //case key_delete:
    }
}

void TextString::PackZeros()
{
    int i = 0, grab = 0;
    char *c = string;

    while(i <= maxPos)
    {
        if(*c == 0)
        {
            grab++;
        }

       *c = *(c + grab);

        c++;
        i++;
    }
}

void TextString::toLeft()
{
    curPos = 0;
}

void TextString::toRight()
{
    curPos = maxPos;
}


void TextString::SetString(const char* name)
{
    memset(string, 0, MAX_NAME_LENGTH);

    if(name != NULL)
    {
        strcpy(string, name);

        curPos = strlen(name);
    }
    else
    {
        curPos = 0;
    }
}

void TextString::CapFirst()
{
    if(strlen(string) > 0 && ((string[0] >= 0x61)&&(string[0] <= 0x7A)))
    {
        string[0] -= 0x20;
    }
}

TextInput::TextInput(std::string def_str)
{
    textstr = def_str;
}

void TextInput::drawself(Graphics & g)
{
    gSetMonoColor(g, 0.3f);

    g.fillRect(x1 + 1, y1 + 1, width - 2, height - 2);

    g.drawRect(x1, y1, width, height);
}



