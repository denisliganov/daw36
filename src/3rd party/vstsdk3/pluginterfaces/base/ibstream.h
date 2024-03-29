//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : ibstream.h
// Created by  : Steinberg, 01/2004
// Modified    : 2008/01/10 10:36:29
// Description : Interface for reading/writing streams
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
//------------------------------------------------------------------------------

#ifndef __ibstream__
#define __ibstream__

#include "funknown.h"

namespace Steinberg {

//------------------------------------------------------------------------
/** Base class for streams.
\ingroup pluginBase
- read / write binary data into/from stream
- get/set stream read-write position (read and write position is the same)
*/
//------------------------------------------------------------------------

class IBStream: public FUnknown
{
public:
	enum IStreamSeekMode
	{
		kIBSeekSet = 0, ///< set absolute seek position
		kIBSeekCur,     ///< set seek position relative to current position
		kIBSeekEnd      ///< set seek position relative to stream end
	};

//------------------------------------------------------------------------
	/** Read binary from stream.
	\param buffer : destination buffer
	\param numBytes : amount of bytes to be read
	\param numBytesRead : result - how many bytes have been read from stream (can be 0 if this is of no interest) */
	virtual tresult PLUGIN_API read  (void* buffer, int32 numBytes, int32* numBytesRead = 0) = 0;
	
	/** Write binary to stream.
	\param buffer : source buffer
	\param numBytes : amount of bytes to write
	\param numBytesWritten : result - how many bytes have been written to stream (can be 0 if this is of no interest) */
	virtual tresult PLUGIN_API write (void* buffer, int32 numBytes, int32* numBytesWritten = 0) = 0;
	
	/** Set stream read-write position. 
	\param pos : new stream position (dependent on mode)
	\param mode : value of enum IStreamSeekMode
	\param result : new seek position (can be 0 if this is of no interest)  */
	virtual tresult PLUGIN_API seek  (int64 pos, int32 mode, int64* result = 0) = 0;
	
	/** Get current stream read-write position. 
	\param pos : Is assigned the current position if function succeeds*/
	virtual tresult PLUGIN_API tell  (int64* pos) = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IBStream, 0xC3BF6EA2, 0x30994752, 0x9B6BF990, 0x1EE33E9B)

}

#endif
