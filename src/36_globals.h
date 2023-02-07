
#pragma once

#include <windows.h>
#include "winreg.h"
#include "winuser.h"

#include "juce_amalgamated.h"


//namespace M {

class Audio36;
class AuxKeys;
class AlertBox;
class BoolParam;
class Browser;
class BpmBox;
class BrwEntry;
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
class Instrument;
class InstrHighlight;
class InstrPanel;
class InstrVU;
class Knob;
class KeyHandler;
class LanePanel;
class Lane;
class ListBoxx;
class MixChannel;
class MainWinObject;
class MeterBox;
class MainEdit;
class Mixer;
class Note;
class OctaveBox;
class Param;
class Project36;
class Pattern;
class Playhead;
class Pattern;
class ParamObject;
class Renderer;
class ParamBox;
class Parameter;
class ParamToggle;
class ParamRadio;
class ParamSelector;
class Renderer;
class RadioBox;
class Scroller;
class Selection;
class SnapMenu;
class Sample;
class ScanThread;
class SampleNote;
class ScrollTimer;
class Slider36;
class SubWindow;
class TextInput;
class Timeline;
class Trigger;
class Transport;
class TimeScreen;
class TextString;
class TextCursor;
class VstEffect;
class Vst2Plugin;
class Vst2Host;
class VstInstr;
class WinObject;
class WinObject;


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
extern int              MenuButtonsSpacing;
extern int              MenuGroupsSpacing;

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
extern TextCursor*      MCursor;

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


