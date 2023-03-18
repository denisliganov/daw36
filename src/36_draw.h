
#pragma once


#include "36_globals.h"





extern Image*  img_curscopy;
extern Image*  img_cursclone;
extern Image*  img_cursslide;
extern Image*  img_cursbrush;
extern Image*  img_cursselect;

extern float globalHue;
extern float globalSaturation;
extern float brightnessCoeff;


typedef enum ImageId
{
    ImageSunLogo,
    ImageTmDisp,
    ImageKnob,
    ImageNum
}ImageId;


struct Point36 
{
    float x; 
    float y; 
};


        void    gArc(Graphics& g, int x, int y, int w, int h, float startAngle, float endAngle);
        void    gCircle(Graphics& g, int x1, int y1, float rad);
        void    gCircle(Graphics& g, int x1, int y1, float rad);
        void    gDoubleLine(Graphics& g, float x1, float y1, float x2, float y2, float thick);
        void    gDrawRectWH(Graphics& g, int x1, int y1, int w, int h);
        void    gDrawImageRegion(Graphics& g, Image* img, int x, int y, int w, int h);
        void    gDrawMonoRect(Graphics& g, int x1, int y1, int x2, int y2, float fillClr, float drawClr);
        void    gDrawTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3);
        void    gDrawRect(Graphics& g, int x1, int y1, int x2, int y2);
        void    gEllipse(Graphics& g, const float x, const float y, const float width, const float height, const float lineThickness);
        void    gEllipseFill(Graphics& g, const float x,const float y,const float width,const float height);
        void    gFillRect(Graphics& g, int x1, int y1, int x2, int y2);
        void    gFillRectWH(Graphics& g, int x1, int y1, int w, int h);
        void    gFilledTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3);
        void    gGeneratePalette(int max_colors);
        void    gGradRect(Graphics& g, uint32 clr, int x1, int y1, int x2, int y2);
        void    gInitGraphics();
        void    gSetColor(Graphics& g, uint32 color);
        void    gSetColor(Graphics& g, uint8 red, uint8 green, uint8 blue, uint8 alpha = 255);
        void    gSetColor2(Graphics& g, uint32 color, float brightness, float alpha = 1.f);
        void    gSetColorHue(Graphics & g, float hue, float brightness=1, float saturation=1, float alpha=1);
        void    gLine(Graphics& g, int x1, int y1, int x2, int y2);
        void    gLine(Graphics& g, float x1, float y1, float x2, float y2);
        void    gLineHorizontal(Graphics& g, int y, int x1, int x2);
        void    gLineVertical(Graphics& g, int x, int y1, int y2);
        void    gPie(Graphics& g, int x, int y, int w, int h, float startAngle, float endAngle);
        void    gLine(Graphics& g, float x1, float y1, float x2, float y2);
        void    gPanelRect(Graphics& g, int x1, int y1, int x2, int y2);
        void    gRound(Graphics& g, int x1, int y1, float rad);
        void    gSetPixel(Graphics& g, int x, int y);
        void    gSetColorIndex(Graphics& g, int color_index);
        void    gSetMonoColor(Graphics& g, float clr, float alpha = 1.f);
        void    gTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3, const Colour& fill, const Colour& outline);
        void    gTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3);
        void    gTriLine(Graphics& g, int x, int y, bool left, bool center, bool right);

        void    paintButton (Graphics& g, int x, int y, int w, int h, bool isMouseOverButton, bool isButtonDown);
        void    drawGlassRound (Graphics& g,
                                   const float x, const float y,
                                   const float diameter,
                                   const Colour& colour,
                                   const float outlineThickness);
        void    drawGlassRect (Graphics& g,
                                    const float x, const float y,
                                    const float width, const float height,
                                    const Colour& colour,
                                    const float outlineThickness,
                                    const float cornerSize,
                                    const bool flatOnLeft,
                                    const bool flatOnRight,
                                    const bool flatOnTop,
                                    const bool flatOnBottom);
        void    drawGlassRect1 (Graphics& g,
                                    const float x, const float y,
                                    const float width, const float height,
                                    const Colour& colour,
                                    const float outlineThickness,
                                    const float cornerSize,
                                    const bool flatOnLeft,
                                    const bool flatOnRight,
                                    const bool flatOnTop,
                                    const bool flatOnBottom);

