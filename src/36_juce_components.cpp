

#include "Binarysrc/images.h"


#include "36_globals.h"
#include "36_objects.h"
#include "36_keyboard.h"
#include "36_grid.h"
#include "36_vu.h"
#include "36_pattern.h"
#include "36_paramnum.h"
#include "36_audio_dev.h"
#include "36_utils.h"
#include "36_renderer.h"
#include "36_system.h"
#include "36_edit.h"
#include "36_sampleinstr.h"
#include "36_instrpanel.h"
#include "36_ctrlpanel.h"
#include "36_menu.h"
#include "36_audiomanager.h"
#include "36_text.h"


#include "36_juce_components.h"
#include "36_juce_windows.h"




int getSliderThumbRadius (ASlider& slider)
{
    return jmin (7, slider.getHeight() / 2, slider.getWidth() / 2) + 2;
}

static double smallestAngleBetween (double a1, double a2)
{
    return jmin (fabs (a1 - a2), fabs (a1 + double_Pi * 2.0 - a2), fabs (a2 + double_Pi * 2.0 - a1));
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

void drawGlassLozenge (Graphics& g,
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
        ColourGradient cg (colour.darker (0.2f), 0, y,
                           colour.darker (0.2f), 0, y + height, false);

        cg.addColour (0.03, colour.withMultipliedAlpha (0.3f));
        cg.addColour (0.4, colour);
        cg.addColour (0.97, colour.withMultipliedAlpha (0.3f));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
		g.setGradientFill(cg);
#endif	
        g.fillPath (outline);
    }

    ColourGradient cg (Colours::transparentBlack, x + edgeBlurRadius, y + height * 0.5f,
                       colour.darker (0.2f), x, y + height * 0.5f, true);

    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.5f) / edgeBlurRadius), Colours::transparentBlack);
    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.25f) / edgeBlurRadius), colour.darker (0.2f).withMultipliedAlpha (0.3f));

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

    {
        const float leftIndent = flatOnLeft ? 0.0f : cs * 0.4f;
        const float rightIndent = flatOnRight ? 0.0f : cs * 0.4f;

        Path highlight;
        createRoundedPath (highlight,
                           x + leftIndent,
                           y + cs * 0.1f,
                           width - (leftIndent + rightIndent),
                           height * 0.4f, cs * 0.4f,
                           ! (flatOnLeft || flatOnTop),
                           ! (flatOnRight || flatOnTop),
                           ! (flatOnLeft || flatOnBottom),
                           ! (flatOnRight || flatOnBottom));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(colour.brighter (10.0f), 0, y + height * 0.06f, Colours::transparentWhite, 0, y + height * 0.4f, false));
#else
		g.setGradientFill (ColourGradient(colour.brighter (10.0f), 0, y + height * 0.06f,
                          Colours::transparentWhite, 0, y + height * 0.4f, false));
#endif
		g.fillPath (highlight);
    }

    g.setColour (colour.darker().withMultipliedAlpha (1.5f));
    g.strokePath (outline, PathStrokeType (outlineThickness));
}

void drawGlassSphere (Graphics& g,
                           const float x, const float y,
                           const float diameter,
                           const Colour& colour,
                           const float outlineThickness) throw()
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.addEllipse (x, y, diameter, diameter);

    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.4, Colours::white.overlaidWith (colour));
#ifdef USE_OLD_JUCE
        g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.fillPath (p);
    }

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
                           x, y + diameter * 0.5f, true);

        cg.addColour (0.7, Colours::transparentBlack);
        cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));

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

void drawGlassSphere1(Graphics& g,
                            const float x, const float y,
                            const float diameter,
                            const Colour& colour,
                            const float outlineThickness) throw()
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.addEllipse (x, y, diameter, diameter);

    {
        ColourGradient cg (Colours::grey.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::grey.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.2, Colours::grey.overlaidWith (colour));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
		g.fillPath (p);
    }

    {
#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(Colours::grey, 0, y + diameter * 0.06f, Colours::transparentWhite, 0, y + diameter * 0.3f, false));
#else
		g.setGradientFill (ColourGradient(Colours::grey, 0, y + diameter * 0.06f,
                          Colours::transparentWhite, 0, y + diameter * 0.3f, false));
#endif
		g.fillEllipse (x + diameter * 0.2f, y + diameter * 0.05f, diameter * 0.6f, diameter * 0.4f);
    }

    {
        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,
                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x, y + diameter * 0.5f, true);

        cg.addColour (0.7, Colours::transparentBlack);
        cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));

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

const Colour createBaseColour (const Colour& buttonColour,
                                   const bool hasKeyboardFocus,
                                   const bool isMouseOverButton,
                                   const bool isButtonDown)
{
    const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
    const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

    if (isButtonDown)
        return baseColour.contrasting (0.2f);
    else if (isMouseOverButton)
        return baseColour.contrasting (0.1f);

    return baseColour;
}

void drawTickBox(Graphics& g,
                     int x, int y, int w, int h,
                     const bool ticked,
                     const bool isEnabled,
                     const bool isMouseOverButton,
                     const bool isButtonDown,
                     int tgtype)
{
    if(tgtype == 0)
    {
        const float boxSize = w * 0.7f;

        Colour clr;
        if(ticked)
        {
            clr = Colour(255, 158, 94);
            clr = Colour(255, 158, 94);
            drawGlassLozenge(g, (float)x, (float)y, boxSize + 2, boxSize + 2, clr, 1, 0, false, false, false, false);
        }
        else
        {
            clr = Colour(33, 86, 75);
            clr = Colour(63, 75, 79);
            drawGlassLozenge(g, (float)x, (float)y, boxSize + 2, boxSize + 2, clr, 1, 0, false, false, false, false);
        }
    }
    else if(tgtype == 1)
    {
        if(ticked)
        {
            g.setColour(Colour(255, 166, 77));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 4;
        g.drawLine(xf, yf, xf + 6, yf + 6);
        g.drawLine(xf, yf + 10, xf + 5, yf + 5);
        g.drawLine(xf + 2, yf, xf + 6 + 2, yf + 6);
        g.drawLine(xf + 2, yf + 10, xf + 5 + 2, yf + 5);
        g.drawLine(xf + 4, yf, xf + 6 + 4, yf + 6);
        g.drawLine(xf + 4, yf + 10, xf + 5 + 4, yf + 5);
        g.drawLine(xf + 6, yf, xf + 6 + 6, yf + 6);
        g.drawLine(xf + 6, yf + 10, xf + 5 + 6, yf + 5);
        g.drawLine(xf + 8, yf, xf + 6 + 8, yf + 6);
        g.drawLine(xf + 8, yf + 10, xf + 5 + 8, yf + 5);
    }
    else if(tgtype == 2)
    {
        const float boxSize = 15;

        Colour clr;
        if(ticked)
        {
            g.setColour(Colour(255, 166, 77));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }

		float xf = (float)x;
        float yf = (float)y;

        g.drawHorizontalLine(y + 6, xf + 2, xf + 15);
        g.drawHorizontalLine(y + 6 + 2, xf + 2, xf + 15);
        g.drawHorizontalLine(y + 6 + 2 + 2, xf + 2, xf + 15);
        g.drawHorizontalLine(y + 6 + 2 + 2 + 2, xf + 2, xf + 15);

        //g.drawRect(x + 2, y + 6, int(boxSize) - 4, 6);
    }
    else if(tgtype == 3)
    {
        if(ticked)
        {
            g.setColour(Colour(255, 166, 77));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 4;
        g.drawLine(xf, yf, xf + 4, yf + 4);
        g.drawLine(xf, yf + 6, xf + 3, yf + 3);
        //g.drawLine(xf + 1, yf, xf + 4 + 1, yf + 4);
        //g.drawLine(xf + 1, yf + 6, xf + 3 + 1, yf + 3);
        g.drawLine(xf + 2, yf, xf + 4 + 2, yf + 4);
        g.drawLine(xf + 2, yf + 6, xf + 3 + 2, yf + 3);
        //g.drawLine(xf + 3, yf, xf + 4 + 3, yf + 4);
        //g.drawLine(xf + 3, yf + 6, xf + 3 + 3, yf + 3);
        g.drawLine(xf + 4, yf, xf + 4 + 4, yf + 4);
        g.drawLine(xf + 4, yf + 6, xf + 3 + 4, yf + 3);
    }
    else if(tgtype == 21) // Sine
    {
        if(ticked)
        {
            g.setColour(Colour(255, 125, 61));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 9;
        g.drawLine(xf, yf, xf + 3, yf - 3);
        g.drawLine(xf + 3, yf - 3, xf + 5, yf - 3);
        g.drawLine(xf + 5, yf - 3, xf + 10, yf + 2);
        g.drawLine(xf + 10, yf + 2, xf + 12, yf + 2);
        g.drawLine(xf + 12, yf + 2, xf + 15, yf - 1);
        //g.drawLine(xf + 12, yf + 4, xf + 16, yf);
    }
    else if(tgtype == 22) // Saw
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 4;
        g.drawLine(xf, yf, xf + 5, yf + 9);
        g.drawLine(xf + 5, yf + 9, xf + 5, yf);
        g.drawLine(xf + 5, yf, xf + 10, yf + 9);
        g.drawLine(xf + 10, yf + 9, xf + 10, yf);
        g.drawLine(xf + 10, yf, xf + 15, yf + 9);
    }
    else if(tgtype == 23) // Tri
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 8;
        g.drawLine(xf, yf, xf + 4, yf - 4);
        g.drawLine(xf + 4, yf - 4, xf + 11, yf + 3);
        g.drawLine(xf + 11, yf + 3, xf + 15, yf - 1);
    }
    else if(tgtype == 26) // Pulse
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 11;
        g.drawLine(xf, yf + 1, xf, yf - 6);
        g.drawLine(xf, yf - 6, xf + 7, yf - 6);
        g.drawLine(xf + 7, yf - 6, xf + 7, yf);
        g.drawLine(xf + 7, yf, xf + 14, yf);
        g.drawLine(xf + 14, yf + 1, xf + 14, yf - 6);
    }
    else if(tgtype == 27) // Noise
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 11;
        g.drawLine(xf, yf + 1, xf + 1, yf - 11);
        g.drawLine(xf + 1, yf - 6, xf + 2, yf + 2);
        g.drawLine(xf + 2, yf + 2, xf + 3, yf - 8);
        g.drawLine(xf + 3, yf - 8, xf + 4, yf + 2);
        g.drawLine(xf + 4, yf + 2, xf + 6, yf - 9);
        g.drawLine(xf + 6, yf - 9, xf + 7, yf + 3);
        g.drawLine(xf + 7, yf + 3, xf + 8, yf - 6);
        g.drawLine(xf + 8, yf - 6, xf + 9, yf + 3);
        g.drawLine(xf + 9, yf + 3, xf + 10, yf - 5);
        g.drawLine(xf + 10, yf - 5, xf + 11, yf + 2);
        g.drawLine(xf + 11, yf + 2, xf + 12, yf - 9);
        g.drawLine(xf + 12, yf - 9, xf + 13, yf + 3);
        g.drawLine(xf + 13, yf + 3, xf + 14, yf - 6);
    }
    else if(tgtype == 24) // Noise
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 4;
        g.drawLine(xf, yf, xf + 4, yf + 4);
        g.drawLine(xf, yf + 6, xf + 3, yf + 3);
        //g.drawLine(xf + 1, yf, xf + 4 + 1, yf + 4);
        //g.drawLine(xf + 1, yf + 6, xf + 3 + 1, yf + 3);
        g.drawLine(xf + 2, yf, xf + 4 + 2, yf + 4);
        g.drawLine(xf + 2, yf + 6, xf + 3 + 2, yf + 3);
        //g.drawLine(xf + 3, yf, xf + 4 + 3, yf + 4);
        //g.drawLine(xf + 3, yf + 6, xf + 3 + 3, yf + 3);
        g.drawLine(xf + 4, yf, xf + 4 + 4, yf + 4);
        g.drawLine(xf + 4, yf + 6, xf + 3 + 4, yf + 3);
    }
    else if(tgtype == 25) // Gugro
    {
        if(ticked)
        {
            g.setColour(Colour(255, 95, 21));
        }
        else
        {
            g.setColour(Colour(16, 43, 37));
        }
        //g.drawRect(x, y + 5, 11, 9);
        float xf = (float)x;
        float yf = (float)y + 4;
        g.drawLine(xf, yf, xf + 4, yf + 4);
        g.drawLine(xf, yf + 6, xf + 3, yf + 3);
        //g.drawLine(xf + 1, yf, xf + 4 + 1, yf + 4);
        //g.drawLine(xf + 1, yf + 6, xf + 3 + 1, yf + 3);
        g.drawLine(xf + 2, yf, xf + 4 + 2, yf + 4);
        g.drawLine(xf + 2, yf + 6, xf + 3 + 2, yf + 3);
        //g.drawLine(xf + 3, yf, xf + 4 + 3, yf + 4);
        //g.drawLine(xf + 3, yf + 6, xf + 3 + 3, yf + 3);
        g.drawLine(xf + 4, yf, xf + 4 + 4, yf + 4);
        g.drawLine(xf + 4, yf + 6, xf + 3 + 4, yf + 3);
    }
}

void drawGlassPointer (Graphics& g,
                           const float x, const float y,
                           const float diameter,
                           const Colour& colour, const float outlineThickness,
                           const int direction) throw()
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.startNewSubPath (x + diameter * 0.5f, y);
    p.lineTo (x + diameter, y + diameter * 0.6f);
    p.lineTo (x + diameter, y + diameter);
    p.lineTo (x, y + diameter);
    p.lineTo (x, y + diameter * 0.6f);
    p.closeSubPath();

    p.applyTransform (AffineTransform::rotation (direction * (float_Pi * 0.5f), x + diameter * 0.5f, y + diameter * 0.5f));

    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.4, Colours::white.overlaidWith (colour));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        g.fillPath (p);
    }

    {
        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,
                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x - diameter * 0.2f, y + diameter * 0.5f, true);

        cg.addColour (0.5, Colours::transparentBlack);
        cg.addColour (0.7, Colours::black.withAlpha (0.07f * outlineThickness));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
		g.fillPath (p);
    }

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.strokePath (p, PathStrokeType (outlineThickness));
}

void drawLinearSliderBackground (Graphics& g,
                                              int x, int y,
                                              int width, int height,
                                              float sliderPos,
                                              float minSliderPos,
                                              float maxSliderPos,
                                              float zeroPos,
                                              const ASlider::SliderStyle /*style*/,
                                              ASlider& slider)
{
    const float sliderRadius = (float) (getSliderThumbRadius (slider) + 2);

    const Colour trackColour(1, 115, 95);
    const Colour gradCol2(trackColour.overlaidWith (Colour (0x54000000)));
    const Colour gradCol1(trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.5f : 0.23f)));
    Path indent;
    Path indent1;

    if (slider.isHorizontal())
    {
        const float iy = y + height * 0.5f;
        const float ih = 11;

        g.setColour(Colour (20, 59, 63));
        g.fillRect(x, int(iy), width, int(ih));

        g.setColour(Colour (25, 37, 44));
        //g.strokePath(indent, PathStrokeType(1.0f));
        g.drawRect(x, int(iy), width, int(ih), 1);

        if(slider.getSigned() == false)
        {
            g.setColour(Colour(55, 101, 113));
            g.fillRect(int(x + 1), int(iy + 1), int(sliderPos), int(ih - 2));
            g.setColour(Colour(55, 74, 85));
            if((int)sliderPos > 1)
                g.fillRect(int(x + 2), int(iy + 2), int(sliderPos - 2), int(ih - 4));
            else if(sliderPos > 0.5f)
                g.drawLine(float(x + sliderPos - 1), float(iy + 1), float(x + sliderPos - 1), float(iy + 1 + ih - 2));
        }
        else
        {
            if(sliderPos != zeroPos)
            {
                g.setColour(Colour(55, 101, 113));
                if(sliderPos >= zeroPos)
                {
                    g.fillRect(x + 1 + width/2, int(iy + 1), RoundFloat(sliderPos - width/2), int(ih - 2));
                    g.setColour(Colour(55, 74, 85));
                    g.fillRect(x + 2 + width/2, int(iy + 2), RoundFloat(sliderPos - width/2 - 2), int(ih - 4));
                }
                else
                {
                    g.fillRect(x + 1 + RoundFloat(sliderPos), int(iy + 1), RoundFloat(width/2 - sliderPos), int(ih - 2));
                    g.setColour(Colour(55, 74, 85));
                    g.fillRect(x + 2 + RoundFloat(sliderPos), int(iy + 2), RoundFloat(width/2 - sliderPos - 2), int(ih - 4));
                }
            }
        }

        /*
        if(slider.getSigned() == true)
        {
            int px;
            if(sliderPos >= zeroPos)
            {
                px = 1 + ApproxFloat(sliderPos);
            }
            else
            {
                px = ApproxFloat(sliderPos);
            }
            g.setColour(Colour (255, 255, 255));
            g.fillRect(x + px, int(iy + 1), 1, int(ih - 2));
        }
        else*/
        {
            int px = 1 + RoundFloat(sliderPos);
            g.setColour(Colour(185, 205, 225).darker(0.1f));
            g.fillRect(x + px - 2, int(iy + 1), 5, int(ih - 2));
            g.setColour(Colour(75, 95, 105).darker(0.1f));
            g.fillRect(x + px, int(iy + 2), 1, int(ih - 4));
        }
    }
    else
    {
        const float ix = x + width * 0.5f;
        const float iw = sliderRadius;

        ColourGradient cg (gradCol1, ix, 0.0f,
                           gradCol2, ix + iw, 0.0f, false);
#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
        indent.addRectangle(ix, float(y), iw, float(height));
        g.fillPath (indent);
    }

    //g.setColour (Colour (0x4c000000));
    //g.strokePath (indent, PathStrokeType (0.5f));
}

void drawLinearSliderThumb (Graphics& g,
                                         int x, int y,
                                         int width, int height,
                                         float sliderPos,
                                         float minSliderPos,
                                         float maxSliderPos,
                                         const ASlider::SliderStyle style,
                                         ASlider& slider)
{
    const float sliderRadius = (float) (getSliderThumbRadius (slider) - 4);

    Colour knobColour(Colour(125, 175, 255)); //(Colour(255, 175, 125));

    const float outlineThickness = 1.0f;

    if (style == ASlider::LinearHorizontal || style == ASlider::LinearVertical)
    {
        float kx, ky;

        if (style == ASlider::LinearVertical)
        {
            kx = x + width * 0.5f;
            ky = sliderPos;
        }
        else
        {
            kx = sliderPos + 1;
            ky = y + height * 0.5f;
        }

        drawGlassSphere(g,
                        kx - sliderRadius,
                        ky - sliderRadius,
                        sliderRadius * 2.0f,
                        knobColour, outlineThickness);
    }
    else
    {
        if (style == ASlider::ThreeValueVertical)
        {
            drawGlassSphere (g, x + width * 0.5f - sliderRadius,
                             sliderPos - sliderRadius,
                             sliderRadius * 2.0f,
                             knobColour, outlineThickness);
        }
        else if (style == ASlider::ThreeValueHorizontal)
        {
            drawGlassSphere (g,sliderPos - sliderRadius,
                             y + height * 0.5f - sliderRadius,
                             sliderRadius * 2.0f,
                             knobColour, outlineThickness);
        }

        if (style == ASlider::TwoValueVertical || style == ASlider::ThreeValueVertical)
        {
            const float sr = jmin (sliderRadius, width * 0.4f);

            drawGlassPointer (g, jmax (0.0f, x + width * 0.5f - sliderRadius * 2.0f),
                              minSliderPos - sliderRadius,
                              sliderRadius * 2.0f, knobColour, outlineThickness, 1);

            drawGlassPointer (g, jmin (x + width - sliderRadius * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                              sliderRadius * 2.0f, knobColour, outlineThickness, 3);
        }
        else if (style == ASlider::TwoValueHorizontal || style == ASlider::ThreeValueHorizontal)
        {
            const float sr = jmin (sliderRadius, height * 0.4f);

            drawGlassPointer (g, minSliderPos - sr,
                              jmax (0.0f, y + height * 0.5f - sliderRadius * 2.0f),
                              sliderRadius * 2.0f, knobColour, outlineThickness, 2);

            drawGlassPointer (g, maxSliderPos - sliderRadius,
                              jmin (y + height - sliderRadius * 2.0f, y + height * 0.5f),
                              sliderRadius * 2.0f, knobColour, outlineThickness, 4);
        }
    }
}

void drawShinyButtonShape(Graphics& g,
                                 float x, float y, float w, float h,
                                 float maxCornerSize,
                                 const Colour& baseColour,
                                 const float strokeWidth,
                                 const bool flatOnLeft,
                                 const bool flatOnRight,
                                 const bool flatOnTop,
                                 const bool flatOnBottom)
{
    if (w <= strokeWidth * 1.1f || h <= strokeWidth * 1.1f)
        return;

    const float cs = jmin (maxCornerSize, w * 0.5f, h * 0.5f);

    Path outline;
    createRoundedPath (outline, x, y, w, h, cs,
                        ! (flatOnLeft || flatOnTop),
                        ! (flatOnRight || flatOnTop),
                        ! (flatOnLeft || flatOnBottom),
                        ! (flatOnRight || flatOnBottom));

    ColourGradient cg (baseColour, 0.0f, y,
                       baseColour.overlaidWith(Colour(0x070000ff)), 0.0f, y + h,
                       false);

    cg.addColour (0.5, baseColour.overlaidWith(Colour(0x33ffffff)));
    cg.addColour (0.51, baseColour.overlaidWith(Colour(0x110000ff)));

#ifdef USE_OLD_JUCE
		g.setBrush(&GradientBrush(cg));
#else
        g.setGradientFill(cg);
#endif
	g.fillPath (outline);

    g.setColour(Colour(0x80000000));
    g.strokePath(outline, PathStrokeType(strokeWidth));
}

void drawLinearSlider(Graphics& g,
                         int x, int y,
                         int width, int height,
                         float sliderPos,
                         float minSliderPos,
                         float maxSliderPos,
                         float zeroPos,
                         const ASlider::SliderStyle style,
                         ASlider& slider)
{
    g.fillAll (slider.findColour (ASlider::backgroundColourId));

    if (style == ASlider::LinearBar)
    {
        g.setColour(Colour(46, 59, 65));
        g.fillRect(x, y, width, height);

        g.setColour(Colour(128, 144, 149));
        g.drawRect(x - 1, y - 1, width + 2, height + 2, 1);

        const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
        Colour baseColour(Colour(145, 165, 175));

        drawShinyButtonShape (g,
                              (float) x + 1, (float) y + 1, sliderPos - (float) x - 2, (float) height - 2, 0.0f,
                              baseColour,
                              0,
                              true, true, true, true);
    }
    else
    {
        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, zeroPos, style, slider);
        //drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void drawRotarySlider (Graphics& g,
                          int x, int y,
                          int width, int height,
                          float sliderPos,
                          const float rotaryStartAngle,
                          const float rotaryEndAngle,
                          ASlider& slider)
{
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    if (radius > 12.0f)
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (ASlider::rotarySliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));

        const float thickness = 0.7f;

        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw,
                                    rotaryStartAngle,
                                    angle,
                                    thickness);

            g.fillPath (filledArc);
        }

        if (thickness > 0)
        {
            const float innerRadius = radius * 0.2f;
            Path p;
            p.addTriangle (-innerRadius, 0.0f,
                           0.0f, -radius * thickness * 1.1f,
                           innerRadius, 0.0f);

            p.addEllipse (-innerRadius, -innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

            g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
        }

        if (slider.isEnabled())
        {
            g.setColour (slider.findColour (ASlider::rotarySliderOutlineColourId));
            Path outlineArc;
            outlineArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
            outlineArc.closeSubPath();

            g.strokePath (outlineArc, PathStrokeType (slider.isEnabled() ? (isMouseOver ? 2.0f : 1.2f) : 0.3f));
        }
    }
    else
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (ASlider::rotarySliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));

        Path p;
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType (rw * 0.1f).createStrokedPath (p, p);

        p.addLineSegment (0.0f, 0.0f, 0.0f, -radius, rw * 0.2f);

        g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
    }
}

Button* createSliderButton(const bool isIncrement)
{
    return new TextButton (isIncrement ? "+" : "-", String::empty);
}

ALabel* createSliderTextBox (ASlider& slider)
{
    ALabel* const l = new ALabel(T("n"), String::empty);
    l->setFont(*ins);
    l->setJustificationType(Justification::centred);
    l->setColour(Label::textColourId, Colour(175, 191, 199));
    l->setColour(Label::backgroundColourId,
                  (slider.getSliderStyle() == ASlider::LinearBar) ? Colours::transparentBlack
                                                                 : slider.findColour (ASlider::textBoxBackgroundColourId));
    l->setColour(Label::outlineColourId, slider.findColour (ASlider::textBoxOutlineColourId));
    l->setColour(TextEditor::textColourId, slider.findColour (ASlider::textBoxTextColourId));
    l->setColour(TextEditor::backgroundColourId,
                  slider.findColour (ASlider::textBoxBackgroundColourId)
                        .withAlpha (slider.getSliderStyle() == ASlider::LinearBar ? 0.7f : 1.0f));
    l->setColour(TextEditor::outlineColourId, slider.findColour (ASlider::textBoxOutlineColourId));

    return l;
}

class AMidiInputSelectorComponentListBox  : public ListBox,
                                           public ListBoxModel
{
public:

    AMidiInputSelectorComponentListBox (JuceAudioDeviceManager& deviceManager_,
                                       const String& noItemsMessage_,
                                       const int minNumber_,
                                       const int maxNumber_)
        : ListBox (String::empty, 0),
          deviceManager (deviceManager_),
          noItemsMessage (noItemsMessage_),
          minNumber (minNumber_),
          maxNumber (maxNumber_)
    {
        items = MidiInput::getDevices();

        setModel (this);
        setOutlineThickness (1);
    }

    ~AMidiInputSelectorComponentListBox()
    {
    }

    int getNumRows()
    {
        return items.size();
    }

    void paintListBoxItem (int row,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected)
    {
        if (((unsigned int) row) < (unsigned int) items.size())
        {
            if (rowIsSelected)
                g.fillAll (findColour (TextEditor::highlightColourId)
                               .withMultipliedAlpha (0.3f));

            const String item (items [row]);
            bool enabled = deviceManager.isMidiInputEnabled (item);

            const int x = getTickX();
            const int tickW = height;

            getLookAndFeel().drawTickBox (g, *this, x - tickW, (height - tickW - 2), tickW + 1, tickW + 1,
                                          enabled, true, true, false);

            g.setFont (*ins);
            g.setColour (Colour(145, 155, 165).withMultipliedAlpha (enabled ? 1.0f : 0.6f));
            g.setColour (Colour(155, 169, 174).withMultipliedAlpha (enabled ? 1.0f : 0.6f));
            g.drawText (item, x, 0, width - x - 2, height, Justification::centredLeft, true);
        }
    }

    void listBoxItemClicked (int row, const MouseEvent& e)
    {
        selectRow (row);

        if (e.x < getTickX())
            flipEnablement (row);
    }

    void listBoxItemDoubleClicked (int row, const MouseEvent&)
    {
        flipEnablement (row);
    }

    void returnKeyPressed (int row)
    {
        flipEnablement (row);
    }

    void paint (Graphics& g)
    {
        ListBox::paint (g);

        if (items.size() == 0)
        {
            g.setColour (Colours::grey);
            g.setFont (13.0f);
            g.drawText (noItemsMessage,
                        0, 0, getWidth(), getHeight() / 2,
                        Justification::centred, true);
        }
    }

    int getBestHeight (const int preferredHeight)
    {
        const int extra = getOutlineThickness() * 2;

        return jmax (getRowHeight() * 2 + extra,
                     jmin (getRowHeight() * getNumRows() + extra,
                           preferredHeight));
    }

    juce_UseDebuggingNewOperator

private:

    JuceAudioDeviceManager& deviceManager;
    const String noItemsMessage;
    StringArray items;
    int minNumber, maxNumber;

    void flipEnablement (const int row)
    {
        if (((unsigned int) row) < (unsigned int) items.size())
        {
            const String item (items [row]);
            deviceManager.setMidiInputEnabled (item, ! deviceManager.isMidiInputEnabled (item));
        }
    }

    int getTickX() const throw()
    {
        return getRowHeight() + 5;
    }

    AMidiInputSelectorComponentListBox (const AMidiInputSelectorComponentListBox&);
    const AMidiInputSelectorComponentListBox& operator= (const AMidiInputSelectorComponentListBox&);
};


ConfigComponent::ConfigComponent()      //: quitButton(0)
{
    sunimg = ImageFileFormat::loadFrom(images::sunconfig_png, images::sunconfig_pngSize);

    //Group = new AGroupComponent(T("Group"), T("Output device"));
    //addAndMakeVisible(Group);

    //Group2 = new AGroupComponent(T("Group"), T("Other settings"));
    //addAndMakeVisible(Group2);

    //AGroupComponent* group = new AGroupComponent(T("Group"), T(""));
    //addAndMakeVisible(group);
    //group->setBounds(0, 2, 350 - 1, 277 - 2);

    int st = 12;
    int xsl = 6;
    int ysl = 45;

    buffLabel = new ALabel(T("BufferSize"), T("Buffer:"));
    buffLabel->setFont(*ti);
    buffLabel->setBounds(xsl, ysl - 3, 89, 20);
    buffLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    addAndMakeVisible(buffLabel);

    addAndMakeVisible(pBufferSlider = new ASlider(T("BufferSlider")));
    pBufferSlider->setRange(512, 512*32, 2);
    //slider->setPopupMenuEnabled (true);
    //pBufferSlider->setValue(Random::getSystemRandom().nextDouble() * 100, false, false);
    pBufferSlider->setValue(MAudio->getBufferSize(), false, false);
    pBufferSlider->setSliderStyle(ASlider::LinearHorizontal);
    pBufferSlider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 180, 20);
    pBufferSlider->setTextBoxIsEditable(false);
    unsigned int uiTime = unsigned int(pBufferSlider->getValue());
    unsigned int uiFreq = unsigned int(MAudio->getSampleRate()/1000);
    uiTime = uiTime/ uiFreq;
    pBufferSlider->setTextValueSuffix(String::formatted(T(" samples")));
    pBufferSlider->setTextBoxXY(39, 0);
    pBufferSlider->setBounds(xsl, ysl, 150, 30);
    pBufferSlider->addListener(this);

    addChildComponent(EdButt = new ATextButton(String::empty));

    EdButt->setButtonText(T("Show device panel"));
    EdButt->addButtonListener(this);
    EdButt->setBounds(xsl, ysl + 1, 111, 22);

    int_comboBox = new AComboBox(T("Pattern interpolation"));
    addAndMakeVisible(int_comboBox);

    int_comboBox->setEditableText(false);
    int_comboBox->setJustificationType(Justification::centred);
    int_comboBox->addItem(T("Linear"), Interpol_linear);
    int_comboBox->addItem(T("3-point Hermite"), Interpol_3dHermit);
    int_comboBox->addItem(T("6-point Polinomial"), Interpol_6dHermit);
    int_comboBox->addItem(T("Audio_Sample_Sinc depth 64"), Interpol_64Sinc);

    //TODO: We are not going to support 128 and 256 points sinc interpolation coz it's bloody slow
    //    int_comboBox->addItem(T("Sinc depth 128"), Interpol_128Sinc);
    //    int_comboBox->addItem(T("Sinc depth 256"), Interpol_256Sinc);

    int_comboBox->setSelectedId(Config_DefaultInterpolation);
    int_comboBox->setEditableText(false);
    int_comboBox->setBounds(22, 122, 116, 18);
    int_comboBox->addListener(this);

    // Interpolation label and box
    ALabel* intLabel = new ALabel(T(""), T("Pattern interpolation:"));

    intLabel->setFont(*ti);
    intLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    intLabel->setBounds(15, 104, 95, 17);

    addAndMakeVisible(intLabel);

    int xrate = 2;
    int yrate = 92;

    int yr = 49;

    ALabel* rLabel = new ALabel(T(""), T("Sample rate"));

    rLabel->setFont(*ti);
    rLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    rLabel->setBounds(xrate, yrate - 20, 75, 17);

    addAndMakeVisible(rLabel);

    tb22 = new AToggleButton(T("22050 Hz"));
    addAndMakeVisible(tb22);

    tb22->setRadioGroupId(1234);
    tb22->setBounds(xrate, yrate, 70, 22);
    tb22->addButtonListener(this);
    tb22->setVisible(false);

    tb44 = new AToggleButton(T("44100 Hz"));
    addAndMakeVisible(tb44);

    tb44->setToggleState(true, false);
    tb44->setRadioGroupId(1234);
    tb44->setBounds(xrate, yrate /*+= st*/, 70, 22);
    tb44->addButtonListener(this);

    tb48 = new AToggleButton(T("48000 Hz"));
    addAndMakeVisible(tb48);

    tb48->setRadioGroupId(1234);
    tb48->setBounds(xrate, yrate+= st, 70, 22);
    tb48->addButtonListener(this);
    tb48->setVisible(false);

    tb96 = new AToggleButton(T("96000 Hz"));
    addAndMakeVisible(tb96);

    tb96->setRadioGroupId(1234);
    tb96->setBounds(xrate, yrate+= st, 70, 22);
    tb96->addButtonListener(this);
    tb96->setVisible(false);

    tb192 = new AToggleButton(T("192000 Hz"));
    addAndMakeVisible(tb192);

    tb192->setRadioGroupId(1234);
    tb192->setBounds(xrate, yrate+= st, 100, 22);
    tb192->addButtonListener(this);
    tb192->setVisible(false);

    tbLength = new AToggleButton(T("Remember note length"));
    addAndMakeVisible(tbLength);

    tbLength->setRadioGroupId(0);
    tbLength->setBounds(xrate, 153, 155, 22);
    tbLength->setName(T("BtLen"));
    tbLength->addButtonListener(this);

    juceAudioDeviceDropDown = new AComboBox ("device");


    JAudioManager->addDeviceNamesToComboBox(*juceAudioDeviceDropDown);

    juceAudioDeviceDropDown->setSelectedId(-1, true);

    JuceAudioDeviceManager::AudioDeviceSetup setup;

    JAudioManager->getAudioDeviceSetup(setup);

    if(setup.outputDeviceName.isNotEmpty())
    {
        juceAudioDeviceDropDown->setText(setup.outputDeviceName, true);
    }

    juceAudioDeviceDropDown->addListener (this);

    addAndMakeVisible (juceAudioDeviceDropDown);

    juceAudioDeviceDropDown->setBounds(xsl, 25, 245, 18);

    //MidiInput::getDevices();

    addAndMakeVisible (midiInputsList = new AMidiInputSelectorComponentListBox (*JAudioManager, TRANS("(no midi inputs available)"), 0, 0));

    midiInputsList->setBounds(xsl + 245 + 25, 25, 215, 128);

    ALabel* midiLabel = new ALabel(T(""), T("Active MIDI inputs"));
    midiLabel->setFont(*ti);
    midiLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    midiLabel->setBounds(xsl + 245 + 25, 7, 85, 17);
    addAndMakeVisible(midiLabel);

    //ALabel* devlabel = new ALabel ("l1", TRANS ("audio device:"));
    //devlabel->attachToComponent(audioDeviceDropDown, true);

    ALabel* outLabel = new ALabel(T(""), T("Output device"));

    outLabel->setFont(*ti);
    outLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    outLabel->setBounds(xsl, 7, 85, 17);

    addAndMakeVisible(outLabel);

    //addAndMakeVisible(OKButt = new ATextButton(String::empty));
    //OKButt->setButtonText(T("OK"));
    //OKButt->addButtonListener(this);

    UpdateComponentsVisibility();

    setSize(800, 800);
}

void ConfigComponent::resized()
{
    //Group->setBounds(1, 3, 347, 120);
    //Group2->setBounds(1, 122, 347, 154);
#ifndef USE_JUCE_AUDIO
    //Group1->setBounds(1, 174, 347, 95);
#endif

    //OKButt->setBounds(getWidth() - 78, getHeight() - 39, 70, 32);
}

void ConfigComponent::paint(Graphics &g)
{
    g.saveState();
    g.reduceClipRegion(2, 4, getWidth() - 2, getHeight() - 6);
    g.drawImageAt(sunimg, 0, 0, false);
    g.restoreState();
}

void ConfigComponent::sliderValueChanged (ASlider* slider)
{
    if (slider == this->pBufferSlider)
    {
        //When the slider value is changed, we should update the bufer size label in ms
        unsigned int uiTime = unsigned int(pBufferSlider->getValue());
        unsigned int uiFreq = unsigned int(MAudio->getSampleRate()/1000);
        uiTime = uiTime/ uiFreq;
        pBufferSlider->setTextValueSuffix(String::formatted(T(" samples")));
    }
}

void ConfigComponent::sliderDragEnded (ASlider* slider)
{
    if (slider == this->pBufferSlider)
    {
        MAudio->setBufferSize((unsigned)(pBufferSlider->getValue()));
    }
}

void ConfigComponent::comboBoxChanged (AComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == juceAudioDeviceDropDown)
    {
        if (juceAudioDeviceDropDown->getSelectedId() < 0)
        {
            //audioDeviceManager->setAudioDevice (String::empty, 0, 0, 0, 0, true);

            JAudioManager->closeAudioDevice();
        }
        else
        {
            JuceAudioDeviceManager::AudioDeviceSetup oldSetup;
            JuceAudioDeviceManager::AudioDeviceSetup setup;
            JAudioManager->getAudioDeviceSetup(setup);
            oldSetup = setup;
            setup.outputDeviceName = juceAudioDeviceDropDown->getText();
            setup.bufferSize = DEFAULT_BUFFER_SIZE;
            AudioIODeviceType* type = JAudioManager->getDeviceTypeObjectByDeviceName(setup.outputDeviceName);

            String error (JAudioManager->setAudioDeviceSetup(setup, true));

            //String error (audioDeviceManager->setAudioDevice (juceAudioDeviceDropDown->getText(),
            //                                            0, 0, 0, 0, true));

            if (error.isNotEmpty())
            {
#if JUCE_WIN32
                if (setup.inputChannels.countNumberOfSetBits() > 0
                      && setup.outputChannels.countNumberOfSetBits() > 0)
                {
                    // in DSound, some machines lose their primary input device when a mic
                    // is removed, and this also buggers up our attempt at opening an output
                    // device, so this is a workaround that doesn't fail in that case.
                    BitArray noInputs;
                    setup.inputChannels = noInputs;
                    error = JAudioManager->setAudioDeviceSetup(setup, true);
                }
#endif
                if (error.isNotEmpty())
                    MWindow->showAlertBox("Error while opening" + (std::string)juceAudioDeviceDropDown->getText());
            }
        }

        if(JAudioManager->getCurrentAudioDevice() != NULL)
        {
	        JuceAudioDeviceManager::AudioDeviceSetup setup;
            JAudioManager->getAudioDeviceSetup(setup);
            juceAudioDeviceDropDown->setText(setup.outputDeviceName, true);
        }
        else
        {
            juceAudioDeviceDropDown->setSelectedId(-1, true);
        }

        UpdateComponentsVisibility();
    }
    else if(comboBoxThatHasChanged == int_comboBox)
    {
        Config_DefaultInterpolation = comboBoxThatHasChanged->getSelectedId();
    }
}


ConfigComponent::~ConfigComponent()
{
}

void ConfigComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked->getRadioGroupId() == 1234)
    {
        unsigned int tmpFreq;

        tmpFreq = buttonThatWasClicked->getName().getIntValue();
        if (tmpFreq != MAudio->getSampleRate())
        {
            std::string ch = MWindow->showAlertBox("Changing sample rate requires current project to be reopened.\n\nAre you sure you want to proceed?", "Yes", "No");

            if (ch == "Yes")
            {
                //Update slider suffix
                unsigned int uiTime = unsigned int(pBufferSlider->getValue());
                unsigned int uiFreq = unsigned int(tmpFreq/1000);

                uiTime = uiTime/ uiFreq;

                pBufferSlider->setTextValueSuffix(String::formatted(T(" samples (%u ms)"), uiTime));

                MAudio->setSampleRate((float)tmpFreq);
            }
            else
            {
                switch ((int)MAudio->getSampleRate())
                {
                    case 22050:
                    {
                        tb22->setToggleState(true, false);
                        break;
                    }
                    case 44100:
                    {
                        tb44->setToggleState(true, false);
                        break;
                    }
                    case 48000:
                    {
                        tb48->setToggleState(true, false);
                        break;
                    }
                    case 96000:
                    {
                        tb96->setToggleState(true, false);
                        break;
                    }
                    case 192000:
                    {
                        tb192->setToggleState(true, false);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    else if(OKButt == buttonThatWasClicked)
    {
       // ConfigWnd->setVisible(false);
    }
    else if(EdButt == buttonThatWasClicked)
    {
        JAudioManager->getCurrentAudioDevice()->showControlPanel();
    }
}

void ConfigComponent::mouseDown(const MouseEvent& e)
{
    int a = 111;
}

void ConfigComponent::UpdateComponentsVisibility()
{
    JuceAudioDeviceManager::AudioDeviceSetup setup;
    JAudioManager->getAudioDeviceSetup(setup);

	if(JAudioManager->getCurrentAudioDevice() != NULL)
    {
        if(JAudioManager->getCurrentAudioDevice()->hasControlPanel())
        {
            EdButt->setVisible(true);
        }
        else
        {
            EdButt->setVisible(false);
        }

        String tname = JAudioManager->getCurrentAudioDevice()->getTypeName();
        if(tname == T("ASIO"))
        {
            pBufferSlider->setVisible(false);
            buffLabel->setVisible(false);
        }
        else
        {
            pBufferSlider->setVisible(true);
            buffLabel->setVisible(true);

            pBufferSlider->setValue(JAudioManager->getCurrentAudioDevice()->getCurrentBufferSizeSamples(), true, true);
        }
    }
    else
    {
        EdButt->setVisible(false);
    }
}

void ProgressTimer::timerCallback()
{
    unsigned int progress;
    long ullCurPos = 0, ullTotalLength = 0;

    if (Render_.GetState() == RNDR_STATUS_DATA_PROCESSING)
    {
        Render_.GetAudioLength(ullTotalLength);
        Render_.GetCurrentPos(ullCurPos);

        progress = unsigned((100.f/(float)ullTotalLength) * (float)ullCurPos);

        ((RenderComponent*)(this->pContent))->SetSlider(progress);
    }
    else
    {
       ((RenderComponent*)(this->pContent))->SetSlider(0);
        this->stopTimer();
        Handle_RenderStop();
       ((RenderComponent*)(this->pContent))->startButt->setButtonText(T("Start"));
    }
}

RenderComponent::RenderComponent()      //: quitButton(0)
{
    waveimg = ImageFileFormat::loadFrom(images::waverender_png, images::waverender_pngSize);

    //AGroupComponent* group = new AGroupComponent(T("Group"), T(""));
    //addAndMakeVisible(group);
    //group->setBounds(0, 2, 300 - 1, 285 - 2);

    //-----------------------------------
    //--- Settings group
    //settingsGroup = new AGroupComponent(T(""), T("Settings"));
    //addAndMakeVisible(settingsGroup);
    // Format label and buttons
    int st = 12;
    int xtype = 14;
    int ytype = 30;
    formatLabel = new ALabel(T(""), T("Format"));
    formatLabel->setFont(*ti);
    formatLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    formatLabel->setBounds(xtype, ytype - 20, 45, 17);
    addAndMakeVisible(formatLabel);

    tbWav = new AToggleButton(T("WAV"));
    addAndMakeVisible(tbWav);
    tbWav->setToggleState(Render_Config.format == RNDR_FORMAT_WAVE, false);
    tbWav->setRadioGroupId(111);
    tbWav->setBounds(xtype, ytype, 70, 22);
    tbWav->setName(T("BtWav"));
    tbWav->addButtonListener(this);

    tbOgg = new AToggleButton(T("OGG"));
    addAndMakeVisible(tbOgg);
    tbOgg->setToggleState(Render_Config.format == RNDR_FORMAT_OGG, false);
    tbOgg->setRadioGroupId(111);
    tbOgg->setBounds(xtype, ytype + st, 70, 22);
    tbOgg->setName(T("BtOgg"));
    tbOgg->addButtonListener(this);

    tbFlac = new AToggleButton(T("FLAC"));
    addAndMakeVisible(tbFlac);
    tbFlac->setToggleState(Render_Config.format == RNDR_FORMAT_FLAC, false);
    tbFlac->setRadioGroupId(111);
    tbFlac->setBounds(xtype, ytype + st + st, 70, 22);
    tbFlac->setName(T("BtFlac"));
    tbFlac->addButtonListener(this);

    AToggleButton* currTBF = Render_Config.format == RNDR_FORMAT_WAVE ? tbWav : 
                             Render_Config.format == RNDR_FORMAT_OGG ? tbOgg : 
                             Render_Config.format == RNDR_FORMAT_FLAC ? tbFlac : NULL;

    // Quality label and buttons
    int xkb = 73;
    int ykb = 30;

    qLabel = new ALabel(T(""), T("Quality"));
    qLabel->setFont(*ti);
    qLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    qLabel->setBounds(xkb, ykb - 20, 45, 17);
    addAndMakeVisible(qLabel);

    tb8Bit = new AToggleButton(T("8Bit"));
    addAndMakeVisible(tb8Bit);
    tb8Bit->setToggleState(Render_Config.q1 == 0, false);
    tb8Bit->setRadioGroupId(222);
    tb8Bit->setBounds(xkb, ykb, 70, 22);
    tb8Bit->setName(T("Bt8"));
    tb8Bit->addButtonListener(this);

    tb16Bit = new AToggleButton(T("16Bit"));
    addAndMakeVisible(tb16Bit);
    tb16Bit->setToggleState(Render_Config.q1 == 1, false);
    tb16Bit->setRadioGroupId(222);
    tb16Bit->setBounds(xkb, ykb + st, 70, 22);
    tb16Bit->setName(T("Bt16"));
    tb16Bit->addButtonListener(this);

    tb24Bit = new AToggleButton(T("24Bit"));
    addAndMakeVisible(tb24Bit);
    tb24Bit->setToggleState(Render_Config.q1 == 2, false);
    tb24Bit->setRadioGroupId(222);
    tb24Bit->setBounds(xkb, ykb + st*2, 70, 22);
    tb24Bit->setName(T("Bt24"));
    tb24Bit->addButtonListener(this);

    tb32Bit = new AToggleButton(T("32Bit"));
    addAndMakeVisible(tb32Bit);
    tb32Bit->setToggleState(Render_Config.q1 == 3, false);
    tb32Bit->setRadioGroupId(222);
    tb32Bit->setBounds(xkb, ykb + st*3, 70, 22);
    tb32Bit->setName(T("Bt32"));
    tb32Bit->addButtonListener(this);

    tb32BitF = new AToggleButton(T("32Bit float"));
    addAndMakeVisible(tb32BitF);
    tb32BitF->setToggleState(Render_Config.q1 == 4, false);
    tb32BitF->setRadioGroupId(222);
    tb32BitF->setBounds(xkb, ykb + st*4, 100, 22);
    tb32BitF->setName(T("BtFloat"));
    tb32BitF->addButtonListener(this);

    tb00 = new AToggleButton(T("0 (~ 64kbps)"));
    addChildComponent(tb00);
    tb00->setToggleState(Render_Config.q2 == 0, false);
    tb00->setRadioGroupId(333);
    tb00->setBounds(xkb, ykb, 120, 22);
    tb00->setName(T("Bt00"));
    tb00->addButtonListener(this);

    tb01 = new AToggleButton(T("1"));
    addChildComponent(tb01);
    tb01->setToggleState(Render_Config.q2 == 1, false);
    tb01->setRadioGroupId(333);
    tb01->setBounds(xkb, ykb + st, 70, 22);
    tb01->setName(T("Bt01"));
    tb01->addButtonListener(this);

    tb02 = new AToggleButton(T("2"));
    addChildComponent(tb02);
    tb02->setToggleState(Render_Config.q2 == 2, false);
    tb02->setRadioGroupId(333);
    tb02->setBounds(xkb, ykb + st*2, 70, 22);
    tb02->setName(T("Bt02"));
    tb02->addButtonListener(this);

    tb03 = new AToggleButton(T("3 (~ 110kbps)"));
    tb03->setToggleState(Render_Config.q2 == 3, false);
    addChildComponent(tb03);
    tb03->setRadioGroupId(333);
    tb03->setBounds(xkb, ykb + st*3, 130, 22);
    tb03->setName(T("Bt03"));
    tb03->addButtonListener(this);

    tb04 = new AToggleButton(T("4"));
    addChildComponent(tb04);
    tb04->setToggleState(Render_Config.q2 == 4, false);
    tb04->setRadioGroupId(333);
    tb04->setBounds(xkb, ykb + st*4, 70, 22);
    tb04->setName(T("Bt04"));
    tb04->addButtonListener(this);

    tb05 = new AToggleButton(T("5 (~ 160kbps)"));
    addChildComponent(tb05);
    tb05->setToggleState(Render_Config.q2 == 5, false);
    tb05->setRadioGroupId(333);
    tb05->setBounds(xkb, ykb + st*5, 130, 22);
    tb05->setName(T("Bt05"));
    tb05->addButtonListener(this);

    tb06 = new AToggleButton(T("6"));
    addChildComponent(tb06);
    tb06->setToggleState(Render_Config.q2 == 6, false);
    tb06->setRadioGroupId(333);
    tb06->setBounds(xkb, ykb + st*6, 70, 22);
    tb06->setName(T("Bt06"));
    tb06->addButtonListener(this);

    tb07 = new AToggleButton(T("7"));
    addChildComponent(tb07);
    tb07->setToggleState(Render_Config.q2 == 7, false);
    tb07->setRadioGroupId(333);
    tb07->setBounds(xkb, ykb + st*7, 70, 22);
    tb07->setName(T("Bt07"));
    tb07->addButtonListener(this);

    tb08 = new AToggleButton(T("8"));
    addChildComponent(tb08);
    tb08->setToggleState(Render_Config.q2 == 8, false);
    tb08->setRadioGroupId(333);
    tb08->setBounds(xkb, ykb + st*8, 70, 22);
    tb08->setName(T("Bt08"));
    tb08->addButtonListener(this);

    tb09 = new AToggleButton(T("9"));
    addChildComponent(tb09);
    tb09->setToggleState(Render_Config.q2 == 9, false);
    tb09->setRadioGroupId(333);
    tb09->setBounds(xkb, ykb + st*9, 70, 22);
    tb09->setName(T("Bt09"));
    tb09->addButtonListener(this);

    tb10 = new AToggleButton(T("10 (~ 400kbps)"));
    addChildComponent(tb10);
    tb10->setToggleState(Render_Config.q2 == 10, false);
    tb10->setRadioGroupId(333);
    tb10->setBounds(xkb, ykb + st*10, 135, 22);
    tb10->setName(T("Bt10"));
    tb10->addButtonListener(this);

    int_comboBox = new AComboBox(T("Interpolation"));
    addAndMakeVisible(int_comboBox);
    int_comboBox->setEditableText(false);
    int_comboBox->setJustificationType(Justification::centred);
    int_comboBox->addItem(T("Linear"), Interpol_linear);
    int_comboBox->addItem(T("3-point Hermite"), Interpol_3dHermit);
    int_comboBox->addItem(T("6-point Polinomial"), Interpol_6dHermit);
    int_comboBox->addItem(T("Audio_Sample_Sinc depth 64"), Interpol_64Sinc);

//TODO: We are not going to support 128 and 256 points sinc interpolation coz it's bloody slow
//    int_comboBox->addItem(T("Sinc depth 128"), Interpol_128Sinc);
//    int_comboBox->addItem(T("Sinc depth 256"), Interpol_256Sinc);
    int_comboBox->setSelectedId (Config_RenderInterpolation);
    int_comboBox->setEditableText(false);
    int_comboBox->setBounds(176, 35, 116, 18);
    int_comboBox->addListener(this);

    // Interpolation label and box

    intLabel = new ALabel(T(""), T("Render interpolation:"));

    intLabel->setFont(*ti);
    intLabel->setColour(ALabel::textColourId, Colour(145, 155, 159));
    intLabel->setBounds(174, 17, 115, 17);

    addAndMakeVisible(intLabel);

    //-----------------------------------
    //--- Filename group
    //
    fileName = new TextEditor(T("FileName"));
    fileName->setFont(*ins);
    fileName->setColour(TextEditor::textColourId, Colour(244, 244, 244));
    fileName->setColour(TextEditor::backgroundColourId, Colour(34, 46, 53));
    fileName->setColour(TextEditor::focusedOutlineColourId, Colour((uint8)208, (uint8)218, (uint8)228, (uint8)192));
    fileName->setColour(TextEditor::outlineColourId, Colour(116, 133, 158));
    fileName->setColour(TextEditor::shadowColourId, Colour(59, 72, 81));
    fileName->setIndents(4, 4);
    addAndMakeVisible(fileName);
    fileName->setText(T("render1"));
    fileName->addListener(this);

    //textFileStore = new ALabel(T(""), T("File location:"));
    //textFileStore->setFont(*ins);
    //textFileStore->setColour(ALabel::textColourId, Colour(157, 255, 225));
    //addAndMakeVisible(textFileStore);

    outdir = Render_Config.output_dir;

    //String strPath(szWorkingDirectory);
    //String strRender(RENDERING_DEFAULT_FOLDER);
    //strPath += strRender.fromFirstOccurrenceOf(T("."), false, true);

    textFolderStore = new ALabel(T(""), Render_Config.output_dir);
    textFolderStore->setFont(*ti);
    textFolderStore->setColour(ALabel::textColourId, Colour(100, 175, 125));
    //addAndMakeVisible(textFolderStore);
    //textFolderStore->setBounds(10, 221, 325, 120);

    addAndMakeVisible(foldButt = new ATextButton(String::empty));
    foldButt->setButtonText(T("Select folder"));
    foldButt->addButtonListener(this);

    addAndMakeVisible(expButt = new ATextButton(String::empty));
    expButt->setButtonText(T("Open folder"));
    expButt->addButtonListener(this);

    addAndMakeVisible(resButt = new ATextButton(String::empty));
    resButt->setButtonText(T("Set folder to \\Rendered"));
    resButt->addButtonListener(this);


    // Progress bar
    addAndMakeVisible(slider = new ASlider(T("slider")));
    slider->setRange(0.0, 100.0, 1);
    //slider->setPopupMenuEnabled (true);
    slider->setValue(0, false, false);
    slider->setSliderStyle(ASlider::LinearBar);
    //slider->setTextBoxStyle(ASlider::TextBoxPositioned, false, 20, 20);
    slider->setTextValueSuffix("%");
    slider->setTextBoxIsEditable(false);
    slider->setTextBoxXY(205, 0);
    slider->setInterceptsMouseClicks(false, false);
    //slider->setValue(25, false, false);
    pSlideTimer = new ProgressTimer((JuceComponent*)this);

    // Start button
    addAndMakeVisible(startButt = new ATextButton(String::empty));
    startButt->setButtonText(T("Start"));
    startButt->addButtonListener(this);

    buttonClicked(currTBF);

    setSize(300, 275);
}

void RenderComponent::resized()
{
    //settingsGroup->setBounds(1, 5, 297, 180);
    //nameGroup->setBounds(0, 208, 221, 31);

    fileName->setBounds(5, 205, 211, 20);
    //textFileStore->setBounds(10, 218, 325, 120);
    foldButt->setBounds(4, 179, 75, 22);
    expButt->setBounds(78, 179, 75, 22);
    resButt->setBounds(152, 179, 132, 22);

    slider->setBounds(3, getHeight() - 45, 239, 42);
    startButt->setBounds(getWidth() - 55, getHeight() - 45, 52, 42);
}

RenderComponent::~RenderComponent()
{
    delete pSlideTimer;

    pSlideTimer = NULL;
}

void RenderComponent::paint(Graphics &g)
{
    g.saveState();

    g.reduceClipRegion(2, 4, getWidth() - 3, getHeight());

    g.drawImageAt(waveimg, 0, 36, false);

    g.restoreState();
}

void RenderComponent::SetSlider(unsigned int progress)
{
    MessageManagerLock ml;

    slider->setValue(progress, false, false);
}

void RenderComponent::buttonClicked(Button* buttonThatWasClicked)
{
    //if (buttonThatWasClicked == quitButton)
    //{
    //   JUCEApplication::quit();
    //}

    if ( buttonThatWasClicked->getButtonText() == T("Start") )
    {
        //Lets find what quality setting to use
        switch (Render_Config.format)
        {
            case RNDR_FORMAT_WAVE:
            case RNDR_FORMAT_FLAC:
            {
                if (tb8Bit->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_8BIT;
                }
                else if (tb16Bit->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_16BIT;
                }
                else if (tb24Bit->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_24BIT;
                }

                if (Render_Config.format == RNDR_FORMAT_WAVE)
                {
                    if (tb32Bit->getToggleState())
                    {
                        Render_Config.quality = RNDR_QUALITY_32BIT;
                    }
                    else if (tb32BitF->getToggleState())
                    {
                        Render_Config.quality = RNDR_QUALITY_FLOAT;
                    }
                }

                break;
            }

            case RNDR_FORMAT_OGG:
            {
                if (tb00->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_0;
                }
                else if (tb01->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_1;
                }
                else if (tb02->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_2;
                }
                else if (tb03->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_3;
                }
                else if (tb04->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_4;
                }
                else if (tb05->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_5;
                }
                else if (tb06->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_6;
                }
                else if (tb07->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_7;
                }
                else if (tb08->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_8;
                }
                else if (tb09->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_9;
                }
                else if (tb10->getToggleState())
                {
                    Render_Config.quality = RNDR_QUALITY_VORBIS_10;
                }

                break;
            }

            default:
                break;
        }

        String strFolder(textFolderStore->getText());
        String strExt;
        strFolder += this->fileName->getText();

        switch (Render_Config.format)
        {
            case RNDR_FORMAT_WAVE:
                {
                    strExt = ".wav";
                }break;
            case RNDR_FORMAT_FLAC:
                {
                    strExt = ".flac";
                }break;
            case RNDR_FORMAT_OGG:
                {
                    strExt = ".ogg";
                }break;
            default:
                strExt = "error";
                break;
        }

        if (!(strExt.containsIgnoreCase(T("error"))))
        {
            if (!(strFolder.endsWithIgnoreCase(strExt)))
            {
                strFolder += strExt;
            }
        }

        strFolder.copyToBuffer(Render_Config.file_name, RNDR_MAX_FILENAME_LENGTH);

        if(Handle_RenderStart() == true)
        {
            //MComp->listen->RedrawPerAction();

            pSlideTimer->startTimer(100);

            //Set button text to 'Abort'

            buttonThatWasClicked->setButtonText(T("Abort"));
        }
    }
    else if(buttonThatWasClicked->getButtonText() == T("Abort") )
    {
        pSlideTimer->stopTimer();

        Handle_RenderStop();

        slider->setValue(0, false, false);

        //Set button text to 'Start'
        buttonThatWasClicked->setButtonText(T("Start"));
    }
    else if(buttonThatWasClicked == foldButt )
    {
        //Let's show a dialog and allow user to choose a directory
        FileChooser *pDirSelector = new FileChooser(T("Select rendering directory"),
                                                     textFolderStore->getText(), String::empty, true);
        if (true == pDirSelector->browseForDirectory())
        {
            outdir = pDirSelector->getResult();
            String str = outdir.getFullPathName();
            textFolderStore->setText(str + "\\", false);

            // Remember selected folder in renderer settings
            String strFolder(textFolderStore->getText());
            Render_Config.output_dir = strFolder;
        }

        delete pDirSelector;
    }
    else if(buttonThatWasClicked == expButt)
    {
        HINSTANCE retval = ShellExecute(WinHWND, "open", outdir.getFullPathName(), NULL, NULL, SW_SHOWNORMAL);
    }
    else if(buttonThatWasClicked == resButt)
    {
        outdir = File(RENDERING_DEFAULT_FOLDER);
        String str = outdir.getFullPathName();
        textFolderStore->setText(str + "\\", false);

        // Remember selected folder in renderer settings
        String strFolder(textFolderStore->getText());
        Render_Config.output_dir = strFolder;
    }
    else if(buttonThatWasClicked == tb00)
        Render_Config.q2 = 0;
    else if(buttonThatWasClicked == tb01)
        Render_Config.q2 = 1;
    else if(buttonThatWasClicked == tb02)
        Render_Config.q2 = 2;
    else if(buttonThatWasClicked == tb03)
        Render_Config.q2 = 3;
    else if(buttonThatWasClicked == tb04)
        Render_Config.q2 = 4;
    else if(buttonThatWasClicked == tb05)
        Render_Config.q2 = 5;
    else if(buttonThatWasClicked == tb06)
        Render_Config.q2 = 6;
    else if(buttonThatWasClicked == tb07)
        Render_Config.q2 = 7;
    else if(buttonThatWasClicked == tb08)
        Render_Config.q2 = 8;
    else if(buttonThatWasClicked == tb09)
        Render_Config.q2 = 9;
    else if(buttonThatWasClicked == tb10)
        Render_Config.q2 = 10;
    else if(buttonThatWasClicked == tb8Bit)
        Render_Config.q1 = 0;
    else if(buttonThatWasClicked == tb16Bit)
        Render_Config.q1 = 1;
    else if(buttonThatWasClicked == tb24Bit)
        Render_Config.q1 = 2;
    else if(buttonThatWasClicked == tb32Bit)
        Render_Config.q1 = 3;
    else if(buttonThatWasClicked == tb32BitF)
        Render_Config.q1 = 4;
    else
    {
        if(this->tbWav == buttonThatWasClicked)
        {
            Render_Config.format = RNDR_FORMAT_WAVE;

            this->tb00->setVisible(false);
            this->tb01->setVisible(false);
            this->tb02->setVisible(false);
            this->tb03->setVisible(false);
            this->tb04->setVisible(false);
            this->tb05->setVisible(false);
            this->tb06->setVisible(false);
            this->tb07->setVisible(false);
            this->tb08->setVisible(false);
            this->tb09->setVisible(false);
            this->tb10->setVisible(false);

            this->tb8Bit->setVisible(true);
            this->tb16Bit->setVisible(true);
            this->tb24Bit->setVisible(true);
            this->tb32Bit->setVisible(true);
            this->tb32Bit->setEnabled(true);
            this->tb32BitF->setVisible(true);
            this->tb32BitF->setEnabled(true);
        }
        else if(this->tbOgg == buttonThatWasClicked)
        {
            Render_Config.format = RNDR_FORMAT_OGG;

            this->tb8Bit->setVisible(false);
            this->tb16Bit->setVisible(false);
            this->tb24Bit->setVisible(false);
            this->tb32Bit->setVisible(false);
            this->tb32BitF->setVisible(false);


            this->tb00->setVisible(true);
            this->tb01->setVisible(true);
            this->tb02->setVisible(true);
            this->tb03->setVisible(true);
            this->tb04->setVisible(true);
            this->tb05->setVisible(true);
            this->tb06->setVisible(true);
            this->tb07->setVisible(true);
            this->tb08->setVisible(true);
            this->tb09->setVisible(true);
            this->tb10->setVisible(true);
        }
        else if ( this->tbFlac == buttonThatWasClicked )
        {
            Render_Config.format = RNDR_FORMAT_FLAC;

            this->tb00->setVisible(false);
            this->tb01->setVisible(false);
            this->tb02->setVisible(false);
            this->tb03->setVisible(false);
            this->tb04->setVisible(false);
            this->tb05->setVisible(false);
            this->tb06->setVisible(false);
            this->tb07->setVisible(false);
            this->tb08->setVisible(false);
            this->tb09->setVisible(false);
            this->tb10->setVisible(false);

            this->tb8Bit->setVisible(true);
            this->tb16Bit->setVisible(true);
            this->tb24Bit->setVisible(true);
            this->tb32Bit->setVisible(true);
            this->tb32Bit->setEnabled(false);
            this->tb32BitF->setVisible(true);
            this->tb32BitF->setEnabled(false);

            if(this->tb32Bit->getToggleState() || this->tb32BitF->getToggleState())
                this->tb24Bit->setToggleState(true, false);
        }
    }
}

void RenderComponent::comboBoxChanged (AComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == this->int_comboBox)
    {
        Config_RenderInterpolation = comboBoxThatHasChanged->getSelectedId();
    }
}

void RenderComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
    if (editor.getName() == this->fileName->getName())
    {
        String strName = editor.getText();
        juce::tchar      wrong_chars[] = {'\\', '/', ';', ',', 0};

        if (strName.containsAnyOf(wrong_chars))
        {
            //this->fileName->clear();
        }
    }
}

void RenderComponent::textEditorTextChanged (TextEditor& editor)
{
    if (editor.getName() == this->fileName->getName())
    {
        String strName = editor.getText();
        juce::tchar      wrong_chars[] = {'\\', '/', ';', ',', 0};

        if (strName.containsAnyOf(wrong_chars))
        {
            //MessageBox(NULL, "Wrong name", "Error", MB_OK);
            this->fileName->setText(strName.removeCharacters(wrong_chars));
        }
    }
}






void AButton::paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    g.setColour(Colour(50, 64, 70));
    g.fillRect(0, 0, getWidth(), getHeight());

    g.setColour(Colour(112, 164, 149));
    g.setColour(Colour(112, 124, 129));
    g.fillRect(1, 1, getWidth() - 2, getHeight() - 2);


    //Button36::DrawButtonBase(g);

    if(!isButtonDown)
    {
        Colour clr = Colour(197, 223, 233);
        g.setColour(clr);
        //g.setColour(Colour(233, 233, 233));
        g.drawHorizontalLine(2, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.35f));
        g.drawHorizontalLine(3, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.25f));
        g.drawHorizontalLine(4, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.2f));
        g.drawHorizontalLine(5, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.15f));
        g.drawHorizontalLine(6, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.1f));
        g.drawHorizontalLine(7, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.08f));
        g.drawHorizontalLine(8, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.06f));
        g.drawHorizontalLine(9, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.05f));
        g.drawHorizontalLine(10, 2.f, float(getWidth() - 2));
        g.setColour(clr.withAlpha(0.04f));
        g.drawHorizontalLine(11, 2.f, float(getWidth() - 2));
        
        g.setColour(Colour(143, 159, 165));
        //g.setColour(Colour(137, 137, 137));
        g.drawHorizontalLine(getHeight() - 3, 2.f, float(getWidth() - 2));
    }
}

ATextButton::ATextButton(const String& name)
    : AButton (name)
{
}

ATextButton::ATextButton(const String& name, const String& toolTip)
    : AButton (name)
{
    setTooltip (toolTip);
}

void ATextButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    AButton::paintButton(g, isMouseOverButton, isButtonDown);

    drawText(g, *this, isButtonDown);
}

void ATextButton::drawText (Graphics& g, ATextButton& button, bool isButtonDown)
{
    g.setFont(*ins);

    if(isButtonDown == true)
    {
        g.setColour(Colour(255, 255, 255));
    }
    else
    {
        g.setColour(Colour(25, 35, 41));
    }

/*
    const int yIndent = jmin(4, button.proportionOfHeight (0.3f));
    const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = roundFloatToInt(g.getCurrentFont().getHeight() * 0.6f);
    const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

    g.drawFittedText(button.getButtonText(),
                     leftIndent,
                     yIndent,
                     button.getWidth() - leftIndent - rightIndent,
                     button.getHeight() - yIndent * 2,
                     Justification::centred, 2);
*/
    int tw = ins->getStringWidth(button.getButtonText());
    int tx = button.getWidth()/2 - tw/2;
    int ty = button.getHeight()/2 + (int)ins->getHeight()/2 - 2;
    g.drawSingleLineText(button.getButtonText(), tx, ty);
}


// We'll use a custom focus traverser here to make sure focus goes from the
// text editor to another component rather than back to the label itself.
class ALabelKeyboardFocusTraverser   : public KeyboardFocusTraverser
{
public:
    ALabelKeyboardFocusTraverser() {}

    Component* getNextComponent (Component* current)
    {
        return KeyboardFocusTraverser::getNextComponent (dynamic_cast <TextEditor*> (current) != 0
                                                            ? current->getParentComponent() : current);
    }

    Component* getPreviousComponent (Component* current)
    {
        return KeyboardFocusTraverser::getPreviousComponent (dynamic_cast <TextEditor*> (current) != 0
                                                                ? current->getParentComponent() : current);
    }
};

ALabel::ALabel (const String& componentName,
              const String& labelText)
    : Component (componentName),
      text (labelText),
      font (15.0f),
      justification (Justification::centredLeft),
      editor (0),
      ownerComponent (0),
      deletionWatcher (0),
      editSingleClick (false),
      editDoubleClick (false),
      lossOfFocusDiscardsChanges (false)
{
    setColour (TextEditor::textColourId, Colours::black);
    setColour (TextEditor::backgroundColourId, Colours::transparentBlack);
    setColour (TextEditor::outlineColourId, Colours::transparentBlack);
}

ALabel::~ALabel()
{
    if (ownerComponent != 0 && ! deletionWatcher->hasBeenDeleted())
        ownerComponent->removeComponentListener (this);

    deleteAndZero (deletionWatcher);

    if (editor != 0)
        delete editor;
}

void ALabel::setText (const String& newText,
                     const bool broadcastChangeMessage)
{
    hideEditor (true);

    if (text != newText)
    {
        text = newText;

        if (broadcastChangeMessage)
            triggerAsyncUpdate();

        repaint();

        if (ownerComponent != 0 && ! deletionWatcher->hasBeenDeleted())
            componentMovedOrResized (*ownerComponent, true, true);
    }
}

const String ALabel::getText (const bool returnActiveEditorContents) const throw()
{
    return (returnActiveEditorContents && isBeingEdited())
                ? editor->getText()
                : text;
}

void ALabel::setFont (const Font& newFont) throw()
{
    font = newFont;
    repaint();
}

const Font& ALabel::getFont() const throw()
{
    return font;
}

void ALabel::setEditable (const bool editOnSingleClick,
                         const bool editOnDoubleClick,
                         const bool lossOfFocusDiscardsChanges_) throw()
{
    editSingleClick = editOnSingleClick;
    editDoubleClick = editOnDoubleClick;
    lossOfFocusDiscardsChanges = lossOfFocusDiscardsChanges_;

    setWantsKeyboardFocus (editOnSingleClick || editOnDoubleClick);
    setFocusContainer (editOnSingleClick || editOnDoubleClick);
}

void ALabel::setJustificationType (const Justification& justification_) throw()
{
    justification = justification_;
    repaint();
}

void ALabel::attachToComponent (Component* owner,
                               const bool onLeft)
{
    if (ownerComponent != 0 && ! deletionWatcher->hasBeenDeleted())
        ownerComponent->removeComponentListener (this);

    deleteAndZero (deletionWatcher);
    ownerComponent = owner;

    leftOfOwnerComp = onLeft;

    if (ownerComponent != 0)
    {
        deletionWatcher = new ComponentDeletionWatcher (owner);

        setVisible (owner->isVisible());
        ownerComponent->addComponentListener (this);
        componentParentHierarchyChanged (*ownerComponent);
        componentMovedOrResized (*ownerComponent, true, true);
    }
}

void ALabel::componentMovedOrResized (Component& component,
                                     bool /*wasMoved*/,
                                     bool /*wasResized*/)
{
    if (leftOfOwnerComp)
    {
        setSize (jmin (getFont().getStringWidth (text) + 8, component.getX()),
                 component.getHeight());

        setTopRightPosition (component.getX(), component.getY());
    }
    else
    {
        setSize (component.getWidth(),
                 8 + roundFloatToInt (getFont().getHeight()));

        setTopLeftPosition (component.getX(), component.getY() - getHeight());
    }
}

void ALabel::componentParentHierarchyChanged (Component& component)
{
    if (component.getParentComponent() != 0)
        component.getParentComponent()->addChildComponent (this);
}

void ALabel::componentVisibilityChanged (Component& component)
{
    setVisible (component.isVisible());
}

void ALabel::textWasEdited()
{
}

void ALabel::showEditor()
{
    if (editor == 0)
    {
        addAndMakeVisible(editor = createEditorComponent());
        editor->setText(getText());
        editor->addListener(this);
        editor->grabKeyboardFocus();
        editor->setHighlightedRegion(0, text.length());
        editor->addListener(this);

        resized();
        repaint();

        enterModalState();
        editor->grabKeyboardFocus();
    }
}

bool ALabel::updateFromTextEditorContents()
{
    jassert (editor != 0);
    const String newText (editor->getText());

    if (text != newText)
    {
        text = newText;

        triggerAsyncUpdate();
        repaint();

        if (ownerComponent != 0 && ! deletionWatcher->hasBeenDeleted())
            componentMovedOrResized (*ownerComponent, true, true);

        return true;
    }

    return false;
}

void ALabel::hideEditor (const bool discardCurrentEditorContents)
{
    if (editor != 0)
    {
        const bool changed = (! discardCurrentEditorContents)
                               && updateFromTextEditorContents();

        deleteAndZero (editor);
        repaint();

        if (changed)
            textWasEdited();

        exitModalState (0);
    }
}

void ALabel::inputAttemptWhenModal()
{
    if (editor != 0)
    {
        if (lossOfFocusDiscardsChanges)
            textEditorEscapeKeyPressed (*editor);
        else
            textEditorReturnKeyPressed (*editor);
    }
}

bool ALabel::isBeingEdited() const throw()
{
    return editor != 0;
}

TextEditor* ALabel::createEditorComponent()
{
    TextEditor* const ed = new TextEditor(getName());
    ed->setFont (*ins);

    // copy these colours from our own settings..
    const int cols[] = { TextEditor::backgroundColourId,
                         TextEditor::textColourId,
                         TextEditor::highlightColourId,
                         TextEditor::highlightedTextColourId,
                         TextEditor::caretColourId,
                         TextEditor::outlineColourId,
                         TextEditor::focusedOutlineColourId,
                         TextEditor::shadowColourId };

    for (int i = 0; i < numElementsInArray (cols); ++i)
        ed->setColour(cols[i], findColour (cols[i]));

    ed->setColour(TextEditor::textColourId, Colour(217, 217, 217));
    return ed;
}

void ALabel::paint (Graphics& g)
{
    //g.fillAll (Colour(33, 86, 75));

    if(editor == 0)
    {
        const float alpha = isEnabled() ? 1.0f : 0.5f;

        g.setColour(findColour(textColourId, false));
        g.setFont(getFont());
        g.drawSingleLineText(text, 2, (int)getFont().getHeight() + 2);

        //g.setColour (Colour(12, 56, 47));
        //g.drawRect (0, 0, getWidth(), getHeight());
    }
    else if(isEnabled())
    {
        g.setColour(editor->findColour (TextEditor::backgroundColourId)
                        .overlaidWith (findColour (outlineColourId)));

        //g.drawRect(0, 0, getWidth(), getHeight());
    }
}

void ALabel::mouseUp (const MouseEvent& e)
{
    if (editSingleClick
         && e.mouseWasClicked()
         && contains (e.x, e.y)
         && ! e.mods.isPopupMenu())
    {
        showEditor();
    }
}

void ALabel::mouseDoubleClick (const MouseEvent& e)
{
    if (editDoubleClick && ! e.mods.isPopupMenu())
        showEditor();
}

void ALabel::resized()
{
    if (editor != 0)
    {
        editor->setBounds(0, 0, getWidth(), getHeight());
    }
}

void ALabel::focusGained (FocusChangeType cause)
{
    if (editSingleClick && cause == focusChangedByTabKey)
        showEditor();
}

void ALabel::enablementChanged()
{
    repaint();
}

void ALabel::colourChanged()
{
    repaint();
}

KeyboardFocusTraverser* ALabel::createFocusTraverser()
{
    return new ALabelKeyboardFocusTraverser();
}

void ALabel::addListener (ALabelListener* const listener) throw()
{
    jassert (listener != 0);
    if (listener != 0)
        listeners.add (listener);
}

void ALabel::removeListener (ALabelListener* const listener) throw()
{
    listeners.removeValue (listener);
}

void ALabel::handleAsyncUpdate()
{
    for (int i = listeners.size(); --i >= 0;)
    {
        ((ALabelListener*) listeners.getUnchecked (i))->labelTextChanged (this);
        i = jmin (i, listeners.size());
    }
}

void ALabel::textEditorTextChanged (TextEditor& ed)
{
    if (editor != 0)
    {
        jassert (&ed == editor);

        if (! (hasKeyboardFocus (true) || isCurrentlyBlockedByAnotherModalComponent()))
        {
            if (lossOfFocusDiscardsChanges)
                textEditorEscapeKeyPressed (ed);
            else
                textEditorReturnKeyPressed (ed);
        }
    }
}

void ALabel::textEditorReturnKeyPressed (TextEditor& ed)
{
    if (editor != 0)
    {
        jassert (&ed == editor);
        (void) ed;

        const bool changed = updateFromTextEditorContents();
        hideEditor (true);

        if (changed)
            textWasEdited();
    }
}

void ALabel::textEditorEscapeKeyPressed (TextEditor& ed)
{
    if (editor != 0)
    {
        jassert (&ed == editor);
        (void) ed;

        editor->setText (text, false);
        hideEditor (true);
    }
}

void ALabel::textEditorFocusLost (TextEditor& ed)
{
    textEditorTextChanged (ed);
}

AComboBox::AComboBox (const String& name)
    : Component(name),
      currentIndex(-1),
      isButtonDown(false),
      separatorPending(false),
      menuActive(false),
      label(0)
{
    noChoicesMessage = TRANS("(no choices)");
    setRepaintsOnMouseActivity (true);

    lbx = 2;
    lby = 2;
    lookAndFeelChanged();
}

AComboBox::~AComboBox()
{
    if (menuActive)
        PopupMenu::dismissAllActiveMenus();

    deleteAllChildren();
}

void AComboBox::setEditableText (const bool isEditable)
{
    label->setEditable (isEditable, isEditable, false);

    setWantsKeyboardFocus (! isEditable);
    resized();
}

bool AComboBox::isTextEditable() const throw()
{
    return label->isEditable();
}

void AComboBox::setJustificationType (const Justification& justification) throw()
{
    label->setJustificationType (justification);
}

const Justification AComboBox::getJustificationType() const throw()
{
    return label->getJustificationType();
}

void AComboBox::setTooltip (const String& newTooltip)
{
    SettableTooltipClient::setTooltip (newTooltip);
    label->setTooltip (newTooltip);
}

void AComboBox::addItem (const String& newItemText,
                        const int newItemId) throw()
{
    // you can't add empty strings to the list..
    jassert (newItemText.isNotEmpty());

    // IDs must be non-zero, as zero is used to indicate a lack of selecion.
    jassert (newItemId != 0);

    // you shouldn't use duplicate item IDs!
    jassert (getItemForId (newItemId) == 0);

    if (newItemText.isNotEmpty() && newItemId != 0)
    {
        if (separatorPending)
        {
            separatorPending = false;

            ItemInfo* const item = new ItemInfo();
            item->itemId = 0;
            item->isEnabled = false;
            item->isHeading = false;
            items.add (item);
        }

        ItemInfo* const item = new ItemInfo();
        item->name = newItemText;
        item->itemId = newItemId;
        item->isEnabled = true;
        item->isHeading = false;
        items.add (item);
    }
}

void AComboBox::addSeparator() throw()
{
    separatorPending = (items.size() > 0);
}

void AComboBox::addSectionHeading (const String& headingName) throw()
{
    // you can't add empty strings to the list..
    jassert (headingName.isNotEmpty());

    if (headingName.isNotEmpty())
    {
        if (separatorPending)
        {
            separatorPending = false;

            ItemInfo* const item = new ItemInfo();
            item->itemId = 0;
            item->isEnabled = false;
            item->isHeading = false;
            items.add (item);
        }

        ItemInfo* const item = new ItemInfo();
        item->name = headingName;
        item->itemId = 0;
        item->isEnabled = true;
        item->isHeading = true;
        items.add (item);
    }
}

void AComboBox::setItemEnabled (const int itemId,
                               const bool isEnabled) throw()
{
    ItemInfo* const item = getItemForId (itemId);

    if (item != 0)
        item->isEnabled = isEnabled;
}

void AComboBox::changeItemText (const int itemId,
                               const String& newText) throw()
{
    ItemInfo* const item = getItemForId (itemId);

    jassert (item != 0);

    if (item != 0)
        item->name = newText;
}

void AComboBox::clear (const bool dontSendChangeMessage)
{
    items.clear();
    separatorPending = false;

    if (! label->isEditable())
        setSelectedItemIndex (-1, dontSendChangeMessage);
}

AComboBox::ItemInfo* AComboBox::getItemForId (const int itemId) const throw()
{
    jassert (itemId != 0);

    if (itemId != 0)
    {
        for (int i = items.size(); --i >= 0;)
            if (items.getUnchecked(i)->itemId == itemId)
                return items.getUnchecked(i);
    }

    return 0;
}

AComboBox::ItemInfo* AComboBox::getItemForIndex (const int index) const throw()
{
    int n = 0;

    for (int i = 0; i < items.size(); ++i)
    {
        ItemInfo* const item = items.getUnchecked(i);

        if (item->isRealItem())
        {
            if (n++ == index)
                return item;
        }
    }

    return 0;
}

int AComboBox::getNumItems() const throw()
{
    int n = 0;

    for (int i = items.size(); --i >= 0;)
    {
        ItemInfo* const item = items.getUnchecked(i);

        if (item->isRealItem())
            ++n;
    }

    return n;
}

const String AComboBox::getItemText (const int index) const throw()
{
    ItemInfo* const item = getItemForIndex (index);

    if (item != 0)
        return item->name;

    return String::empty;
}

int AComboBox::getItemId (const int index) const throw()
{
    ItemInfo* const item = getItemForIndex (index);

    return (item != 0) ? item->itemId : 0;
}

bool AComboBox::ItemInfo::isSeparator() const throw()
{
    return name.isEmpty();
}

bool AComboBox::ItemInfo::isRealItem() const throw()
{
    return ! (isHeading || name.isEmpty());
}

int AComboBox::getSelectedItemIndex() const throw()
{
    return (currentIndex >= 0 && getText() == getItemText (currentIndex))
                ? currentIndex
                : -1;
}

void AComboBox::setSelectedItemIndex (const int index,
                                     const bool dontSendChangeMessage) throw()
{
    if (currentIndex != index || label->getText() != getItemText (currentIndex))
    {
        if (((unsigned int) index) < (unsigned int) getNumItems())
            currentIndex = index;
        else
            currentIndex = -1;

        label->setText (getItemText (currentIndex), false);

        if (! dontSendChangeMessage)
            triggerAsyncUpdate();
    }
}

void AComboBox::setSelectedId (const int newItemId,
                              const bool dontSendChangeMessage) throw()
{
    for (int i = getNumItems(); --i >= 0;)
    {
        if (getItemId(i) == newItemId)
        {
            setSelectedItemIndex (i, dontSendChangeMessage);
            break;
        }
    }
}

int AComboBox::getSelectedId() const throw()
{
    const ItemInfo* const item = getItemForIndex (currentIndex);

    return (item != 0 && getText() == item->name)
                ? item->itemId
                : 0;
}

void AComboBox::addListener(AComboBoxListener* const listener) throw()
{
    jassert (listener != 0);
    if (listener != 0)
        listeners.add (listener);
}

void AComboBox::removeListener(AComboBoxListener* const listener) throw()
{
    listeners.removeValue (listener);
}

void AComboBox::handleAsyncUpdate()
{
    for (int i = listeners.size(); --i >= 0;)
    {
       ((AComboBoxListener*) listeners.getUnchecked (i))->comboBoxChanged (this);
        i = jmin (i, listeners.size());
    }
}

const String AComboBox::getText() const throw()
{
    return label->getText();
}

void AComboBox::setText (const String& newText,
                        const bool dontSendChangeMessage) throw()
{
    for (int i = items.size(); --i >= 0;)
    {
        ItemInfo* const item = items.getUnchecked(i);

        if (item->isRealItem()
             && item->name == newText)
        {
            setSelectedId (item->itemId, dontSendChangeMessage);
            return;
        }
    }

    currentIndex = -1;

    if (label->getText() != newText)
    {
        label->setText (newText, false);

        if (! dontSendChangeMessage)
            triggerAsyncUpdate();
    }

    repaint();
}

void AComboBox::showEditor()
{
    jassert (isTextEditable()); // you probably shouldn't do this to a non-editable combo box?

    resized();
    label->showEditor();
}

void AComboBox::setTextWhenNothingSelected (const String& newMessage) throw()
{
    textWhenNothingSelected = newMessage;
    repaint();
}

const String AComboBox::getTextWhenNothingSelected() const throw()
{
    return textWhenNothingSelected;
}

void AComboBox::setTextWhenNoChoicesAvailable (const String& newMessage) throw()
{
    noChoicesMessage = newMessage;
}

const String AComboBox::getTextWhenNoChoicesAvailable() const throw()
{
    return noChoicesMessage;
}

void AComboBox::paint (Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    int buttonX = label->getRight();
    int buttonY = 0;
    int buttonW = getWidth() - label->getRight();
    int buttonH = getHeight();

    //g.fillAll(Colour(5, 66, 57));
    g.fillAll(Colour(45, 55, 59));

    /*
    if (isEnabled() && hasKeyboardFocus (false))
    {
        g.setColour (findColour (TextButton::buttonColourId));
        g.drawRect (0, 0, width, height, 2);
    }
    else*/
    {
        //g.setColour (Colour(36, 158, 133));
        g.setColour (Colour(133, 147, 152));
        g.drawRect (0, 0, width, height);
    }

    g.setColour(Colour(63, 73, 77));
    g.drawRect(buttonX + 1, buttonY + 1, buttonW - 2, buttonH - 2);

    g.setColour(Colour(132, 149, 154));
    g.fillRect(buttonX + 2, buttonY + 2, buttonW - 4, buttonH - 4);

    g.setColour(Colour(197, 228, 233));
    g.fillRect(buttonX + 2, buttonY + 2, buttonW - 4, 1);
    
    g.setColour(Colour(111, 121, 125));
    g.fillRect(buttonX + 2, buttonH - 3, buttonW - 4, 1);

    if(isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;

        Path p;
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);

        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);

        g.setColour(Colour(37, 46, 48));
        g.fillPath (p);
    }

    if(textWhenNothingSelected.isNotEmpty()
        && label->getText().isEmpty()
        && ! label->isBeingEdited())
    {
        g.setColour (findColour (textColourId).withMultipliedAlpha (0.5f));
        g.setFont (label->getFont());
        g.drawFittedText (textWhenNothingSelected,
                          label->getX() + 2, label->getY() + 1,
                          label->getWidth() - 4, label->getHeight() - 2,
                          label->getJustificationType(),
                          jmax (1, (int) (label->getHeight() / label->getFont().getHeight())));
    }
}

void AComboBox::resized()
{
    if(getHeight() > 0 && getWidth() > 0)
    {
        label->setBounds(lbx, lby, getWidth() - getHeight() - 2, getHeight() - 2);
    }
}

void AComboBox::enablementChanged()
{
    repaint();
}

void AComboBox::lookAndFeelChanged()
{
    repaint();

    ALabel* const newLabel = new ALabel(String::empty, String::empty);

    if (label != 0)
    {
        newLabel->setEditable (label->isEditable());
        newLabel->setJustificationType (label->getJustificationType());
        newLabel->setTooltip (label->getTooltip());
        newLabel->setText (label->getText(), false);
    }

    delete label;

    label = newLabel;

    addAndMakeVisible (newLabel);

    newLabel->setFont (*ins);
    newLabel->addListener (this);
    newLabel->addMouseListener (this, false);

    newLabel->setColour(ALabel::backgroundColourId, Colours::transparentBlack);
    newLabel->setColour(ALabel::textColourId, Colour(166, 175, 182));

    newLabel->setColour(TextEditor::textColourId, findColour (AComboBox::textColourId));
    newLabel->setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
    newLabel->setColour(TextEditor::highlightColourId, findColour (TextEditor::highlightColourId));
    newLabel->setColour(TextEditor::outlineColourId, Colours::transparentBlack);

    resized();
}

void AComboBox::colourChanged()
{
    lookAndFeelChanged();
}

bool AComboBox::keyPressed (const KeyPress& key)
{
    bool used = false;

    if (key.isKeyCode (KeyPress::upKey)
        || key.isKeyCode (KeyPress::leftKey))
    {
        setSelectedItemIndex (jmax (0, currentIndex - 1));
        used = true;
    }
    else if (key.isKeyCode (KeyPress::downKey)
              || key.isKeyCode (KeyPress::rightKey))
    {
        setSelectedItemIndex (jmin (currentIndex + 1, getNumItems() - 1));
        used = true;
    }
    else if (key.isKeyCode (KeyPress::returnKey))
    {
        showPopup();
        used = true;
    }

    return used;
}

bool AComboBox::keyStateChanged()
{
    // only forward key events that aren't used by this component
    return KeyPress::isKeyCurrentlyDown (KeyPress::upKey)
        || KeyPress::isKeyCurrentlyDown (KeyPress::leftKey)
        || KeyPress::isKeyCurrentlyDown (KeyPress::downKey)
        || KeyPress::isKeyCurrentlyDown (KeyPress::rightKey);
}

void AComboBox::focusGained (FocusChangeType)
{
    repaint();
}

void AComboBox::focusLost (FocusChangeType)
{
    repaint();
}

void AComboBox::labelTextChanged (ALabel*)
{
    triggerAsyncUpdate();
}

void AComboBox::showPopup()
{
    if(!menuActive)
    {
        const int currentId = getSelectedId();
        ComponentDeletionWatcher deletionWatcher (this);

        PopupMenu menu;

        menu.setLookAndFeel (&getLookAndFeel());

        for(int i = 0; i < items.size(); ++i)
        {
            const ItemInfo* const item = items.getUnchecked(i);

            if (item->isSeparator())
                menu.addSeparator();
            else if (item->isHeading)
                menu.addSectionHeader (item->name);
            else
                menu.addItem (item->itemId, item->name, item->isEnabled, item->itemId == currentId);
        }

        if(items.size() == 0)
            menu.addItem (1, noChoicesMessage, false);

        const int itemHeight = jlimit (12, 17, getHeight());

        menuActive = true;
        const int resultId = menu.showAt(this, currentId, getWidth(), 1, itemHeight);

        if(deletionWatcher.hasBeenDeleted())
            return;

        menuActive = false;

        if(resultId != 0)
            setSelectedId (resultId);
    }
}

void AComboBox::mouseDown (const MouseEvent& e)
{
    beginDragAutoRepeat (300);

    isButtonDown = isEnabled();

    if (isButtonDown
         && (e.eventComponent == this || ! label->isEditable()))
    {
        showPopup();
    }
}

void AComboBox::mouseDrag (const MouseEvent& e)
{
    beginDragAutoRepeat (50);

    if (isButtonDown && ! e.mouseWasClicked())
        showPopup();
}

void AComboBox::mouseUp (const MouseEvent& e2)
{
    if (isButtonDown)
    {
        isButtonDown = false;

        repaint();

        const MouseEvent e (e2.getEventRelativeTo (this));

        if (reallyContains (e.x, e.y, true)
             && (e2.eventComponent == this || ! label->isEditable()))
        {
            showPopup();
        }
    }
}

AToggleButton::AToggleButton (const String& buttonText)
    : Button (buttonText)
{
    setClickingTogglesState (true);
    setType(0);
}

AToggleButton::~AToggleButton()
{
}

void AToggleButton::paintButton (Graphics& g,
                                   bool isMouseOverButton,
                                   bool isButtonDown)
{
    int tkWidth = jmin (20, getHeight() - 4);

    drawTickBox (g, 4, -4,
                 tkWidth, tkWidth,
                 getToggleState(),
                 isEnabled(),
                 isMouseOverButton,
                 isButtonDown,
                 getType());

    int textX = tkWidth + 5;
    int textY = 0;

    g.setFont(*ins);

    if(getType() == 3)
    {
        textX = tkWidth - 3;
        textY = -3;

        g.setFont(*ti);
    }
    else if(getType() == 1)
    {
        textX = tkWidth + 2;
    }

/*
    g.setColour(Colour(16, 48, 42));
    g.drawFittedText(getButtonText(),
                     textX + 1, textY,
                     getWidth() - textX, getHeight() - 8,
                     Justification::centredLeft, 10);
*/

    g.setColour(findColour(ToggleButton::textColourId));

    //g.setFont (jmin (15.0f, button.getHeight() * 0.6f));
    //g.setColour(Colour(190, 255, 190));

    g.setColour(Colour(145, 155, 159));

    if (!isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText(getButtonText(),
                     textX, textY - 1,
                     getWidth() - textX, getHeight() - 8,
                     Justification::centredLeft, 10);
}

void AToggleButton::changeWidthToFitText()
{
    const int tkWidth = jmin (24, getHeight());

    setSize (ins->getStringWidth(getButtonText()) + tkWidth + 8, getHeight());
}

void AToggleButton::colourChanged()
{
    repaint();
}

void AToggleButton::setType(int newtype)
{
    tgtype = newtype;
}

int AToggleButton::getType()
{
    return tgtype;
}

class ASliderPopupDisplayComponent  : public BubbleComponent
{
public:

    ASliderPopupDisplayComponent(ASlider* const owner_)
        : owner (owner_),
          font (15.0f, Font::bold)
    {
        setAlwaysOnTop (true);
    }

    ~ASliderPopupDisplayComponent()
    {
    }

    void paintContent (Graphics& g, int w, int h)
    {
        g.setFont (font);
        g.setColour (Colours::black);

        g.drawFittedText (text, 0, 0, w, h, Justification::centred, 1);
    }

    void getContentSize (int& w, int& h)
    {
        w = font.getStringWidth (text) + 18;
        h = (int) (font.getHeight() * 1.6f);
    }

    void updatePosition (const String& newText)
    {
        if (text != newText)
        {
            text = newText;
            repaint();
        }

        BubbleComponent::setPosition (owner);
    }

    juce_UseDebuggingNewOperator

private:
    ASlider* owner;
    Font font;
    String text;

    ASliderPopupDisplayComponent (const ASliderPopupDisplayComponent&);
    const ASliderPopupDisplayComponent& operator= (const ASliderPopupDisplayComponent&);
};

ASlider::ASlider(const String& name)
  : Component (name),
    currentValue (0.0),
    valueMin (0.0),
    valueMax (0.0),
    minimum (0),
    maximum (10),
    interval (0),
    param (0),
    skewFactor (1.0),
    velocityModeSensitivity (1.0),
    velocityModeOffset (0.0),
    velocityModeThreshold (1),
    rotaryStart (float_Pi * 1.2f),
    rotaryEnd (float_Pi * 2.8f),
    numDecimalPlaces (7),
    sliderRegionStart (0),
    sliderRegionSize (1),
    pixelsForFullDragExtent (250),
    style (LinearHorizontal),
    textBoxPos (TextBoxLeft),
    textBoxWidth (80),
    textBoxHeight (20),
    textBoxX(0),
    textBoxY(0),
    incDecButtonMode (incDecButtonsNotDraggable),
    editableText (true),
    doubleClickToValue (false),
    isVelocityBased (false),
    userKeyOverridesVelocity (true),
    rotaryStop (true),
    incDecButtonsSideBySide (false),
    sendChangeOnlyOnRelease (false),
    popupDisplayEnabled (false),
    menuEnabled (false),
    menuShown (false),
    scrollWheelEnabled (true),
    snapsToMousePos (true),
    valueBox (0),
    incButton (0),
    decButton (0),
    popupDisplay (0),
    parentForPopupDisplay(0),
    paramIndex(0)
{
    setWantsKeyboardFocus (false);
    setRepaintsOnMouseActivity (true);
    setSigned(false);

    lookAndFeelChanged();
    updateText();
}

ASlider::~ASlider()
{
    deleteAndZero (popupDisplay);
    deleteAllChildren();
}

void ASlider::setSigned(bool sgn)
{
    is_signed = sgn;
}

bool ASlider::getSigned()
{
    return is_signed;
}

void ASlider::handleAsyncUpdate()
{
    cancelPendingUpdate();

    for (int i = listeners.size(); --i >= 0;)
    {
        ((ASliderListener*) listeners.getUnchecked (i))->sliderValueChanged (this);
        i = jmin (i, listeners.size());
    }
}

void ASlider::sendDragStart()
{
    startedDragging();

    for (int i = listeners.size(); --i >= 0;)
    {
        ((ASliderListener*) listeners.getUnchecked (i))->sliderDragStarted (this);
        i = jmin (i, listeners.size());
    }
}

void ASlider::sendDragEnd()
{
    stoppedDragging();

    for (int i = listeners.size(); --i >= 0;)
    {
        ((ASliderListener*) listeners.getUnchecked (i))->sliderDragEnded (this);
        i = jmin (i, listeners.size());
    }
}

void ASlider::addListener (ASliderListener* const listener) throw()
{
    jassert (listener != 0);
    if (listener != 0)
        listeners.add (listener);
}

void ASlider::removeListener (ASliderListener* const listener) throw()
{
    listeners.removeValue (listener);
}

void ASlider::setSliderStyle (const SliderStyle newStyle)
{
    if (style != newStyle)
    {
        style = newStyle;
        repaint();
        lookAndFeelChanged();
    }
}

void ASlider::setRotaryParameters(const float startAngleRadians,
                                       const float endAngleRadians,
                                       const bool stopAtEnd)
{
    // make sure the values are sensible..
    jassert (rotaryStart >= 0 && rotaryEnd >= 0);
    jassert (rotaryStart < float_Pi * 4.0f && rotaryEnd < float_Pi * 4.0f);
    jassert (rotaryStart < rotaryEnd);

    rotaryStart = startAngleRadians;
    rotaryEnd = endAngleRadians;
    rotaryStop = stopAtEnd;
}

void ASlider::setVelocityBasedMode(const bool velBased) throw()
{
    isVelocityBased = velBased;
}

void ASlider::setVelocityModeParameters(const double sensitivity,
                                              const int threshold,
                                              const double offset,
                                              const bool userCanPressKeyToSwapMode) throw()
{
    jassert (threshold >= 0);
    jassert (sensitivity > 0);
    jassert (offset >= 0);

    velocityModeSensitivity = sensitivity;
    velocityModeOffset = offset;
    velocityModeThreshold = threshold;
    userKeyOverridesVelocity = userCanPressKeyToSwapMode;
}

void ASlider::setSkewFactor (const double factor) throw()
{
    skewFactor = factor;
}

void ASlider::setSkewFactorFromMidPoint (const double sliderValueToShowAtMidPoint) throw()
{
    if (maximum > minimum)
        skewFactor = log (0.5) / log ((sliderValueToShowAtMidPoint - minimum)
                                        / (maximum - minimum));
}

void ASlider::setMouseDragSensitivity (const int distanceForFullScaleDrag)
{
    jassert (distanceForFullScaleDrag > 0);

    pixelsForFullDragExtent = distanceForFullScaleDrag;
}

void ASlider::setIncDecButtonsMode (const IncDecButtonMode mode)
{
    if (incDecButtonMode != mode)
    {
        incDecButtonMode = mode;
        lookAndFeelChanged();
    }
}

void ASlider::setTextBoxStyle (const TextEntryBoxPosition newPosition,
                              const bool isReadOnly,
                              const int textEntryBoxWidth,
                              const int textEntryBoxHeight)
{
    textBoxPos = newPosition;
    editableText = ! isReadOnly;
    textBoxWidth = textEntryBoxWidth;
    textBoxHeight = textEntryBoxHeight;

    repaint();
    lookAndFeelChanged();
}

void ASlider::setTextBoxIsEditable (const bool shouldBeEditable) throw()
{
    editableText = shouldBeEditable;

    if (valueBox != 0)
    {
        valueBox->setEditable (shouldBeEditable && isEnabled());
        if(shouldBeEditable)
        {
            valueBox->setInterceptsMouseClicks(true, true);
        }
        else
        {
            valueBox->setInterceptsMouseClicks(false, false);
        }
    }
}

void ASlider::showTextBox()
{
    jassert (editableText); // this should probably be avoided in read-only sliders.

    if (valueBox != 0)
        valueBox->showEditor();
}

void ASlider::hideTextBox (const bool discardCurrentEditorContents)
{
    if (valueBox != 0)
    {
        valueBox->hideEditor (discardCurrentEditorContents);

        if (discardCurrentEditorContents)
            updateText();
    }
}

void ASlider::setChangeNotificationOnlyOnRelease (const bool onlyNotifyOnRelease) throw()
{
    sendChangeOnlyOnRelease = onlyNotifyOnRelease;
}

void ASlider::setSliderSnapsToMousePosition (const bool shouldSnapToMouse) throw()
{
    snapsToMousePos = shouldSnapToMouse;
}

void ASlider::setPopupDisplayEnabled (const bool enabled,
                                     Component* const parentComponentToUse) throw()
{
    popupDisplayEnabled = enabled;
    parentForPopupDisplay = parentComponentToUse;
}

void ASlider::colourChanged()
{
    lookAndFeelChanged();
}

void ASlider::lookAndFeelChanged()
{
    const String previousTextBoxContent (valueBox != 0 ? valueBox->getText()
                                                       : getTextFromValue (currentValue));

    deleteAllChildren();
    valueBox = 0;

    LookAndFeel& lf = getLookAndFeel();

    if (textBoxPos != NoTextBox)
    {
        addAndMakeVisible(valueBox = createSliderTextBox(*this));

        valueBox->setWantsKeyboardFocus (false);
        valueBox->setText (previousTextBoxContent, false);

        valueBox->setEditable(editableText && isEnabled());
        valueBox->addListener(this);

        if (style == LinearBar)
            valueBox->addMouseListener (this, false);
    }

    if (style == IncDecButtons)
    {
        addAndMakeVisible (incButton = lf.createSliderButton (true));
        incButton->addButtonListener (this);

        addAndMakeVisible (decButton = lf.createSliderButton (false));
        decButton->addButtonListener (this);

        if (incDecButtonMode != incDecButtonsNotDraggable)
        {
            incButton->addMouseListener (this, false);
            decButton->addMouseListener (this, false);
        }
        else
        {
            incButton->setRepeatSpeed (300, 100, 20);
            incButton->addMouseListener (decButton, false);

            decButton->setRepeatSpeed (300, 100, 20);
            decButton->addMouseListener (incButton, false);
        }
    }

    resized();
    repaint();
}

void ASlider::setRange (const double newMin,
                       const double newMax,
                       const double newInt)
{
    if (minimum != newMin
        || maximum != newMax
        || interval != newInt)
    {
        minimum = newMin;
        maximum = newMax;
        interval = newInt;

        // figure out the number of DPs needed to display all values at this
        // interval setting.
        numDecimalPlaces = 7;

        if (newInt != 0)
        {
            int v = abs ((int) (newInt * 10000000));

            while ((v % 10) == 0)
            {
                --numDecimalPlaces;
                v /= 10;
            }
        }

        // keep the current values inside the new range..
        if (style != TwoValueHorizontal && style != TwoValueVertical)
        {
            setValue (currentValue, false, false);
        }
        else
        {
            setMinValue (getMinValue(), false, false);
            setMaxValue (getMaxValue(), false, false);
        }

        updateText();
    }
}

void ASlider::triggerChangeMessage (const bool synchronous)
{
    if (synchronous)
        handleAsyncUpdate();
    else
        triggerAsyncUpdate();

    valueChanged();
}

double ASlider::getValue() const throw()
{
    // for a two-value style slider, you should use the getMinValue() and getMaxValue()
    // methods to get the two values.
    jassert (style != TwoValueHorizontal && style != TwoValueVertical);

    return currentValue;
}

void ASlider::setValue (double newValue,
                       const bool sendUpdateMessage,
                       const bool sendMessageSynchronously)
{
    // for a two-value style slider, you should use the setMinValue() and setMaxValue()
    // methods to set the two values.
    jassert (style != TwoValueHorizontal && style != TwoValueVertical);

    newValue = constrainedValue (newValue);

    if (style == ThreeValueHorizontal || style == ThreeValueVertical)
    {
        jassert (valueMin <= valueMax);
        newValue = jlimit (valueMin, valueMax, newValue);
    }

    if (currentValue != newValue)
    {
        if (valueBox != 0)
            valueBox->hideEditor (true);

        currentValue = newValue;
        updateText();
        repaint();

        if (popupDisplay != 0)
        {
            ((ASliderPopupDisplayComponent*) popupDisplay)->updatePosition (getTextFromValue (currentValue));
            popupDisplay->repaint();
        }

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

double ASlider::getMinValue() const throw()
{
    // The minimum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    return valueMin;
}

double ASlider::getMaxValue() const throw()
{
    // The maximum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    return valueMax;
}

void ASlider::setMinValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously)
{
    // The minimum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    newValue = constrainedValue (newValue);

    if (style == TwoValueHorizontal || style == TwoValueVertical)
        newValue = jmin (valueMax, newValue);
    else
        newValue = jmin (currentValue, newValue);

    if (valueMin != newValue)
    {
        valueMin = newValue;
        repaint();

        if (popupDisplay != 0)
        {
            ((ASliderPopupDisplayComponent*) popupDisplay)->updatePosition (getTextFromValue (valueMin));
            popupDisplay->repaint();
        }

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void ASlider::setMaxValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously)
{
    // The maximum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    newValue = constrainedValue (newValue);

    if (style == TwoValueHorizontal || style == TwoValueVertical)
        newValue = jmax (valueMin, newValue);
    else
        newValue = jmax (currentValue, newValue);

    if (valueMax != newValue)
    {
        valueMax = newValue;
        repaint();

        if (popupDisplay != 0)
        {
            ((ASliderPopupDisplayComponent*) popupDisplay)->updatePosition (getTextFromValue (valueMax));
            popupDisplay->repaint();
        }

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void ASlider::setDoubleClickReturnValue (const bool isDoubleClickEnabled,
                                        const double valueToSetOnDoubleClick) throw()
{
    doubleClickToValue = isDoubleClickEnabled;
    doubleClickReturnValue = valueToSetOnDoubleClick;
}

double ASlider::getDoubleClickReturnValue (bool& isEnabled_) const throw()
{
    isEnabled_ = doubleClickToValue;
    return doubleClickReturnValue;
}

void ASlider::updateText()
{
    if (valueBox != 0)
        valueBox->setText (getTextFromValue (currentValue), false);
}

void ASlider::setText(const String& newText,
                     const bool broadcastChangeMessage)
{
    if (valueBox != 0)
        valueBox->setText (newText, broadcastChangeMessage);
}

void ASlider::setTextColour(Colour& newcolour)
{
    if (valueBox != 0)
        valueBox->setColour(Label::textColourId, newcolour);
}

void ASlider::setTextValueSuffix (const String& suffix)
{
    if (textSuffix != suffix)
    {
        textSuffix = suffix;
        updateText();
    }
}

const String ASlider::getTextFromValue (double v)
{
    if (numDecimalPlaces > 0)
        return String (v, numDecimalPlaces) + textSuffix;
    else
        return String (roundDoubleToInt (v)) + textSuffix;
}

double ASlider::getValueFromText (const String& text)
{
    String t (text.trimStart());

    if (t.endsWith (textSuffix))
        t = t.substring (0, t.length() - textSuffix.length());

    while (t.startsWithChar (T('+')))
        t = t.substring (1).trimStart();

    return t.initialSectionContainingOnly (T("0123456789.,-"))
            .getDoubleValue();
}

double ASlider::proportionOfLengthToValue (double proportion)
{
    if (skewFactor != 1.0 && proportion > 0.0)
        proportion = exp (log (proportion) / skewFactor);

    return minimum + (maximum - minimum) * proportion;
}

double ASlider::valueToProportionOfLength (double value)
{
    const double n = (value - minimum) / (maximum - minimum);

    return skewFactor == 1.0 ? n : pow (n, skewFactor);
}

double ASlider::snapValue (double attemptedValue, const bool)
{
    return attemptedValue;
}

void ASlider::startedDragging()
{
}

void ASlider::stoppedDragging()
{
}

void ASlider::valueChanged()
{
}

void ASlider::enablementChanged()
{
    repaint();
}

void ASlider::setPopupMenuEnabled (const bool menuEnabled_) throw()
{
    menuEnabled = menuEnabled_;
}

void ASlider::setScrollWheelEnabled (const bool enabled) throw()
{
    scrollWheelEnabled = enabled;
}

void ASlider::labelTextChanged(ALabel* label)
{
    const double newValue = snapValue (getValueFromText (label->getText()), false);

    if (getValue() != newValue)
    {
        sendDragStart();
        setValue (newValue, true, true);
        sendDragEnd();
    }

    updateText(); // force a clean-up of the text, needed in case setValue() hasn't done this.
}

void ASlider::buttonClicked (Button* button)
{
    if (style == IncDecButtons)
    {
        sendDragStart();

        if (button == incButton)
            setValue (snapValue (getValue() + interval, false), true, true);
        else if (button == decButton)
            setValue (snapValue (getValue() - interval, false), true, true);

        sendDragEnd();
    }
}

double ASlider::constrainedValue (double value) const throw()
{
    if (interval > 0)
        value = minimum + interval * floor ((value - minimum) / interval + 0.5);

    if (value <= minimum || maximum <= minimum)
        value = minimum;
    else if (value >= maximum)
        value = maximum;

    return value;
}

float ASlider::getLinearSliderPos (const double value)
{
    double sliderPosProportional;

    if (maximum > minimum)
    {
        if (value < minimum)
        {
            sliderPosProportional = 0.0;
        }
        else if (value > maximum)
        {
            sliderPosProportional = 1.0;
        }
        else
        {
            sliderPosProportional = valueToProportionOfLength (value);
            jassert (sliderPosProportional >= 0 && sliderPosProportional <= 1.0);
        }
    }
    else
    {
        sliderPosProportional = 0.5;
    }

    if (style == LinearVertical || style == IncDecButtons)
        sliderPosProportional = 1.0 - sliderPosProportional;

    return (float) (sliderRegionStart + sliderPosProportional*(sliderRegionSize - 4));
}

bool ASlider::isHorizontal() const throw()
{
    return style == LinearHorizontal
        || style == LinearBar
        || style == TwoValueHorizontal
        || style == ThreeValueHorizontal;
}

bool ASlider::isVertical() const throw()
{
    return style == LinearVertical
        || style == TwoValueVertical
        || style == ThreeValueVertical;
}

bool ASlider::incDecDragDirectionIsHorizontal() const throw()
{
    return incDecButtonMode == incDecButtonsDraggable_Horizontal
            || (incDecButtonMode == incDecButtonsDraggable_AutoDirection && incDecButtonsSideBySide);
}

float ASlider::getPositionOfValue (const double value)
{
    if (isHorizontal() || isVertical())
    {
        return getLinearSliderPos (value);
    }
    else
    {
        jassertfalse // not a valid call on a slider that doesn't work linearly!
        return 0.0f;
    }
}

void ASlider::paint (Graphics& g)
{
    if (style != IncDecButtons)
    {
        if (style == Rotary || style == RotaryHorizontalDrag || style == RotaryVerticalDrag)
        {
            const float sliderPos = (float) valueToProportionOfLength (currentValue);
            jassert (sliderPos >= 0 && sliderPos <= 1.0f);

            drawRotarySlider (g,
                              sliderRect.getX(),
                              sliderRect.getY(),
                              sliderRect.getWidth(),
                              sliderRect.getHeight(),
                              sliderPos,
                              rotaryStart, rotaryEnd,
                             *this);
        }
        else
        {
            drawLinearSlider(g,
                             sliderRect.getX() + 1,
                             sliderRect.getY(),
                             sliderRect.getWidth() - 2,
                             sliderRect.getHeight(),
                             getLinearSliderPos (currentValue),
                             getLinearSliderPos (valueMin),
                             getLinearSliderPos (valueMax),
                             getLinearSliderPos (0),
                             style,
                            *this);
        }
    }
}

void ASlider::resized()
{
    int minXSpace = 0;
    int minYSpace = 0;

    if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
        minXSpace = 30;
    else
        minYSpace = 15;

    const int tbw = jmax (0, jmin (textBoxWidth, getWidth() - minXSpace));
    const int tbh = jmax (0, jmin (textBoxHeight, getHeight() - minYSpace));

    if (style == LinearBar)
    {
        if (valueBox != 0)
            valueBox->setBounds (textBoxX, textBoxY, getWidth(), getHeight());
    }
    else
    {
        if (textBoxPos == NoTextBox)
        {
            sliderRect.setBounds (0, 0, getWidth(), getHeight());
        }
        else if (textBoxPos == TextBoxPositioned)
        {
            sliderRect.setBounds (0, 0, getWidth() - 1, getHeight());
            valueBox->setBounds (textBoxX, textBoxY, getWidth(), getHeight());
        }
        else if (textBoxPos == TextBoxLeft)
        {
            valueBox->setBounds (0, (getHeight() - tbh) / 2, tbw, tbh);
            sliderRect.setBounds (tbw, 0, getWidth() - tbw, getHeight());
        }
        else if (textBoxPos == TextBoxRight)
        {
            valueBox->setBounds (getWidth() - tbw, (getHeight() - tbh) / 2, tbw, tbh);
            sliderRect.setBounds (0, 0, getWidth() - tbw, getHeight());
        }
        else if (textBoxPos == TextBoxAbove)
        {
            valueBox->setBounds ((getWidth() - tbw) / 2, 0, tbw, tbh);
            sliderRect.setBounds (0, tbh, getWidth(), getHeight() - tbh);
        }
        else if (textBoxPos == TextBoxBelow)
        {
            valueBox->setBounds ((getWidth() - tbw) / 2, getHeight() - tbh - 12, tbw, tbh);
            sliderRect.setBounds (0, 0, getWidth(), getHeight() - tbh);
        }
    }

    //const int indent = getSliderThumbRadius(*this);
    const int indent = 0;

    if (style == LinearBar)
    {
        const int barIndent = 1;
        sliderRegionStart = barIndent;
        sliderRegionSize = getWidth() - barIndent * 2;

        sliderRect.setBounds (sliderRegionStart, barIndent,
                              sliderRegionSize, getHeight() - barIndent * 2);
    }
    else if (isHorizontal())
    {
        sliderRegionStart = sliderRect.getX() + indent;
        sliderRegionSize = jmax (1, sliderRect.getWidth() - indent * 2);

        sliderRect.setBounds (sliderRegionStart, sliderRect.getY(),
                              sliderRegionSize, sliderRect.getHeight());
    }
    else if (isVertical())
    {
        sliderRegionStart = sliderRect.getY() + indent;
        sliderRegionSize = jmax (1, sliderRect.getHeight() - indent * 2);

        sliderRect.setBounds (sliderRect.getX(), sliderRegionStart,
                              sliderRect.getWidth(), sliderRegionSize);
    }
    else
    {
        sliderRegionStart = 0;
        sliderRegionSize = 100;
    }

    if (style == IncDecButtons)
    {
        Rectangle buttonRect (sliderRect);

        if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
            buttonRect.expand (-2, 0);
        else
            buttonRect.expand (0, -2);

        incDecButtonsSideBySide = buttonRect.getWidth() > buttonRect.getHeight();

        if (incDecButtonsSideBySide)
        {
            decButton->setBounds (buttonRect.getX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth() / 2,
                                  buttonRect.getHeight());

            decButton->setConnectedEdges (Button::ConnectedOnRight);

            incButton->setBounds (buttonRect.getCentreX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth() / 2,
                                  buttonRect.getHeight());

            incButton->setConnectedEdges (Button::ConnectedOnLeft);
        }
        else
        {
            incButton->setBounds (buttonRect.getX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth(),
                                  buttonRect.getHeight() / 2);

            incButton->setConnectedEdges (Button::ConnectedOnBottom);

            decButton->setBounds (buttonRect.getX(),
                                  buttonRect.getCentreY(),
                                  buttonRect.getWidth(),
                                  buttonRect.getHeight() / 2);

            decButton->setConnectedEdges (Button::ConnectedOnTop);
        }
    }
}

void ASlider::focusOfChildComponentChanged (FocusChangeType)
{
    repaint();
}

void ASlider::mouseDown (const MouseEvent& e)
{
    mouseWasHidden = false;
    incDecDragged = false;

    if (isEnabled())
    {
        if (e.mods.isPopupMenu() && menuEnabled && param != NULL)
        {
            // Env at cursor
        }
        else if (maximum > minimum)
        {
            menuShown = false;

            if (valueBox != 0)
                valueBox->hideEditor (true);

            sliderBeingDragged = 0;

            if (style == TwoValueHorizontal
                 || style == TwoValueVertical
                 || style == ThreeValueHorizontal
                 || style == ThreeValueVertical)
            {
                const float mousePos = (float) (isVertical() ? e.y : e.x);

                const float normalPosDistance = fabsf (getLinearSliderPos (currentValue) - mousePos);
                const float minPosDistance = fabsf (getLinearSliderPos (valueMin) - 0.1f - mousePos);
                const float maxPosDistance = fabsf (getLinearSliderPos (valueMax) + 0.1f - mousePos);

                if (style == TwoValueHorizontal || style == TwoValueVertical)
                {
                    if (maxPosDistance <= minPosDistance)
                        sliderBeingDragged = 2;
                    else
                        sliderBeingDragged = 1;
                }
                else if (style == ThreeValueHorizontal || style == ThreeValueVertical)
                {
                    if (normalPosDistance >= minPosDistance && maxPosDistance >= minPosDistance)
                        sliderBeingDragged = 1;
                    else if (normalPosDistance >= maxPosDistance)
                        sliderBeingDragged = 2;
                }
            }

            minMaxDiff = valueMax - valueMin;

            mouseXWhenLastDragged = e.x;
            mouseYWhenLastDragged = e.y;
            lastAngle = rotaryStart + (rotaryEnd - rotaryStart)
                                        * valueToProportionOfLength (currentValue);

            if (sliderBeingDragged == 2)
                valueWhenLastDragged = valueMax;
            else if (sliderBeingDragged == 1)
                valueWhenLastDragged = valueMin;
            else
                valueWhenLastDragged = currentValue;

            valueOnMouseDown = valueWhenLastDragged;

            if (popupDisplayEnabled)
            {
                ASliderPopupDisplayComponent* const popup = new ASliderPopupDisplayComponent (this);
                popupDisplay = popup;

                if (parentForPopupDisplay != 0)
                {
                    parentForPopupDisplay->addChildComponent (popup);
                }
                else
                {
                    popup->addToDesktop (0);
                }

                popup->setVisible (true);
            }

            sendDragStart();

            mouseDrag (e);
        }
    }
}

void ASlider::mouseUp (const MouseEvent&)
{
    if (isEnabled()
         && (! menuShown)
         && (maximum > minimum)
         && (style != IncDecButtons || incDecDragged))
    {
        restoreMouseIfHidden();

        if (sendChangeOnlyOnRelease && valueOnMouseDown != currentValue)
            triggerChangeMessage (false);

        sendDragEnd();

        deleteAndZero (popupDisplay);

        if (style == IncDecButtons)
        {
            incButton->setState (Button::buttonNormal);
            decButton->setState (Button::buttonNormal);
        }
    }
}

void ASlider::restoreMouseIfHidden()
{
    if (mouseWasHidden)
    {
        mouseWasHidden = false;

        Component* c = Component::getComponentUnderMouse();

        if (c == 0)
            c = this;

        c->enableUnboundedMouseMovement (false);

        const double pos = (sliderBeingDragged == 2) ? getMaxValue()
                                                     : ((sliderBeingDragged == 1) ? getMinValue()
                                                                                  : currentValue);

        const int pixelPos = (int) getLinearSliderPos (pos);

        int x = isHorizontal() ? pixelPos : (getWidth() / 2);
        int y = isVertical()   ? pixelPos : (getHeight() / 2);

        relativePositionToGlobal (x, y);

   //     SystemLevel_SetMousePosition(x, y);
    }
}

void ASlider::modifierKeysChanged (const ModifierKeys& modifiers)
{
    if (isEnabled()
         && style != IncDecButtons
         && style != Rotary
         && isVelocityBased == modifiers.isAnyModifierKeyDown())
    {
        restoreMouseIfHidden();
    }
}
void ASlider::mouseDrag (const MouseEvent& e)
{
    if (isEnabled()
         && (! menuShown)
         && (maximum > minimum))
    {
        if (style == Rotary)
        {
            int dx = e.x - sliderRect.getCentreX();
            int dy = e.y - sliderRect.getCentreY();

            if (dx * dx + dy * dy > 25)
            {
                double angle = atan2 ((double) dx, (double) -dy);
                while (angle < 0.0)
                    angle += double_Pi * 2.0;

                if (rotaryStop && ! e.mouseWasClicked())
                {
                    if (fabs (angle - lastAngle) > double_Pi)
                    {
                        if (angle >= lastAngle)
                            angle -= double_Pi * 2.0;
                        else
                            angle += double_Pi * 2.0;
                    }

                    if (angle >= lastAngle)
                        angle = jmin (angle, (double) jmax (rotaryStart, rotaryEnd));
                    else
                        angle = jmax (angle, (double) jmin (rotaryStart, rotaryEnd));
                }
                else
                {
                    while (angle < rotaryStart)
                        angle += double_Pi * 2.0;

                    if (angle > rotaryEnd)
                    {
                        if (smallestAngleBetween (angle, rotaryStart) <= smallestAngleBetween (angle, rotaryEnd))
                            angle = rotaryStart;
                        else
                            angle = rotaryEnd;
                    }
                }

                const double proportion = (angle - rotaryStart) / (rotaryEnd - rotaryStart);

                valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, proportion));

                lastAngle = angle;
            }
        }
        else
        {
            if (style == LinearBar && e.mouseWasClicked()
                 && valueBox != 0 && valueBox->isEditable())
                return;

            if (style == IncDecButtons)
            {
                if (! incDecDragged)
                    incDecDragged = e.getDistanceFromDragStart() > 10 && ! e.mouseWasClicked();

                if (! incDecDragged)
                    return;
            }

            if ((isVelocityBased == (userKeyOverridesVelocity ? e.mods.testFlags (ModifierKeys::ctrlModifier | ModifierKeys::commandModifier | ModifierKeys::altModifier)
                                                              : false))
                || ((maximum - minimum) / sliderRegionSize < interval))
            {
                const int mousePos = (isHorizontal() || style == RotaryHorizontalDrag) ? e.x : e.y;

                double scaledMousePos = (mousePos - sliderRegionStart) / (double) sliderRegionSize;

                if (style == RotaryHorizontalDrag
                    || style == RotaryVerticalDrag
                    || style == IncDecButtons
                    || ((style == LinearHorizontal || style == LinearVertical || style == LinearBar)
                        && ! snapsToMousePos))
                {
                    const int mouseDiff = (style == RotaryHorizontalDrag
                                             || style == LinearHorizontal
                                             || style == LinearBar
                                             || (style == IncDecButtons && incDecDragDirectionIsHorizontal()))
                                            ? e.getDistanceFromDragStartX()
                                            : -e.getDistanceFromDragStartY();

                    double newPos = valueToProportionOfLength (valueOnMouseDown)
                                       + mouseDiff * (1.0 / pixelsForFullDragExtent);

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, newPos));

                    if (style == IncDecButtons)
                    {
                        incButton->setState (mouseDiff < 0 ? Button::buttonNormal : Button::buttonDown);
                        decButton->setState (mouseDiff > 0 ? Button::buttonNormal : Button::buttonDown);
                    }
                }
                else
                {
                    if (style == LinearVertical)
                        scaledMousePos = 1.0 - scaledMousePos;

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, scaledMousePos));
                }
            }
            else
            {
                const int mouseDiff = (isHorizontal() || style == RotaryHorizontalDrag
                                         || (style == IncDecButtons && incDecDragDirectionIsHorizontal()))
                                        ? e.x - mouseXWhenLastDragged
                                        : e.y - mouseYWhenLastDragged;

                const double maxSpeed = jmax (200, sliderRegionSize);
                double speed = jlimit (0.0, maxSpeed, (double) abs (mouseDiff));

                if (speed != 0)
                {
                    speed = 0.2 * velocityModeSensitivity
                              * (1.0 + sin (double_Pi * (1.5 + jmin (0.5, velocityModeOffset
                                                                            + jmax (0.0, (double) (speed - velocityModeThreshold))
                                                                                / maxSpeed))));

                    if (mouseDiff < 0)
                        speed = -speed;

                    if (style == LinearVertical || style == RotaryVerticalDrag
                         || (style == IncDecButtons && ! incDecDragDirectionIsHorizontal()))
                        speed = -speed;

                    const double currentPos = valueToProportionOfLength (valueWhenLastDragged);

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, currentPos + speed));

                    e.originalComponent->enableUnboundedMouseMovement (true, false);
                    mouseWasHidden = true;
                }
            }
        }

        valueWhenLastDragged = jlimit (minimum, maximum, valueWhenLastDragged);

        if (sliderBeingDragged == 0)
        {
            setValue (snapValue (valueWhenLastDragged, true),
                      ! sendChangeOnlyOnRelease, true);
        }
        else if (sliderBeingDragged == 1)
        {
            setMinValue (snapValue (valueWhenLastDragged, true),
                         ! sendChangeOnlyOnRelease, false);

            if (e.mods.isShiftDown())
                setMaxValue (getMinValue() + minMaxDiff, false);
            else
                minMaxDiff = valueMax - valueMin;
        }
        else
        {
            jassert (sliderBeingDragged == 2);

            setMaxValue (snapValue (valueWhenLastDragged, true),
                         ! sendChangeOnlyOnRelease, false);

            if (e.mods.isShiftDown())
                setMinValue (getMaxValue() - minMaxDiff, false);
            else
                minMaxDiff = valueMax - valueMin;
        }

        mouseXWhenLastDragged = e.x;
        mouseYWhenLastDragged = e.y;
    }
}

void ASlider::mouseDoubleClick (const MouseEvent&)
{
    if (doubleClickToValue
         && isEnabled()
         && style != IncDecButtons
         && minimum <= doubleClickReturnValue
         && maximum >= doubleClickReturnValue)
    {
        sendDragStart();
        setValue (doubleClickReturnValue, true, true);
        sendDragEnd();
    }
}

void ASlider::mouseWheelMove (const MouseEvent& e, float wheelIncrementX, float wheelIncrementY)
{
    if (scrollWheelEnabled && isEnabled())
    {
        if (maximum > minimum && ! isMouseButtonDownAnywhere())
        {
            if (valueBox != 0)
                valueBox->hideEditor (false);

            const double proportionDelta = (wheelIncrementX != 0 ? -wheelIncrementX : wheelIncrementY)*1.f/0.35156250f/getWidth();
			//const double proportionDelta = valueToProportionOfLength (1)/*wheelIncrementY/16*/;
            const double currentPos = valueToProportionOfLength (currentValue);
            const double newValue = proportionOfLengthToValue (jlimit (0.0, 1.0, currentPos + proportionDelta));

            double delta = (newValue != currentValue)
                            ? jmax (fabs (newValue - currentValue), interval) : 0;

            if (currentValue > newValue)
                delta = -delta;

            sendDragStart();
            setValue (snapValue (currentValue + delta, false), true, true);
            sendDragEnd();
        }
    }
    else
    {
        Component::mouseWheelMove (e, wheelIncrementX, wheelIncrementY);
    }
}

void ASlider::setParamIndex(int index)
{
    paramIndex = index;
}

int ASlider::getParamIndex()
{
    return paramIndex;
}

void ASlider::setParameter(Parameter* setparam)
{
    param = setparam;
}

Parameter*  ASlider::getParameter()
{
    return param;
}

void  ASlider::handleMessage(const Message & message)
{
    if(message.intParameter1 != 0x7fff0102) // Unique id for our message
        Component::handleMessage(message);
    else
    {
        Parameter* param = (Parameter*)message.pointerParameter;

        //if(!param->aslider_updating)
        {
            setValue(param->getValue(), false, false);
        }

       //setText(String(param->getLabel()), false);
    }
}

void ASlider::postParamMessage (Parameter* param) throw()
{
    postMessage(new Message (0x7fff0102, 0, 0, param)); // Unique id for our message
}

void ASliderListener::sliderDragStarted (ASlider*)
{
}

void ASliderListener::sliderDragEnded (ASlider*)
{
}



