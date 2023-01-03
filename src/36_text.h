
#pragma once

#include "36_globals.h"

#include <string>

extern juce::Font*      ins;
extern juce::Font*      ari;
extern juce::Font*      ti;
extern juce::Font*      prj;
extern juce::Font*      rox;
extern juce::Font*      bld;
extern juce::Font*      fix;
extern juce::Font*      taho;
extern juce::Font*      ins;



typedef enum FontId
{
    FontArial,
    FontInst,
    FontRox,
    FontSmall,
    FontSmall1,
    FontFix,
    FontBold,
    FontBig,
    FontVis,
    FontProject,
    FontDix
}FontId;

    void    gLoadImages();
    void    gLoadFonts();
    Font*   gGetFontById(FontId id);
    int     gText(Graphics& g, FontId fontId, std::string str, int x, int y);
    void    gTextFit(Graphics& g, FontId fontId, std::string str, int x, int y, int maxwidth);
    int     gGetTextWidth(FontId fontId, std::string str);
    int     gGetTextHeight(FontId fontId);
    Image*  gGetTextImage(Graphics& g, FontId fontId, const char* text, uint32 color);
    void    gZxString(Graphics& g, const char* string, int x, int y);
    void    gDawChar(Graphics& g, const char symbol, int x, int y, int w, int h);
    void    gDString(Graphics& g, const char* string, int x, int y, int sym_w, int sym_h);
    int     gNoteString(Graphics& g, int x, int y, int note, bool relative);


