


#include "rosic/rosic.h"

#include "36_utils.h"
#include "36_draw.h"
#include "36_text.h"

#include "Binarysrc/images.h"



Image*  img_curscopy;
Image*  img_cursclone;
Image*  img_cursslide;
Image*  img_cursbrush;
Image*  img_cursselect;

Image*  img[ImageNum];


std::vector<Colour*> colors;

void gGeneratePalette(int max_colors)
{
    uint8 clrStep = int(255/max_colors);
    uint8 clrStart = 255;

    float r = (float)clrStart;
    float g = (float)clrStart;
    float b = (float)clrStart;

    uint32 color = 0xff000000;

    //float clrStepF = clrStep;
    float clrStepF = clrStep * 2.2f;
    float fval = 0.f;

    for(int step = 0; step < max_colors; step++)
    {
        uint8 ri = uint8(r*0.8f);
        uint8 gi = uint8(g*0.95f);
        uint8 bi = uint8(b);

        //colors.push_back(new Colour(ri, gi, bi, (uint8)255));

        colors.push_back(new Colour(fval, .7f, .5f, 1.f));

        r -= clrStepF;
        g -= clrStepF;
        b -= clrStepF;

        clrStepF /= 1.1f;

        if(clrStepF <= 4.f) clrStepF = 4.f;

        fval += 1.f/float(max_colors);
    }
}


void gInitGraphics()
{
    gLoadImages();

    gLoadFonts();

    gGeneratePalette(20);
}

void gLoadImages()
{
    img_curscopy = ImageFileFormat::loadFrom(images::curscopy_png, images::curscopy_pngSize);
    img_cursclone = ImageFileFormat::loadFrom(images::cursclone_png, images::cursclone_pngSize);
    img_cursslide = ImageFileFormat::loadFrom(images::cursslide_png, images::cursslide_pngSize);
    img_cursbrush = ImageFileFormat::loadFrom(images::cursbrush_png, images::cursbrush_pngSize);
    img_cursselect = ImageFileFormat::loadFrom(images::cursselect_png, images::cursselect_pngSize);

    img[ImageSunLogo] = ImageFileFormat::loadFrom(images::sunlogo_png, images::sunlogo_pngSize);
    img[ImageTmDisp] = ImageFileFormat::loadFrom(images::disp_png, images::disp_pngSize);
    img[ImageKnob] = ImageFileFormat::loadFrom(images::knob_large_png, images::knob_large_pngSize);
}


void gSetColorIndex(Graphics& g, int color_index)
{
    g.setColour(*(colors[color_index]));
}

float   globalHue = 0.25f;
float   globalSaturation = 0.35f;
float   brightnessCoeff = 1.f;

void gSetMonoColor(Graphics& g, float clr, float alpha)
{
    g.setColour(Colour(globalHue, globalSaturation, pow(clr, brightnessCoeff), alpha));
}

void gSetColor(Graphics& g, uint32 color)
{
    g.setColour(Colour(color));
}

void gSetColor2(Graphics& g, uint32 color, float brightness, float alpha)
{
    g.setColour(Colour(color).withBrightness(brightness).withAlpha(alpha));
}

void gSetColorHue(Graphics & g, float hue, float brightness, float saturation, float alpha)
{
    g.setColour(Colour(hue, saturation, brightness, alpha));
}

void gFillRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void gFillRectWH(Graphics& g, int x1, int y1, int w, int h)
{
    g.fillRect(x1, y1, w, h);
}

void gDrawRectWH(Graphics& g, int x1, int y1, int w, int h)
{
    g.drawRect(x1, y1, w, h);
}

void gDrawRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void gDrawMonoRect(Graphics& g, int x1, int y1, int x2, int y2, float fillClr, float drawClr)
{
    gSetMonoColor(g, fillClr);

    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    gSetMonoColor(g, drawClr);

    g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void gLine(Graphics& g, float x1, float y1, float x2, float y2)
{
    g.drawLine(x1, y1, x2, y2);
}

void gLine(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.drawLine((float)x1, (float)y1, (float)x2, (float)y2);
}

void gSetColor(Graphics& g, uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
    g.setColour(Colour(red, green, blue).withAlpha(alpha));
}

void gDrawImageRegion(Graphics& g, Image* img, int x, int y, int w, int h)
{
    g.saveState();
    g.reduceClipRegion(x, y, w, h);
    g.drawImageAt(img, x, y);
    g.restoreState();
}

void gCircleDot(Graphics& g, int x1, int y1, float rad)
{
    float r2 = rad + 3.f;

    for(double ang = 0; ang < PI*2; ang += PI/180.f*15.f)
    {
        double xlen;
        double xlen2;

        if(ang >= PI/2)
        {
            xlen = rad*cos(ang);
            xlen2 = r2*cos(ang);
        }
        else
        {
            xlen = -rad*cos(PI - ang);
            xlen2 = r2*cos(ang);
        }

        //    xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));
    
        //gSetColor(((0xff7FDFFF));

        g.setPixel(RoundFloat(float(x1 + xlen)), RoundFloat(y1 - float(rad*sin(ang))));

        //g.drawLine(RoundFloat(x1 + xlen), RoundFloat(y1 - rad*sin(ang)), RoundFloat(x1 + xlen2), RoundFloat(y1 - r2*sin(ang)));
    }
}

void gSetPixel(Graphics& g, int x, int y)
{
    g.setPixel(x, y);
}

void gPanelRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    gSetMonoColor(g, 0.35f);

    gFillRect(g, x1, y1, x2, y2);

    gSetMonoColor(g, 0.4f);

    g.drawHorizontalLine(y1, (float)x1, (float)x2);
}

void gDoubleLine(Graphics& g, float x1, float y1, float x2, float y2, float thick)
{
    g.drawLine(x1, y1, x2, y2, thick);
}

void gTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3, const Colour& fill, const Colour& outline)
{
    Path p;

    p.addTriangle ((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3);

    g.setColour (fill);
    g.fillPath (p);

    g.setColour (outline);
    g.strokePath (p, PathStrokeType (0.3f));
}

void gTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3)
{
    Path p;
    p.addTriangle ((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3);
    g.fillPath (p);
    g.strokePath (p, PathStrokeType (0.3f));
}

void gArc(Graphics& g, int x, int y, int w, int h, float startAngle, float endAngle)
{
    Path p;
    p.addArc(x, y, w, h, startAngle, endAngle);
    g.fillPath (p);
    g.strokePath (p, PathStrokeType (1));
}

void gPie(Graphics& g, int x, int y, int w, int h, float startAngle, float endAngle)
{
    Path p;
    p.addPieSegment(x, y, w, h, startAngle, endAngle, 0);
    g.fillPath (p);
    g.strokePath (p, PathStrokeType (1));
}

void gDrawTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3)
{
    g.drawLine(x3, y3, x1, y1);
    g.drawLine(x1, y1, x2, y2);
    g.drawLine(x2, y2, x3, y3);
}

void gFilledTriangle(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3)
{
    Path trip;

    trip.startNewSubPath((float)x1, (float)y1);
    trip.lineTo((float)x2, (float)y2);
    trip.lineTo((float)x3, (float)y3);
    trip.closeSubPath();

    g.fillPath(trip);
}

void gLineVertical(Graphics& g, int x, int y1, int y2)
{
    if(y2 >= y1)
    {
        g.drawVerticalLine(x, (float)y1, (float)y2);
    }
}

void gLineHorizontal(Graphics& g, int y, int x1, int x2)
{
    if(x2 >= x1)
    {
        g.drawHorizontalLine(y, (float)x1, (float)x2);
    }
}

void gCircle(Graphics& g, int x1, int y1, float rad)
{
    g.drawEllipse((float)x1 - rad/2 + 1, (float)y1 - rad/2 + 1, rad, rad, 1.0f);
}

void gEllipse(Graphics& g, const float x,const float y,const float width,const float height, const float lineThickness)
{
    g.drawEllipse(x, y, width, height, lineThickness);
}

void gEllipseFill(Graphics& g, const float x,const float y,const float width,const float height)
{
    g.fillEllipse(x, y, width, height);
}

void gRound(Graphics& g, int x1, int y1, float rad)
{
    g.fillEllipse((float)x1 - rad/2 + 1, (float)y1 - rad/2 + 1, rad, rad);
}

void gTriLine(Graphics& g, int x, int y, bool left, bool center, bool right)
{
    int delay = 12;

    if(center)
    {
        gSetColor(g, 0xffFFFFFF);
    }
    else
    {
        gSetColor(g, 0xff646464);
    }

    gLine(g, x, y - delay - 1, x, y - 13);

    if(right)
    {
        gSetColor(g, 0xffFFFFFF);
    }
    else
    {
        gSetColor(g, 0xff646464);
    }

    gLine(g, x - 1 + delay, y + delay, x + 9, y + 10);

    if(left)
    {
        gSetColor(g, 0xffFFFFFF);
    }
    else
    {
        gSetColor(g, 0xff646464);
    }

    gLine(g, x - delay, y + delay, x - 10, y + 10);
}



