
#pragma once


#include "36_globals.h"


#define c_abs(x) ((x) < 0 ? -(x) : (x))


//
//
extern float            wt_sine[];
extern float            wt_cosine[];
extern float            wt_saw[];
extern float            wt_triangle[];
extern float            wt_coeff;
extern float            wt_angletoindex;

//
// Frequency multipliers 
const double freq_mul_table[13] = {1, 1.059463094, 1.122462048, 1.189207115, 1.25992105, 1.334839854, 1.414213562, 1.498307077, 1.587401052, 1.68179283, 1.781797436, 1.887748625, 2};
const char note_table[][12] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};



extern float        GetVolOutput(float val);
extern void         PanLinearRule(float pan, float* volL, float* volR);
extern inline float Interpolate_Line(double x1, float y1, double x2, float y2, double x3);

void        InitWavetables();
float       NoteToFreq(int note);
void        ToLowerCase(char* data);
std::string ToUpperCase(std::string str);
void        Num2String(long num, char* string);
void        ParamNum2String(long num, char* string);
long        ParamString2Num(char* string);
float       Calc_SlideMultiplier(unsigned long frame_length, int semitones);
float       Calc_PercentPan(float pan_percent);
int         RoundDouble(double val);
long        RoundDoubleLong(double val);
int         RoundFloat1(float val);
bool        CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2);
bool        CheckPlaneCrossing(float x1, float y1, float x2, float y2, float a1, float b1, float a2, float b2);
void        PanConstantRule(float pan, float* volL, float* volR);
void        GetPatternNameImage(Graphics& g, Pattern* pt);
String      GetPercentString(float value);
int         RoundFloat(float val);
float       CalcFreqRatio(int semitones);
void        Num2String(long num, char* string);


