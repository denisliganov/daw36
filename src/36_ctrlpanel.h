
#pragma once

#include "36_globals.h"
#include "36_paramobject.h"




class ControlPanel : public ParamObject
{
friend  MainWinObject;
friend  MainEdit;
friend  Grid;
friend  Transport;
friend  KeyHandler;
friend  Scroller;

public:


            ControlPanel();
            SnapMenu&           getSnapMenu()   { return *snapMenu; } 
            TimeScreen&         getTimeScreen()     { return *timeScreen; } 
            Scroller&           getScroller()       { return *gridScroller; } 
            ContextMenu*        createContextMenu();
            Button36&           getPlayButton()       { return *btPlay; } 

            bool                wavesAreVisible();
            bool                isDarkGrid() { return true; }

protected:

            int                 xButtons;

            Button36*           trkSizeUp;
            Button36*           trkSizeDown;
            Button36*           paletteUp;
            Button36*           paletteDown;
            Button36*           colorChange;

            Button36*           btRender;
            Button36*           btSpectrum;
            Button36*           btMetronome;
            Button36*           btEditMode;
            Button36*           btConfig;
            Button36*           btMixer;
            Button36*           btStop;
            Button36*           btRec;
            Button36*           btHome;
            Button36*           btEnd;
            Button36*           btHelp;
            Button36*           btScrollBack;
            Button36*           btScrollForth;

            Button36*           btZoomIn;
            Button36*           btZoomOut;
            Button36*           btScale1;
            Button36*           btScale2;
            Button36*           btScale3;

            Button36*           btBrowser;
            GroupButton*        btStepMode;
            GroupButton*        btBarMode;
            GroupButton*        btVols;
            GroupButton*        btPans;
            GroupButton*        btKeys;

            Button36*           btWaveMode;
            Button36*           btMouseMode;
            Button36*           btPreviewOn;
            Button36*           btPlay;

            BpmBox*             bpmBox;
            MeterBox*           meterBox;
            OctaveBox*          octaveBox;
            SnapMenu*           snapMenu;
            TimeScreen*         timeScreen;
            Scroller*           gridScroller;

            DropBox*            fileMenu;
            DropBox*            editMenu;
            DropBox*            helpMenu;
            DropBox*            brwMenu;

            void                handleChildEvent(Gobj* obj, InputEvent& ev);
            void                drawSelf(Graphics& g);
            void                remap();
};

