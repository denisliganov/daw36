
#pragma once


#include <string>

#include "36_globals.h"
#include "36_objects.h"




#define BrwEntryHeight      (16)
#define BrwEntryOffset      (1)


const char WavExt[4] = "wav";
const char VstExt[4] = "dll";
const char ProjExt[4] = "cmm";


typedef enum FileType
{
    FType_Unknown           = 0x1,
    FType_Directory         = 0x2,
    FType_Wave              = 0x8,
    FType_VST               = 0x100,
    FType_Native            = 0x200,
    FType_Projects          = 0x800,
    FType_LevelDirectory    = 0x1000,
    FType_DiskSelector      = 0x2000,
    FType_DiskDrive         = 0x4000
}FileType;


class BrwEntry : public Gobj
{
public:

        BrwEntry() {};
        BrwEntry(FileType ft, long s, unsigned a, std::string nm, std::string pth);
        BrwEntry(DevClass dclass, std::string nm, std::string pth, std::string al);
        BrwEntry(Device36* preset_dev);

        virtual ~BrwEntry() {};

        int             listIndex;
        std::string     path;
        FileType        ftype;
        long            size;
        unsigned int    attrs;
        DevClass        devClass;
        std::string     alias;
        long            prindex;
        Device36*       dev;
        Browser*        browser;

        void    handleMouseWheel(InputEvent& ev)    { parent->handleMouseWheel(ev); };
        void    handleMouseDrag(InputEvent& ev)     { parent->handleMouseDrag(ev); }
        void    handleMouseDown(InputEvent& ev)     { parent->handleMouseDown(ev); }
        void    handleMouseUp(InputEvent& ev)       { parent->handleMouseUp(ev); }

        bool    isGenerator()       {return (devClass == DevClass_GenInternal || devClass == DevClass_GenVst || devClass == DevClass_Invalid);};
        bool    isEffect()          {return (devClass == DevClass_EffInternal || devClass == DevClass_EffVst || devClass == DevClass_Invalid);};
        bool    isNative()          {return (devClass == DevClass_GenInternal || devClass == DevClass_EffInternal);};
        bool    isExternal()        {return (devClass == DevClass_GenVst || devClass == DevClass_EffVst || devClass == DevClass_Default);};

        ContextMenu*  createContextMenu();

        void    activateMenuItem(std::string item);

        uint32  getModeColor();
        void    drawDevEntry(Graphics& g, int xo, int ycoord, int w);
        void    drawFileEntry(Graphics& g, int xo, int ycoord, int w);
        void    drawSelf(Graphics & g);
};



FileType GetFileTypeByExtension(std::string extension);


