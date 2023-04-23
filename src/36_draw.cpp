


#include "rosic/rosic.h"

#include "36_utils.h"
#include "36_draw.h"
#include "36_text.h"
#include "36_config.h"

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
float   globalSaturation = 0.f;
float   brightnessCoeff = 1.f;

float   currHue = 0.25f;
float   currSat = 0.f;

void gResetColorSettings()
{
    currHue = globalHue;
    currSat = globalSaturation;
}

void gSetColorSettings(float hue, float sat)
{
    currHue = hue;
    currSat = sat;
}

void gSetMonoColor(Graphics& g, float clr, float alpha)
{
    g.setColour(Colour(currHue, currSat, pow(clr, brightnessCoeff), alpha));
}

void gSetColor(Graphics& g, uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
    g.setColour(Colour(red, green, blue).withAlpha(alpha));
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

void gGradRect(Graphics& g, uint32 clr, int x1, int y1, int x2, int y2)
{
    uint32 colorHL = clr;
    uint32 hlDecr = 0x80000000;
    
    for (int c = 0; c < 4; c++)
    {
        gSetColor(g, (uint32)colorHL);

        gDrawRect(g, x1 + c, y1 + c, x2 - c, y2 - c);

        colorHL -= hlDecr;
        hlDecr /=2;
    }
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
    g.strokePath (p, PathStrokeType (0));
}

void gPie(Graphics& g, int x, int y, int w, int h, float startAngle, float endAngle)
{
    Path p;
    p.addPieSegment(x, y, w, h, startAngle, endAngle, 0);
    g.fillPath (p);
    g.strokePath (p, PathStrokeType (0));
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

static void createRoundedPath (Path& p,
                               const float x, const float y,
                               const float w, const float h,
                               const float cs,
                               const bool curveTopLeft, const bool curveTopRight,
                               const bool curveBottomLeft, const bool curveBottomRight) throw()
{
    const float cs2 = 2.0f * cs;

    if (curveTopLeft)
    {
        p.startNewSubPath (x, y + cs);
        p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
    }
    else
    {
        p.startNewSubPath (x, y);
    }

    if (curveTopRight)
    {
        p.lineTo (x + w - cs, y);
        p.addArc (x + w - cs2, y, cs2, cs2, 0.0f, float_Pi * 0.5f);
    }
    else
    {
        p.lineTo (x + w, y);
    }

    if (curveBottomRight)
    {
        p.lineTo (x + w, y + h - cs);
        p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
    }
    else
    {
        p.lineTo (x + w, y + h);
    }

    if (curveBottomLeft)
    {
        p.lineTo (x + cs, y + h);
        p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
    }
    else
    {
        p.lineTo (x, y + h);
    }

    p.closeSubPath();
}


void drawGlassRect1 (Graphics& g,
                            const float x, const float y,
                            const float width, const float height,
                            const Colour& colour,
                            const float outlineThickness,
                            const float cornerSize,
                            const bool flatOnLeft,
                            const bool flatOnRight,
                            const bool flatOnTop,
                            const bool flatOnBottom) throw()
{
    if (width <= outlineThickness || height <= outlineThickness)
        return;

    const int intX = (int) x;
    const int intY = (int) y;
    const int intW = (int) width;
    const int intH = (int) height;

    const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
    const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
    const int intEdge = (int) edgeBlurRadius;

    Path outline;

    createRoundedPath (outline, x, y, width, height, cs,
                        ! (flatOnLeft || flatOnTop),
                        ! (flatOnRight || flatOnTop),
                        ! (flatOnLeft || flatOnBottom),
                        ! (flatOnRight || flatOnBottom));

    {
        ColourGradient cg (colour, 0, y, colour, 0, y + height, false);

        //cg.addColour (0.03, colour.withMultipliedAlpha (0.9f));
        cg.addColour (0.4, colour);
        cg.addColour (0.9, colour.withMultipliedAlpha (0.6f));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif	
        g.fillPath (outline);
    }

    //return;

    ColourGradient cg (Colours::transparentBlack, x + edgeBlurRadius, y + height * 0.5f, colour.darker (0.4f), x, y + height * 0.5f, true);

    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.5f) / edgeBlurRadius), Colours::transparentBlack);
    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.25f) / edgeBlurRadius), colour.darker (0.4f).withMultipliedAlpha (0.3f));

    if (! (flatOnLeft || flatOnTop || flatOnBottom))
    {
        g.saveState();

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.reduceClipRegion (intX, intY, intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }


    if (! (flatOnRight || flatOnTop || flatOnBottom))
    {
        cg.x1 = x + width - edgeBlurRadius;
        cg.x2 = x + width;

        g.saveState();
#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.reduceClipRegion (intX + intW - intEdge, intY, 2 + intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

    //return;

    // White upper shine
    {
        const float leftIndent = flatOnLeft ? 0.0f : cs * 0.4f;
        const float rightIndent = flatOnRight ? 0.0f : cs * 0.4f;

        Path highlight;
        createRoundedPath (highlight,
                           x + leftIndent,
                           y + cs * 0.1f,
                           width - (leftIndent + rightIndent),
                           4, cs * 0.3f,
                           ! (flatOnLeft || flatOnTop),
                           ! (flatOnRight || flatOnTop),
                           ! (flatOnLeft || flatOnBottom),
                           ! (flatOnRight || flatOnBottom));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(colour.brighter (1.0f), 0, y , Colours::transparentWhite, 0, y + 3, false));
#else
        g.setGradientFill (ColourGradient(colour.brighter (1.0f), 0, y, Colours::transparentWhite, 0, y + 3, false));
#endif
        g.fillPath (highlight);
    }

    g.setColour (colour.darker().withMultipliedAlpha (1.5f));
    g.strokePath (outline, PathStrokeType (outlineThickness));

    return;
}


void drawGlassRect (Graphics& g,
                            const float x, const float y,
                            const float width, const float height,
                            const Colour& colour,
                            const float outlineThickness,
                            const float cornerSize,
                            const bool flatOnLeft,
                            const bool flatOnRight,
                            const bool flatOnTop,
                            const bool flatOnBottom) throw()
{
    if (width <= outlineThickness || height <= outlineThickness)
        return;

    const int intX = (int) x;
    const int intY = (int) y;
    const int intW = (int) width;
    const int intH = (int) height;

    const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
    const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
    const int intEdge = (int) edgeBlurRadius;

    Path outline;

    createRoundedPath (outline, x, y, width, height, cs,
                        ! (flatOnLeft || flatOnTop),
                        ! (flatOnRight || flatOnTop),
                        ! (flatOnLeft || flatOnBottom),
                        ! (flatOnRight || flatOnBottom));

    {
        ColourGradient cg (colour, 0, y, colour, 0, y + height, false);

        //cg.addColour (0.03, colour.withMultipliedAlpha (0.9f));
        cg.addColour (0.4, colour);
        cg.addColour (0.94, colour.withMultipliedAlpha (0.6f));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif	
        g.fillPath (outline);
    }

    //return;

    ColourGradient cg (Colours::transparentBlack, x + edgeBlurRadius, y + height * 0.5f, colour.darker (0.4f), x, y + height * 0.5f, true);

    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.5f) / edgeBlurRadius), Colours::transparentBlack);
    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.25f) / edgeBlurRadius), colour.darker (0.4f).withMultipliedAlpha (0.3f));

    if (! (flatOnLeft || flatOnTop || flatOnBottom))
    {
        g.saveState();

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.reduceClipRegion (intX, intY, intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }


    if (! (flatOnRight || flatOnTop || flatOnBottom))
    {
        cg.x1 = x + width - edgeBlurRadius;
        cg.x2 = x + width;

        g.saveState();
#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.reduceClipRegion (intX + intW - intEdge, intY, 2 + intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

    //return;

    // White upper shine
    {
        const float leftIndent = flatOnLeft ? 0.0f : cs * 0.4f;
        const float rightIndent = flatOnRight ? 0.0f : cs * 0.4f;

        Path highlight;
        createRoundedPath (highlight,
                           x + leftIndent,
                           y + cs * 0.1f,
                           width - (leftIndent + rightIndent),
                           height * 0.3f, cs * 0.3f,
                           ! (flatOnLeft || flatOnTop),
                           ! (flatOnRight || flatOnTop),
                           ! (flatOnLeft || flatOnBottom),
                           ! (flatOnRight || flatOnBottom));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(colour.brighter (2.0f), 0, y + height * 0.06f, Colours::transparentWhite, 0, y + height * 0.3f, false));
#else
        g.setGradientFill (ColourGradient(colour.brighter (2.0f), 0, y + height * 0.06f, Colours::transparentWhite, 0, y + height * 0.3f, false));
#endif
        g.fillPath (highlight);
    }

    g.setColour (colour.darker().withMultipliedAlpha (1.5f));
    g.strokePath (outline, PathStrokeType (outlineThickness));

    return;
}

void drawGlassRound (Graphics& g,
                           const float x, const float y,
                           const float diameter,
                           const Colour& colour,
                           const float outlineThickness) throw()
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.addEllipse (x, y, diameter, diameter);

    if (1)
    {
    /*
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.4, Colours::white.overlaidWith (colour));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif*/

        g.setColour(colour);
        g.fillPath (p);
    }

    if (0)
    {

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(Colours::white, 0, y + diameter * 0.06f, Colours::transparentWhite, 0, y + diameter * 0.3f, false));
#else
        g.setGradientFill(ColourGradient(Colours::white, 0, y + diameter * 0.06f,
                          Colours::transparentWhite, 0, y + diameter * 0.3f, false));
#endif

        g.fillEllipse (x + diameter * 0.2f, y + diameter * 0.05f, diameter * 0.6f, diameter * 0.4f);
    }

    {
        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,

                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x, y + diameter * 0.5f, 
                           true);

        /*
        ColourGradient cg (Colours::red,
                           x + diameter * 0.5f, y + diameter * 0.5f,

                           Colours::green,
                           x, y + diameter * 0.5f, 
                           true);
        */

        cg.addColour (0.7, Colours::transparentBlack);
        cg.addColour (0.8, Colours::white.withAlpha (0.1f * outlineThickness));

        //cg.addColour (0.7, Colours::transparentBlack);
        //cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));

#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.fillPath (p);
    }

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.drawEllipse (x, y, diameter, diameter, outlineThickness);
}


void paintButton (Graphics& g, int x, int y, int w, int h, bool isMouseOverButton, bool isButtonDown)
{
    g.setColour(Colour(21, 25, 26));
    g.fillRect(x, y, w, h);

    g.setColour(Colour(75, 101, 90));
    //g.setColour(Colour(74, 88, 91));
    g.drawRect(x+1, y+1, w - 2, h - 2);

    g.setColour(Colour(110, 160, 150));
    //g.setColour(Colour(112, 124, 129));
    g.fillRect(x+2, y+2, w - 4, h - 4);

    if(!isButtonDown)
    {
        Colour clr = Colour(200, 220, 230);

        g.setColour(clr);
        //g.setColour(Colour(233, 233, 233));
        g.drawHorizontalLine(y+2, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.35f));
        g.drawHorizontalLine(y+3, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.25f));
        g.drawHorizontalLine(y+4, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.2f));
        g.drawHorizontalLine(y+5, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.15f));
        g.drawHorizontalLine(y+6, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.1f));
        g.drawHorizontalLine(y+7, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.08f));
        g.drawHorizontalLine(y+8, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.06f));
        g.drawHorizontalLine(y+9, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.05f));
        g.drawHorizontalLine(y+10, x+2.f, x+float(w - 2));
        g.setColour(clr.withAlpha(0.04f));
        g.drawHorizontalLine(y+11, x+2.f, x+float(w - 2));
        
        g.setColour(Colour(143, 159, 165));
        //g.setColour(Colour(137, 137, 137));
        g.drawHorizontalLine(y + h - 3, x + 2.f, x + float(w - 2));
    }
    else
    {
    }
}



