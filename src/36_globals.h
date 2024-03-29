
#pragma once

#include <windows.h>
#include "winreg.h"
#include "winuser.h"

#include "juce_amalgamated.h"




class Audio36;
class AuxKeys;
class AlertBox;
class Browser;
class BpmBox;
class BrowserList;
class BrwListEntry;
class Button36;
class ChanVU;
class Control;
class ContextMenu;
class ControlPanel;
class DropBox;
class DragAndDrop;
class Device36;
class EditHistory;
class Envelope;
class EnvPoint;
class Element;
class Eff;
class Event;
class GroupButton;
class Grid;
class Gobj;
class HelperPanel;
class HistoryAction;
class InfoWin;
class Instr;
class InstrHighlight;
class InstrPanel;
class InstrVU;
class Knob;
class KeyHandler;
class ListBoxx;
class ListBoxS;
class MixChannel;
class MainWinObject;
class MeterBox;
class MainEdit;
class Note;
class OctaveBox;
class ParamObject;
class Parameter;
class Project36;
class Pattern;
class Playhead;
class Pattern;
class Renderer;
class SelectorBox;
class Scroller;
class Selection;
class SnapMenu;
class Sample;
class ScanThread;
class SampleNote;
class ScrollTimer;
class SubWindow;
class TextInput;
class Timeline;
class Trigger;
class Transport;
class TimeScreen;
class TextString;
class TextCursor;
class Vst2Plugin;
class Vst2Host;


struct RenderConfig;


// JUCE's forward defs

class VstComponent;
class JuceAudioDeviceManager;
class JuceAudioCallback;
class JuceMidiInputCallback;
class JuceMidiWrapper;
class SampleWindow;
class RenderWindow;

class SampleWave;
class AButton;
class RenderComponent;
class JuceVstParamWindow;
class JuceComponent;
class MainWindow;




typedef long tframe;


extern HWND             WinHWND;

extern int              AuxCtrlWidth;
extern int              AuxHeight;
extern int              BottomPadHeight;
extern int              CtrlPanelHeight;
extern int              FxPanelMaxWidth;
extern int              FxPanelScrollerWidth;
extern int              FxPanelBottomHeight;
extern int              FxMaxEffWidth;
extern int              GridScrollWidth;
extern int              InstrHeight;
extern int              InstrWidth;
extern int              InstrScrollerWidth;
extern int              InstrControlWidth;
extern int              LeftGap;
extern int              LineNumWidth;
extern int              MenuButtonsSpacing;
extern int              MenuGroupsSpacing;
extern int              MainLineHeight;
extern int              MixerHeight;
extern int              MixerTopHeight;
extern int              MixChanWidth;
extern int              MixChannelPadHeight;
extern int              PanelGap;

extern Transport*       MTransp;
extern EditHistory*     MHistory;
extern KeyHandler*      MKeys;
extern Project36        MProject;
extern MainWinObject*   MObject;
extern Audio36*         MAudio;
extern MainEdit*        MEdit;
extern Grid*            MGrid;
extern InstrPanel*      MInstrPanel;
extern ControlPanel*    MCtrllPanel;
extern InstrPanel*      MInstrPanel;
extern HelperPanel*     MHelperPanel;
extern Browser*         MBrowser;
extern Pattern*         MPattern;
extern TextCursor*      MCursor;
extern DragAndDrop*     MDragDrop;


extern Instr*           SoloInstr;
extern bool             GPlaying;
extern bool             GRecOn;
extern bool             MixViewSingle;

extern bool             InitComplete;

extern int              BaseNote;
extern int              Octave;
extern char*            WorkDirectory;
extern unsigned int     Config_RenderInterpolation;
extern unsigned int     Config_DefaultInterpolation;



extern MainWindow*                  MWindow;
extern juce::XmlElement*            xmlAudioSettings;
extern JuceAudioDeviceManager*      JAudioManager;
extern JuceAudioCallback*           JAudioCallBack;
extern JuceMidiInputCallback*       JMidiCallBack;


//}


