//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : pluginview.cpp
// Created by  : Steinberg, 01/2004
// Description : Plug-In View Implementation
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

#include "pluginview.h"

namespace Steinberg {

//------------------------------------------------------------------------
//  CPluginView implementation
//------------------------------------------------------------------------
CPluginView::CPluginView (const ViewRect* _rect)
: rect (0, 0, 0, 0)
, systemWindow (0)
, plugFrame (0)
{
	FUNKNOWN_CTOR
	if (_rect)
		rect = *_rect;
}

//------------------------------------------------------------------------
CPluginView::~CPluginView ()
{
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_REFCOUNT (CPluginView)

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, Steinberg::FUnknown::iid, IPlugView)
	QUERY_INTERFACE (iid, obj, Steinberg::IPlugView::iid,  IPlugView)
	*obj = 0;
	return kNoInterface;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::isPlatformTypeSupported (FIDString type)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::attached (void* parent, FIDString type)
{
	systemWindow = parent;
	
	attachedToParent ();
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::removed ()
{
	systemWindow = 0;

	removedFromParent ();
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::onSize (ViewRect* newSize)
{
	if (newSize)
		rect = *newSize;
	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginView::getSize (ViewRect* size)
{
	if (size)
	{
		*size = rect;
		return kResultTrue;
	}
	return kInvalidArgument;
}

} // end of namespace
