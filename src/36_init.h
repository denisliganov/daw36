
#pragma once

#include "36_globals.h"




extern MouseCursor*        cursCopy;
extern MouseCursor*        cursClone;
extern MouseCursor*        cursSlide;
extern MouseCursor*        cursBrush;
extern MouseCursor*        cursSelect;



    void    InitializeAndStartProgram();
    void    ExitProgram();
    void    HandleCommandLine(const String str);
    void    HandleAnotherInstance(const String& command_line);
    void    HandleSystremQuitRequest();
    void    ToggleConfigWindow();



