

#include <math.h>
#include "rosic/rosic.h"

#include "36_globals.h"
#include "36_pattern.h"
#include "36_sampleinstr.h"
#include "36_utils.h"
#include "36_juce_components.h"
#include "36_instrpanel.h"
#include "36_audio_dev.h"
#include "36_textinput.h"
#include "36_text.h"





String GetPercentString(float value)
{
    String str = String(int(value*100));

    str += "%";

    return str;
}


//
////
///////////
////////////////////////// / / /
// Function ToLowerCase(char* data) - transforms all big letters into small ones
//
void ToLowerCase(char* data)
{
    int ic, len, code;
    len = strlen(data);

    for(ic = 0; ic < len; ic++)
    {
        code = (short)data[ic];
        if(code >= 0x41 && code <= 0x5A)
        {
            code += 0x20;
            data[ic] = code;
        }
    }
}

std::string ToUpperCase(std::string str)
{
    std::string newstr = str;

    for(unsigned int ic = 0; ic < str.size(); ic++)
    {
        uint8 code = (uint8)newstr[ic];

        if(code >= 0x61 && code <= 0x7A)
        {
            code -= 0x20;

            newstr[ic] = code;
        }
    }

    return newstr;
}

//
//////////////////////
////
//
//

void Num2String(long num, char* string)
{
    long    count_digits = 100000;
    long    count_num;
    short   digit;
    char    char_digit;
    char*   p_string = string;
    bool    start = false;

    count_num = num;

    while(count_digits > 0)
    {
        digit = (short)(count_num / count_digits);
        count_num = count_num % count_digits;

        if((digit !=0)||(start == true))
        {
            char_digit = (char)(digit + 48);

           *p_string = char_digit;
            p_string++;

            start = true;
        }
        count_digits /= 10;
    }

    if(start == false)
    {
       *p_string = (char)48; // zero
       *p_string++;
    }

   *p_string = 0;
}

void ParamNum2String(long num, char* string)
{
    long    count_digits = 10;
    long    count_num;
    short   digit;
    char    char_digit;
    char*   p_string = string;

    if(num == 100)  // make dashed string "--" if 100
    {
        *p_string++ = 0x2D;
        *p_string++ = 0x2D;
    }
    else if(num < 100)
    {
        count_num = num;

        while(count_digits > 0)
        {
            digit = (short)(count_num / count_digits);
            count_num = count_num % count_digits;

            if(digit == 0)
            {
               *p_string = '0';
                p_string++;
            }

            if(digit !=0)
            {
                char_digit = (char)(digit + 48);
               *p_string = char_digit;
                p_string++;
            }

            count_digits /= 10;
        }
    }

   *p_string = 0;
}

long ParamString2Num(char* string)
{
    long    count_digits = 1;
    int     digit, nc;
    int     len = strlen(string);
    long    num = 0;
    int     dashcount = 0;

    if(len != 0)
    {
        for(nc = len - 1; nc >= 0; nc--)
        {
            if(string[nc] == 0x2D)
            {
                dashcount++;
            }

            if((string[nc] >= 0x30)&&(string[nc] <= 0x39))
            {
                digit = string[nc] - 0x30;
                num = num + digit * count_digits;
                count_digits *= 10;
            }
            else if((string[nc] == 0x2D)||(string[nc] == 0x25))
            {
                count_digits *= 10;
            }
        }

        if(dashcount == len)
        {
            num = 100; // 100 is a workaround for 2-digit number, likely to be changed to 10^len (len power of 10)
        }
    }

    return(num);
}

float Calc_SlideMultiplier(unsigned long frame_length, int semitones)
{
    float r = CalcFreqRatio(semitones);

    float sm = pow(r, (float)((double)1/(double)frame_length));

    return sm;
}

float NoteToFreq(int note)
{
    return 440.0f*CalcFreqRatio(note - 57);
}

// Returns relative frequency multiplier between two notes

float CalcFreqRatio(int semitones)
{
    int octave = abs(semitones / 12);

    int note_oct = abs(semitones % 12);

    float r = (float)freq_mul_table[note_oct]*(pow((float)2, octave));

    if(semitones >= 0)
    {
        return r;
    }
    else
    {
        return (float)(1.0/r);
    }
}

// Returns multiplier coefficients for left and right channels based on panning in percents.

float Calc_PercentPan(float pan_percent)
{
    return pan_percent/50 - 1;
}

// Returns linearly interpolated value between two values. Return value is y3 for given x3
// Interval is defined by [x1,x2] where x3 is any point on this interval
// y - is amplitude value of the signal being interpolated

INLINE float Interpolate_Line(double x1, float y1, double x2, float y2, double x3)
{
    return (float)(y1 + (x3 - x1)*((y2 - y1)/(x2 - x1)));
}

int RoundDouble(double val)
{
    int ival = abs((int)val);

    if((fabs(val) - ival) >= 0.5)
    {
        ival++;
    }

    if(val > 0)
    {
        return ival;
    }
    else
    {
        return 0 - ival;
    }
}

long RoundDoubleLong(double val)
{
    long ival = c_abs((long)val);

    if((c_abs(val) - ival) >= 0.5)
    {
        ival++;
    }

    if(val > 0)
    {
        return ival;
    }
    else
    {
        return 0 - ival;
    }
}

int RoundFloat(float val)
{
    int ival = abs((int)val);

    if((fabs(val) - ival) >= 0.5)
    {
        ival++;
    }

    if(val > 0)
    {
        return ival;
    }
    else
    {
        return 0 - ival;
    }
}

int RoundFloat1(float val)
{
    int ival = abs((int)val);

    if((fabs(val) - ival) > 0.5)
    {
        ival++;
    }

    if(val > 0)
    {
        return ival;
    }
    else
    {
        return 0 - ival;
    }
}

bool CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2)
{
    if((((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2)) && ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2))) || 
                ((x1 < a1 && x2 > a2) && ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2))) || 
                        ((y1 < b1 && y2 > b2) && ((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2))) || 
                                ((x1 < a1 && x2 > a2)&&(y1 < b1 && y2 > b2)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CheckPlaneCrossing(float x1, float y1, float x2, float y2, float a1, float b1, float a2, float b2)
{
    if((((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2)) && ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2))) || 
                ((x1 < a1 && x2 > a2) && ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2))) || 
                        ((y1 < b1 && y2 > b2) && ((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2))) || 
                                ((x1 < a1 && x2 > a2)&&(y1 < b1 && y2 > b2)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline void PanConstantRule(float pan, float* volL, float* volR)
{
    float pp = (float)PI*(pan + 1)/4;

   *volL = wt_cosine[int(pp/wt_angletoindex)];//cos(pp); // TODO: precalculate
   *volR = wt_sine[int(pp/wt_angletoindex)];//sin(pp); // TODO: precalculate
}

inline void PanLinearRule(float pan, float* volL, float* volR)
{
   *volL = *volR = 1;

    if(pan > 0)
    {
       *volL -= pan;
    }
    else if(pan < 0)
    {
       *volR += pan;
    }
}

inline float GetVolOutput(float val)
{
    if(val < 1)
    {
        //outval = 1 - pow((1 - val), 1.0f/1.5f);
        //outval = pow(10, 2*log10(1 - val));
        //outval = pow(val, 2) + 1 - pow((1 - val), 1.0f/1.5f);
        //outval /= 2;
        //outval = pow(val, 2);
        return (pow(val, 2) + (1 - pow((1 - val), 1.0f/1.5f)))/2;
    }
    else
    {
        return pow(val, 2);
    }
}


// Precalculated wavetables

float           wt_sine[WAVETABLE_SIZE];
float           wt_cosine[WAVETABLE_SIZE];
float           wt_saw[WAVETABLE_SIZE];
float           wt_triangle[WAVETABLE_SIZE];
float           wt_coeff;
float           wt_angletoindex;

void InitWavetables()
{
    // Wavetables for basic oscillators
    wt_coeff = WAVETABLE_SIZE/DEFAULT_SAMPLE_RATE;
    wt_angletoindex = float(PI*2/WAVETABLE_SIZE);

    float trianglestep = 1.0f/(WAVETABLE_SIZE/4);
    float triangleval = 0;

    //float sawstep1 = 2.0f/(WT_SIZE/32);
    //float sawstep2 = 2.0f/(WT_SIZE/32*31);
    float sawstep = 2.0f/(WAVETABLE_SIZE);
    float sawval = 0;

    float anglestep = wt_angletoindex;
    float angle = 0;

    for(int i = 0; i < WAVETABLE_SIZE; i++)
    {
        wt_sine[i] = sin(angle);
        wt_cosine[i] = cos(angle);

        angle += anglestep;

        wt_saw[i] = sawval;

        sawval += sawstep;

        if(sawval >= 1)
        {
            sawval = -1;
        }

        //else if(sawval <= -1)
        //{
        //    sawstep = sawstep1;
        //}

        wt_triangle[i] = triangleval;

        triangleval += trianglestep;

        if(triangleval >= 1)
        {
            trianglestep = -trianglestep;
        }
        else if(triangleval <= -1)
        {
            trianglestep = -trianglestep;
        }
    }
}

