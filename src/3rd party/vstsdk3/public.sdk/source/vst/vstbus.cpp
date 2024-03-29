//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstbus.cpp
// Created by  : Steinberg, 03/2008
// Description : VST Bus Implementation
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

#include "vstbus.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// Bus
//------------------------------------------------------------------------
Bus::Bus (const TChar* _name, BusType _busType, int32 _flags)
: busType (_busType)
, name (_name)
, flags (_flags)
, active (false)
{}

//------------------------------------------------------------------------
bool Bus::getInfo (BusInfo& info)
{
	name.copyTo (info.name, USTRINGSIZE (info.name));
	info.busType = busType;
	info.flags = flags;
	return true;
}

//------------------------------------------------------------------------
// EventBus
//------------------------------------------------------------------------
EventBus::EventBus (const TChar* name, BusType busType, int32 flags, int32 channelCount)
: Bus (name, busType, flags)
, channelCount (channelCount)
{}

//------------------------------------------------------------------------
bool EventBus::getInfo (BusInfo& info)
{
	info.channelCount = channelCount;
	return Bus::getInfo (info);
}

//------------------------------------------------------------------------
// AudioBus
//------------------------------------------------------------------------
AudioBus::AudioBus (const TChar* name, BusType busType, int32 flags, SpeakerArrangement arr)
: Bus (name, busType, flags)
, speakerArr (arr)
{}

//------------------------------------------------------------------------
bool AudioBus::getInfo (BusInfo& info)
{
	info.channelCount = SpeakerArr::getChannelCount (speakerArr);
	return Bus::getInfo (info);
}


//------------------------------------------------------------------------
// BusList
//------------------------------------------------------------------------
BusList::BusList (MediaType type, BusDirection dir)
: type (type)
, direction (dir)
{}



} // namespace Vst
} // namespace Steinberg
