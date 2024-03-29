//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : pluginfactory.h
// Created by  : Steinberg, 01/2004
// Description : Standard Plug-In Factory
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

#ifndef __pluginfactory__
#define __pluginfactory__

#ifndef __ipluginbase__
#include "pluginterfaces/base/ipluginbase.h"
#endif

namespace Steinberg {

//------------------------------------------------------------------------
/** Default Class Factory implementation.
\ingroup sdkBase
\see \ref classFactoryMacros */
//------------------------------------------------------------------------
class CPluginFactory : public IPluginFactory2
{
public:
//------------------------------------------------------------------------
	CPluginFactory (const PFactoryInfo& info);
	virtual ~CPluginFactory ();

//------------------------------------------------------------------------
	/** Registers a plug-in class with classInfo version 1, returns true for success. */
	bool registerClass (const PClassInfo* info,
						FUnknown* (*createFunc)(void*),
						void* context = 0);

	/** Registers a plug-in class with classInfo version 2, returns true for success. */
	bool registerClass (const PClassInfo2* info,
						FUnknown* (*createFunc)(void*),
						void* context = 0);

	/** Check if a class for a given classId is already registered. */
	bool isClassRegistered (const FUID& cid);

//------------------------------------------------------------------------
	DECLARE_FUNKNOWN_METHODS

	//---from IPluginFactory------
	tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info);
	int32 PLUGIN_API countClasses ();
	tresult PLUGIN_API getClassInfo (int32 index, PClassInfo* info);
	tresult PLUGIN_API createInstance (const char* cid, const char* iid, void** obj);

	//---from IPluginFactory2-----
	tresult PLUGIN_API getClassInfo2 (int32 index, PClassInfo2* info);
//------------------------------------------------------------------------
protected:
	struct PClassEntry
	{
	//-----------------------------------
		PClassInfo2 info;
		FUnknown* (*createFunc)(void*);
		void* context;
	//-----------------------------------
	};

	PFactoryInfo factoryInfo;
	PClassEntry* classes;
	int32 classCount;
	int32 maxClassCount;

	bool growClasses ();
};

extern CPluginFactory* gPluginFactory;
} // end namespace

//------------------------------------------------------------------------
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
	#define EXPORT_FACTORY	__attribute__ ((visibility ("default")))
#else
	#define EXPORT_FACTORY
#endif

//------------------------------------------------------------------------
/** \defgroup classFactoryMacros Macros for defining the class factory
\ingroup sdkBase

\b Example - How to use the class factory macros:
\code
BEGIN_FACTORY ("Steinberg Technologies", 
			   "http://www.steinberg.de", 
			   "mailto:info@steinberg.de", 
			   PFactoryInfo::kNoFlags)

DEF_CLASS (INLINE_UID (0x00000000, 0x00000000, 0x00000000, 0x00000000),
			PClassInfo::kManyInstances,    
			"Service",
			"Test Service",
			TestService::newInstance)

END_FACTORY
\endcode 
@{*/
//------------------------------------------------------------------------
#define BEGIN_FACTORY(vendor,url,email,flags) using namespace Steinberg; \
	EXPORT_FACTORY IPluginFactory* PLUGIN_API GetPluginFactory () { \
	if (!gPluginFactory) \
	{	static PFactoryInfo factoryInfo = { vendor,url,email,flags }; \
		gPluginFactory = new CPluginFactory (factoryInfo);

#define DEF_CLASS(cid,cardinality,category,name,createMethod) \
	{ static PClassInfo componentClass = { cid,cardinality,category,name };\
	gPluginFactory->registerClass (&componentClass,createMethod); }

#define DEF_CLASS2(cid,cardinality,category,name,classFlags,subCategories,version,sdkVersion,createMethod) \
	{ static PClassInfo2 componentClass = { cid,cardinality,category,name,classFlags,subCategories,"",version,sdkVersion };\
	gPluginFactory->registerClass (&componentClass,createMethod); }

#define END_FACTORY	} else gPluginFactory->addRef (); \
	return gPluginFactory; }

/** @} */

#endif	// __pluginfactory__
