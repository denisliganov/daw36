//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstaudioeffect.cpp
// Created by  : Steinberg, 04/2005
// Description : Basic Audio Effect Implementation
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
//-----------------------------------------------------------------------------

#include "vstaudioeffect.h"

namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
// AudioEffect
//------------------------------------------------------------------------
AudioEffect::AudioEffect ()
{
	processSetup.maxSamplesPerBlock = 1024;
	processSetup.processMode = Vst::kRealtime;
	processSetup.sampleRate = 44100.0;
	processSetup.symbolicSampleSize = Vst::kSample32;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, IAudioProcessor::iid, IAudioProcessor)
	return Component::queryInterface (iid, obj);
}

//------------------------------------------------------------------------
AudioBus* AudioEffect::addAudioInput (const TChar* name, SpeakerArrangement arr, 
									  BusType busType, int32 flags)
{
	AudioBus* newBus = new AudioBus (name, busType, flags, arr);
	audioInputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
AudioBus* AudioEffect::addAudioOutput (const TChar* name, SpeakerArrangement arr, 
									   BusType busType, int32 flags)
{
	AudioBus* newBus = new AudioBus (name, busType, flags, arr);
	audioOutputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
AudioBus* AudioEffect::getAudioInput (int32 index)
{
	AudioBus* bus = static_cast<AudioBus*> (audioInputs.at (index));
	return bus;
}

//------------------------------------------------------------------------
AudioBus* AudioEffect::getAudioOutput (int32 index)
{
	AudioBus* bus = static_cast<AudioBus*> (audioOutputs.at (index));
	return bus;
}

//------------------------------------------------------------------------
EventBus* AudioEffect::addEventInput (const TChar* name, int32 channels, 
									  BusType busType, int32 flags)
{
	EventBus* newBus = new EventBus (name, busType, flags, channels);
	eventInputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
EventBus* AudioEffect::addEventOutput (const TChar* name, int32 channels, 
									   BusType busType, int32 flags)
{
	EventBus* newBus = new EventBus (name, busType, flags, channels);
	eventOutputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
EventBus* AudioEffect::getEventInput (int32 index)
{
	EventBus* bus = static_cast<EventBus*> (eventInputs.at (index));
	return bus;
}

//------------------------------------------------------------------------
EventBus* AudioEffect::getEventOutput (int32 index)
{
	EventBus* bus = static_cast<EventBus*> (eventOutputs.at (index));
	return bus;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::setBusArrangements (SpeakerArrangement* inputs, int32 numIns, 
													SpeakerArrangement* outputs, int32 numOuts)
{
	int32 counter = 0;
	LIST_FOREACH (AudioBus, bus, audioInputs)
		if (counter < numIns)
			bus->setArrangement (inputs[counter]);
		counter++;
	LIST_ENDFOR

	counter = 0;
	LIST_FOREACH (AudioBus, bus, audioOutputs)
		if (counter < numOuts)
			bus->setArrangement (outputs[counter]);
		counter++;
	LIST_ENDFOR

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::getBusArrangement (BusDirection dir, int32 busIndex, SpeakerArrangement& arr)
{
	BusList* busList = getBusList (kAudio, dir);
	AudioBus* audioBus = busList ? static_cast<AudioBus*> (busList->at (busIndex)) : 0;
	if (audioBus)
	{
		arr = audioBus->getArrangement ();
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::setupProcessing (ProcessSetup& newSetup)
{
	if (newSetup.symbolicSampleSize != kSample32)
		return kResultFalse;

	processSetup = newSetup;
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::setProcessing (TBool state)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::canProcessSampleSize (int32 symbolicSampleSize)
{
	return symbolicSampleSize == kSample32 ? kResultTrue : kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AudioEffect::process (ProcessData& data)
{
	return kNotImplemented;
}

} // namespace Vst
} // namespace Steinberg
