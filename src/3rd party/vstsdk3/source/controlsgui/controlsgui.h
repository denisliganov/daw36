//------------------------------------------------------------------------
//-
//- Project     : Use different Controls of VSTGUI
//- Filename    : controlsgui.h
//- Created by  : Yvan Grabit
//- Description :
//- 
//- � 1999, Steinberg Soft und Hardware GmbH, All Rights Reserved
//------------------------------------------------------------------------

#ifndef __controlsgui__
#define __controlsgui__

#ifndef __audioeffectx__
#include "audioeffectx.h"
#endif

#include <string.h>

enum
{
	kSliderHTag = 0,
	kSliderVTag,
	kKnobTag,

	kNumParams,

	kOnOffTag,
	kKickTag,
	kMovieButtonTag,
	kAutoAnimationTag,
	kOptionMenuTag,

	kRockerSwitchTag,
	kSwitchHTag,
	kSwitchVTag,

	kSplashTag,
	kMovieBitmapTag,
	kAnimKnobTag,
	kDigitTag,
	kTextEditTag,

	kAbout
};

class Program;

//---------------------------------------------------------
class Controlsgui : public AudioEffectX
{
public:
	Controlsgui (audioMasterCallback audioMaster);
	~Controlsgui ();

	virtual long vendorSpecific (long lArg1, long lArg2, void* ptrArg, float floatArg);
	virtual void process (float **inputs, float **outputs, long sampleFrames);
	virtual void processReplacing (float **inputs, float **outputs, long sampleFrames);
	virtual void setBlockSize (long blockSize);
	virtual void setProgram (long program);
	virtual void setProgramName (char *name);
	virtual void getProgramName (char *name);
	virtual bool getProgramNameIndexed (long category, long index, char* text);
	virtual void setParameter (long index, float value);
	virtual float getParameter (long index);

	virtual void getParameterLabel(long index, char *label);
	virtual void getParameterDisplay(long index, char *text);
	virtual void getParameterName(long index, char *text);

	void mIdle () {masterIdle ();}

	virtual void suspend ();
	virtual void resume ();
	virtual bool keysRequired ();

	virtual bool string2parameter (long index, char* text);

protected:

	Program *programs;
	float fSliderValue;

};

#endif
