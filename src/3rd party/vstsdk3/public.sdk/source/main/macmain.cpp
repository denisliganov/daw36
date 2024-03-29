// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : macmain.cpp
// Created by  : Steinberg, 01/2004
// Description : Mac OS X Bundle Entry
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

#ifndef __CF_USE_FRAMEWORK_INCLUDES__
#define __CF_USE_FRAMEWORK_INCLUDES__ 1
#endif

#include <CoreFoundation/CoreFoundation.h>

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
	#define EXPORT	__attribute__ ((visibility ("default")))
#else
	#define EXPORT
#endif

//------------------------------------------------------------------------
CFBundleRef ghInst = 0;
void* moduleHandle = 0;
#define MAX_PATH 2048
char gPath[MAX_PATH] = {0};

//------------------------------------------------------------------------
bool InitModule ();		///< must be provided by plug-in: called when the library is loaded
bool DeinitModule ();	///< must be provided by plug-in: called when the library is unloaded

//------------------------------------------------------------------------
extern "C"
{
EXPORT bool bundleEntry (CFBundleRef);
EXPORT bool bundleExit (void);
}

//------------------------------------------------------------------------
bool bundleEntry (CFBundleRef ref)
{
	if (ref)
	{
		moduleHandle = ghInst = ref;
		CFRetain (ghInst);

		// optain the bundle path
		CFURLRef tempURL = CFBundleCopyBundleURL (ref);
		CFURLGetFileSystemRepresentation (tempURL, true, (UInt8*)gPath, MAX_PATH);
		CFRelease (tempURL);
	}
	return InitModule ();
}

//------------------------------------------------------------------------
bool bundleExit (void)
{
	if (ghInst)
		CFRelease (ghInst);

	return DeinitModule ();
}
