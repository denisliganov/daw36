//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstsinglecomponenteffect.h
// Created by  : Steinberg, 03/2008
// Description : Recombination class of Audio Effect and Edit Controller
//
//-----------------------------------------------------------------------------
// LICENSE
// � 2008, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// This Software Development Kit may not be distributed in parts or its entirety  
// without prior written agreement by Steinberg Media Technologies GmbH. 
// This SDK must not be used to re-engineer or manipulate any technology used  
// in any Steinberg or Third-party application or software module, 
// unless permitted by law.
// Neither the name of the Steinberg Media Technologies nor the names of its
// contributors may be used to endorse or promote products derived from this 
// software without specific prior written permission.
// 
// THIS SDK IS PROVIDED BY STEINBERG MEDIA TECHNOLOGIES GMBH "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL STEINBERG MEDIA TECHNOLOGIES GMBH BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------------

#ifndef __vstsinglecomponenteffect__
#define __vstsinglecomponenteffect__

#include "pluginterfaces/vst/ivstaudioprocessor.h"

// work around for the name clash of IComponent::setState and IEditController::setState
#define setState setEditorState
#define getState getEditorState
#include "pluginterfaces/vst/ivsteditcontroller.h"
#undef setState
#undef getState

#include "vstbus.h"
#include "vstparameters.h"


//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Default implementation for a non-distributable plug-in that combines
processor and edit controller in one component.
\ingroup vstClasses 

Can be used as base class for a VST3 effect implementation in case that the standard way of 
defining two separate components would cause too many implementation difficulties.
- Cubase 4.2 is the first host that supports combined VST3 plug-ins
- <b> Use this class only after giving the standard way of defining two components
serious considerations! </b>*/
//------------------------------------------------------------------------
class SingleComponentEffect: public IComponent,
                         public IAudioProcessor, 
                         public IEditController
						 
{
public:
//------------------------------------------------------------------------
	SingleComponentEffect ();
	~SingleComponentEffect ();

	//---FUnknown------------
	DECLARE_FUNKNOWN_METHODS

	//---IPluginBase---------
	tresult PLUGIN_API initialize (FUnknown* context);
	tresult PLUGIN_API terminate  ();

	//---IComponent-----------------------
	virtual tresult PLUGIN_API getControllerClassId (TUID classId)            {return kNotImplemented;}
	virtual tresult PLUGIN_API setIoMode (IoMode mode)                        {return kNotImplemented;}
	virtual int32 PLUGIN_API getBusCount (MediaType type, BusDirection dir);
	virtual tresult PLUGIN_API getBusInfo (MediaType type, BusDirection dir, 
		int32 index, BusInfo& bus /*out*/);
	virtual tresult PLUGIN_API getRoutingInfo (RoutingInfo& inInfo,           
		RoutingInfo& outInfo /*out*/)                                         {return kNotImplemented;}
	virtual tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, 
		int32 index, TBool state);
	virtual tresult PLUGIN_API setActive (TBool state)                        {return kResultOk;}
	virtual tresult PLUGIN_API setState (IBStream* state)                     {return kNotImplemented;}
	virtual tresult PLUGIN_API getState (IBStream* state)                     {return kNotImplemented;}

	// bus setup methods
	AudioBus* addAudioInput (const TChar* name, SpeakerArrangement arr, 
							 BusType busType = kMain, 
							 int32 flags = BusInfo::kDefaultActive);

	AudioBus* addAudioOutput (const TChar* name, SpeakerArrangement arr, 
							  BusType busType = kMain, 
							  int32 flags = BusInfo::kDefaultActive);

	EventBus* addEventInput (const TChar* name, int32 channels = 16, 
						      BusType busType = kMain, 
							  int32 flags = BusInfo::kDefaultActive);

	EventBus* addEventOutput (const TChar* name, int32 channels = 16, 
							  BusType busType = kMain, 
							  int32 flags = BusInfo::kDefaultActive);

	tresult removeAudioBusses ();
	tresult removeEventBusses ();
	tresult removeAllBusses ();


	//---IAudioProcessor -------------------
	virtual tresult PLUGIN_API setBusArrangements (
		SpeakerArrangement* inputs, int32 numIns, 
		SpeakerArrangement* outputs, int32 numOuts);
	virtual tresult PLUGIN_API getBusArrangement (
		BusDirection dir, int32 index, 
		SpeakerArrangement& arr);
	virtual tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize);
	virtual uint32  PLUGIN_API getLatencySamples ()	                       {return 0; } 
	virtual tresult PLUGIN_API setupProcessing (ProcessSetup& setup);
	virtual tresult PLUGIN_API setProcessing (TBool state)                 {return kNotImplemented;}
	virtual tresult PLUGIN_API process (ProcessData& data)                 {return kNotImplemented;}
	virtual uint32 PLUGIN_API getTailSamples ()                            {return 0;}  


	//---IEditController-------
	tresult PLUGIN_API setComponentState (IBStream* state)                 {return kNotImplemented;}
	tresult PLUGIN_API setEditorState (IBStream* state)	                   {return kNotImplemented;}
	tresult PLUGIN_API getEditorState (IBStream* state)                    {return kNotImplemented;}
	int32 PLUGIN_API getParameterCount ();
	tresult PLUGIN_API getParameterInfo (int32 paramIndex, ParameterInfo& info);
	tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string);
	tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized);
	ParamValue PLUGIN_API normalizedParamToPlain (ParamID tag, ParamValue valueNormalized);
	ParamValue PLUGIN_API plainParamToNormalized (ParamID tag, ParamValue plainValue);
	ParamValue PLUGIN_API getParamNormalized (ParamID tag);
	tresult PLUGIN_API setParamNormalized (ParamID tag, ParamValue value);
	tresult PLUGIN_API setComponentHandler (IComponentHandler* handler);
	IPlugView* PLUGIN_API createView (const char* name)                    {return 0;}

	//---Internal Methods-------
	virtual tresult beginEdit (ParamID tag);
	virtual tresult performEdit (ParamID tag, ParamValue valueNormalized);
	virtual tresult endEdit (ParamID tag);
	
//------------------------------------------------------------------------
protected:
	BusList* getBusList (MediaType type, BusDirection dir);

	FUnknown* hostContext;
	IComponentHandler* componentHandler;

	ParameterContainer parameters;
	ProcessSetup processSetup;

	BusList audioInputs;
	BusList audioOutputs;
	BusList eventInputs;
	BusList eventOutputs;
};


//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vstSingleComponentEffect__
