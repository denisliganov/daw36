
#pragma once

#include "36_objects.h"
#include "36_globals.h"
#include "36_controls.h"
#include "36_config.h"






class TextString : public Control
{
public:

            char        string[MAX_NAME_LENGTH];

            int         curPos;
            int         maxPos;
            int         cposx;
            int         maxlen;

            bool        space_allowed;

            TextString(const char* name, bool spc_allowed, Element* ow);
            TextString(const char* name, bool spc_allowed, Instrument* inst);
            TextString(const char* name, bool spc_allowed);
            ~TextString();

            void	ProcessChar(char character);
            void    ProcessKey(unsigned int key, unsigned int flags);
            void    toLeft();
            void    toRight();
            void    PackZeros();
            void    SetString(const char* name);
            void    CapFirst();
};



class TextInput : public Control
{
public:

        std::string     textstr;

                TextInput(std::string def_str);
        void    drawSelf(Graphics& g);
};



