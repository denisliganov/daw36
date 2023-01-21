
#pragma once

#include <windows.h>
#include "winreg.h"
#include "winuser.h"

#include "juce_amalgamated.h"


//namespace M {

class Event;
class Note;
class Slider36;
class Device36;
class Audio36;
class Project36;
class Button36;

class WinObject;
class Eff;
class Lane;
class Knob;
class Grid;
class Gobj;
class Mixer;
class Pattern;
class Element;
class Trigger;
class Sample;
class ChanVU;
class InstrVU;
class Transport;
class Vst2Host;
class Browser;
class AuxKeys;
class Playhead;
class Pattern;
class Control;
class OctaveBox;
class BrwEntry;
class HelperPanel;
class AlertBox;
class Renderer;
class DropBox;
class BpmBox;
class MeterBox;
class MainEdit;
class SubWindow;
class BoolParam;
class Parameter;
class Envelope;
class Renderer;
class Scroller;
class Selection;
class WinObject;
class VstInstr;
class Timeline;
class SnapMenu;
class TextInput;
class TimeScreen;
class InstrPanel;
class ScanThread;
class SampleNote;
class TextString;
class VstEffect;
class LanePanel;
class Vst2Plugin;
class Instrument;
class MixChannel;
class KeyHandler;
class ScrollTimer;
class DragAndDrop;
class GroupButton;
class ContextMenu;
class MainWinObject;
class ControlPanel;
class EditHistory;
class HistoryAction;
class EnvPoint;
class ParamBox;
class TextCursor;
class InstrHighlight;


struct RenderConfig;


// JUCE's forward defs

class VstComponent;
class JuceAudioDeviceManager;
class JuceAudioCallback;
class JuceMidiInputCallback;
class JuceMidiWrapper;
class SampleWindow;
class RenderWindow;
class ConfigWindow;
class ParamWindow;
class SampleWave;
class AButton;
class RenderComponent;
class JuceVstParamWindow;
class JuceComponent;
class MainWindow;







typedef enum DevClass
{
    DevClass_Default,
    DevClass_GenVst,
    DevClass_EffVst,
    DevClass_GenInternal,
    DevClass_EffInternal,
    DevClass_Invalid
}DevClass;

typedef long tframe;


extern HWND             WinHWND;                //-windep

extern int              CtrlPanelHeight;
extern int              LeftGap;
extern int              MainLineHeight;
extern int              GridScrollWidth;
extern int              MixerHeight;
extern int              MixerTopHeight;
extern int              MixChanWidth;
extern int              MixChannelPadHeight;
extern int              AuxHeight;
extern int              InstrHeight;
extern int              InstrWidth;
extern int              AuxCtrlWidth;
extern int              InstrScrollerWidth;
extern int              InstrControlWidth;
extern int              PanelGap;
extern int              FxPanelMaxWidth;
extern int              BottomPadHeight;
extern int              LineNumWidth;

extern Transport*       MTransp;
extern EditHistory*     MHistory;
extern KeyHandler*      MKeys;
extern Project36        MProject;
extern MainWinObject*   MObject;
extern Audio36*         MAudio;
extern Mixer*           MMixer;
extern LanePanel*       MLanePanel;
extern MainEdit*        MEdit;
extern Grid*            MGrid;
extern InstrPanel*      MInstrPanel;
extern ControlPanel*    MCtrllPanel;
extern InstrPanel*      MInstrPanel;
extern HelperPanel*     MHelperPanel;
extern Browser*         MBrowser;
extern Pattern*         MPattern;
extern TextCursor*      MTextCursor;

extern MixChannel*      SoloMixChannel;
extern Instrument*      SoloInstr;
extern bool             GPlaying;
extern bool             GRecOn;


extern int              BaseNote;
extern int              Octave;
extern char*            WorkDirectory;
extern unsigned int     Config_RenderInterpolation;
extern unsigned int     Config_DefaultInterpolation;



extern MainWindow*                  MWindow;
extern juce::XmlElement*            xmlAudioSettings;
extern JuceAudioDeviceManager*      JAudManager;
extern JuceAudioCallback*           JAudCallBack;
extern JuceMidiInputCallback*       JMidiCallBack;


//}


