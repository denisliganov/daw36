

#include "36_text.h"
#include "36_draw.h"
#include "36_keyboard.h"
#include "36_project.h"
#include "36_utils.h"



#include "Binarysrc/mfont.h"
#include "Binarysrc/tahoma.h"
#include "Binarysrc/tiny.h"
#include "Binarysrc/tiny1.h"
#include "Binarysrc/aripix.h"
#include "Binarysrc/instrum.h"
#include "Binarysrc/big.h"
#include "Binarysrc/projs.h"
#include "Binarysrc/roxx.h"
#include "Binarysrc/dpix.h"
#include "Binarysrc/fixed.h"
#include "Binarysrc/bold.h"
#include "Binarysrc/visitor.h"


Font*   taho;
Font*   ti;
Font*   ti1;
Font*   ari;
Font*   ins;
Font*   prj;
Font*   rox;
Font*   dix;
Font*   fix;
Font*   bld;
Font*   vis;




Font* gGetFontById(FontId id)
{
    switch(id)
    {
        case FontArial:
            return ari;
        case FontInst:
            return ins;
        case FontRox:
            return rox;
        case FontVis:
            return vis;
        case FontProject:
            return prj;
        case FontBold:
            return bld;
        case FontBig:
            return taho;
        case FontFix:
            return fix;
        case FontSmall:
            return ti;
        case FontSmall1:
            return ti1;
        case FontDix:
            return dix;
    }

    return NULL;
}

int gText(Graphics& g, FontId fontId, std::string str, int x, int y)
{
    Font* font = gGetFontById(fontId);

    g.setFont(*font);

    String s = String(str.data());

    g.drawSingleLineText(s, x, y);

    return font->getStringWidth(s);
}

int gTextS(Graphics& g, FontId fontId, String str, int x, int y)
{
    Font* font = gGetFontById(fontId);

    g.setFont(*font);

    g.drawSingleLineText(str, x, y);

    return font->getStringWidth(str);
}

void gTextFit(Graphics& g, FontId fontId, std::string str, int x, int y, int maxwidth)
{
    std::string str1 = str;
    Font* font = gGetFontById(fontId);
    int wi = font->getStringWidth(String(str.data()));
    int ww = font->getStringWidth(String(".."));
    int pos = str.size();
    while((wi > (maxwidth - ww)) && pos-- >= 0)
    {
        int sw = font->getStringWidth(String(str1.substr(pos, 1).data()));

        wi -= sw;

        str1 = str1.substr(0, str1.size()-1);
    }

    if(pos >= 0 && pos < str.size())
    {
        str1 += "..";
    }

    gText(g, fontId, str1, x, y);
}

void gTextFitS(Graphics& g, FontId fontId, String str, int x, int y, int maxwidth)
{
    Font* font = gGetFontById(fontId);
    int wi = font->getStringWidth(str);
    int ww = font->getStringWidth(String(".."));

    int pos = str.length() - 1;

    String str1 = str;

    while((wi > (maxwidth - ww)) && pos-- >= 0)
    {
        int sw = font->getStringWidth(str1.substring(pos, pos));

        wi -= sw;

        str1 = str1.substring(0, pos - 1);
    }

    if(pos >= 0 && pos < str.length())
    {
        str1 += "..";
    }

    gTextS(g, fontId, str1, x, y);
}

int gNoteString(Graphics& g, int x, int y, int note, bool relative)
{
    bool neg = false;
    int nnum = note % 12;

    if(note < 0)
    {
        neg = true;
        nnum = 12 + nnum;
        nnum = nnum % 12;
    }

    int onum = note / 12;

    if(neg == true)
    {
        onum = abs(onum);

        if(nnum > 0)
        {
            onum += 1;
        }
    }

    int nw = gText(g, FontSmall, (std::string)note_table[abs(nnum)], x, y);

    char oct[3];

    Num2String(onum, oct);

    if(neg)
    {
        oct[1] = oct[0];
        oct[0] = '-';
        oct[2] = 0;
    }

    nw += gText(g, FontSmall, (std::string)oct, x + nw, y);

    return nw;
}

int gGetTextWidth(FontId fontId, std::string str)
{
    return (int)gGetFontById(fontId)->getStringWidth(String(str.data()));
}

int gGetTextHeight(FontId fontId)
{
    return (int)gGetFontById(fontId)->getHeight();
}

Image* gGetTextImage(Graphics& g, FontId fontId, const char* text, uint32 color)
{
    Font* font = gGetFontById(fontId);

    int width = gGetTextWidth(fontId, text);

    Image* image = NULL;

    if(width > 0)
    {
        image = new Image(Image::ARGB, width, (int)(gGetTextHeight(fontId) + 5), true);

        Graphics imgContext(*image);

        imgContext.setColour(Colour(color));

        gText(imgContext, fontId, (std::string)text, 0, (int)(gGetTextHeight(fontId)));

        return image;
    }
    else
    {
        return NULL;
    }
}

void gDChar(Graphics& g, char symbol, int x, int y, int w, int h)
{
    gDawChar(g, symbol, x, y, w, h);
}

void gDString(Graphics& g, const char* string, int x, int y, int sym_w, int sym_h)
{
    int xv = x;

    for(unsigned int c = 0; c < strlen(string); c++)
    {
        if(string[c] != ' ')
        {
            gDChar(g, string[c], xv, y, sym_w, sym_h);

            xv += sym_w + sym_w/5;
        }
        else
        {
            xv += sym_w/2;
        }
    }
}


int gZxChar(Graphics& g, const char symbol, int x, int y, int s)
{
    char** glyph = NULL;
    int h = 8, w = 7;

    if(symbol == '0')
    {
        char* g[8] =
        {
            "       ",
            " ##### ",
            "#     #",
            "#   # #",
            "#  #  #",
            "# #   #",
            "#     #",
            " ##### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '1')
    {
        w = 4;
        char* g[8] =
        {
            "   ",
            " # ",
            "## ",
            " # ",
            " # ",
            " # ",
            " # ",
            "###",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '2')
    {
        char* g[8] =
        {
            "       ",
            " ##### ",
            "#     #",
            "      #",
            " ##### ",
            "#      ",
            "#     #",
            "#######",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '3')
    {
        char* g[8] =
        {
            "       ",
            " ##### ",
            "#     #",
            "      #",
            "  #### ",
            "      #",
            "#     #",
            " ##### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '4')
    {
        char* g[8] =
        {
            "       ",
            "#     #",
            "#     #",
            "#     #",
            " ######",
            "      #",
            "      #",
            "      #",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '5')
    {
        char* g[8] =
        {
            "       ",
            "#######",
            "#      ",
            "###### ",
            "      #",
            "      #",
            "      #",
            "###### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == '6')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#       ",
            "######  ",
            "#     # ",
            "#     # ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'A')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#     # ",
            "#     # ",
            "####### ",
            "#     # ",
            "#     # ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'B')
    {
        char* g[8] =
        {
            "        ",
            "######  ",
            "#     # ",
            "#     # ",
            "######  ",
            "#     # ",
            "#     # ",
            "######  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'C')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#     # ",
            "#       ",
            "#       ",
            "#       ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'D')
    {
        char* g[8] =
        {
            "        ",
            "######  ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            "######  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'E')
    {
        char* g[8] =
        {
            "        ",
            "####### ",
            "#       ",
            "#       ",
            "####    ",
            "#       ",
            "#       ",
            "####### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'F')
    {
        char* g[8] =
        {
            "        ",
            "####### ",
            "#       ",
            "#       ",
            "####    ",
            "#       ",
            "#       ",
            "#       ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'G')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#     # ",
            "#       ",
            "#       ",
            "#   ### ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'H')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "#     # ",
            "#     # ",
            "####### ",
            "#     # ",
            "#     # ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'I')
    {
        w = 5;
        char* g[8] =
        {
            "     ",
            " ### ",
            "  #  ",
            "  #  ",
            "  #  ",
            "  #  ",
            "  #  ",
            " ### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'J')
    {
        char* g[8] =
        {
            "        ",
            "      # ",
            "      # ",
            "      # ",
            "      # ",
            "      # ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'K')
    {
        char* g[8] =
        {
            "        ",
            "#    ## ",
            "#  ##   ",
            "###     ",
            "#  #    ",
            "#   #   ",
            "#    #  ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'L')
    {
        char* g[8] =
        {
            "        ",
            "#       ",
            "#       ",
            "#       ",
            "#       ",
            "#       ",
            "#       ",
            "####### ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'M')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "##   ## ",
            "# # # # ",
            "#  #  # ",
            "#     # ",
            "#     # ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'N')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "##    # ",
            "# #   # ",
            "#  #  # ",
            "#   # # ",
            "#    ## ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'O')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'P')
    {
        char* g[8] =
        {
            "        ",
            "######  ",
            "#     # ",
            "#     # ",
            "######  ",
            "#       ",
            "#       ",
            "#       ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'Q')
    {
        char* g[8] =
        {
            "        ",
            " #####  ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#   # # ",
            "#    ## ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'R')
    {
        char* g[8] =
        {
            "        ",
            "######  ",
            "#     # ",
            "#     # ",
            "######  ",
            "#   #   ",
            "#    #  ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'S')
    {
        char* g[8] =
        {
            "        ",
            " ###### ",
            "#       ",
            "#       ",
            " #####  ",
            "      # ",
            "      # ",
            "######  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'T')
    {
        char* g[8] =
        {
            "        ",
            "####### ",
            "   #    ",
            "   #    ",
            "   #    ",
            "   #    ",
            "   #    ",
            "   #    ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'U')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'V')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "#     # ",
            " #   #  ",
            " #   #  ",
            "  # #   ",
            "  # #   ",
            "   #    ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'W')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "#     # ",
            "#     # ",
            "#  #  # ",
            "# # # # ",
            "##   ## ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'X')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            " #   #  ",
            "  # #   ",
            "   #    ",
            "  # #   ",
            " #   #  ",
            "#     # ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'Y')
    {
        char* g[8] =
        {
            "        ",
            "#     # ",
            "#     # ",
            "#     # ",
            " ###### ",
            "      # ",
            "#     # ",
            " #####  ",
        };
        glyph = (char**)&g;
    }
    else if(symbol == 'Z')
    {
        char* g[8] =
        {
            "        ",
            "####### ",
            "     #  ",
            "    #   ",
            "   #    ",
            "  #     ",
            " #      ",
            "####### ",
        };
        glyph = (char**)&g;
    }

    if(glyph != NULL)
    {
        for(int yy = 0; yy < h; yy++)
        {
            for(int xx = 0; xx < w; xx++)
            {
                if(glyph[yy][xx] == '#')
                {
                    g.setPixel(x + xx, y + yy);
                }
            }
        }
    }

    return w;
}

void gZxString(Graphics& g, const char* string, int x, int y)
{
    int xc = x;
    int len = strlen(string);

    for(int c = 0; c < len; c++)
    {
        int w = 8;

        if(string[c] != ' ')
        {
            w = gZxChar(g, string[c], xc, y, 8);
        }

        xc += w + 2;
    }
}

void gDawChar(Graphics& g, const char symbol, int x, int y, int w, int h)
{
    switch(symbol)
    {
        case '0':
            gDrawRectWH(g, x, y, w, h);
            gLine(g, x, y + h - 1, x + w - 1, y);
            break;
        case '1':
            gLineVertical(g, x + w/2 - 1, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineHorizontal(g, y, x, x + w/2 - 1);
            break;
        case '2':
            gLineHorizontal(g, y, x + w/4, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h/2 + 1);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            gLineVertical(g, x, y + h/2, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w);
            break;
        case '3':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineHorizontal(g, y + h/2, x + w/2 - 1, x + w - 1);
            break;
        case '4':
            gLineVertical(g, x, y, y + h/2);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h);
            break;
        case '5':
            gLineHorizontal(g, y, x + w/5, x + w);
            gLineVertical(g, x + w/5, y, y + h/2);
            gLineHorizontal(g, y + h/2, x + w/5, x + w - 1);
            gLineVertical(g, x + w - 1, y + h/2, y + h);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            break;
        case '6':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x + w - 1, y + h/2, y + h);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            break;
        case '7':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h);
            break;
        case '8':
            gDrawRect(g, x, y, x + w - 1, y + h - 1);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            break;
        case '9':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x, y, y + h/2 + 1);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            break;
        case 'A':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h);
            gLineVertical(g, x + w - 1, y, y + h);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            break;
        case 'B':
            gDrawRect(g, x, y, int(x + w*0.75f), y + h/2);
            gDrawRect(g, x, y + h/2, x + w - 1, y + h - 1);
            break;
        case 'C':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            break;
        case 'D':
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y, x, x + w/2);
            gLine(g,  x + w/2, y, x + w - 1, y + h/2);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x + w - 1, y + h/2, y + h);
            break;
        case 'E':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineHorizontal(g, y + h/2, x, x + w/2);
            break;
        case 'F':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h/2, x, x + w/2);
            break;
        case 'G':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x + w - 1, y + h/2, y + h);
            gLineHorizontal(g, y + h/2, x + w/2, x + w - 1);
            break;
        case 'H':
            gLineVertical(g, x, y, y + h);
            gLineVertical(g, x + w - 1, y, y + h);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            break;
        case 'I':
            gLineVertical(g, x + w/2 - 1, y, y + h - 1);
            gLineHorizontal(g, y, x, x + w - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            break;
        case 'J':
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x + w - 1, y, y + h - 1);
            break;
        case 'K':
            gLineVertical(g, x, y, y + h);
            gLine(g, x, y + h/2, x + w - 1, y);
            gLine(g, x, y + h/2, x + w, y + h);
            break;
        case 'L':
            gLineVertical(g, x, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w);
            break;
        case 'M':
            gLineVertical(g, x, y, y + h);
            gLineVertical(g, x + w - 1, y, y + h);
            gLine(g, (float)x, (float)y, float((float)x + (float)w/2.f), float(y + h));
            gLine(g, float((float)x + (float)w/2.f), float(y + h), float(x + w - 1), (float)y);
            break;
        case 'N':
            gLineVertical(g, x, y, y + h);
            gLineVertical(g, x + w - 1, y, y + h);
            gLine(g, x, y, x + w - 1, y + h - 1);
            break;
        case 'O':
            gDrawRect(g, x, y, x + w - 1, y + h - 1);
            break;
        case 'P':
            gLineVertical(g, x, y, y + h - 1);
            gDrawRect(g, x, y, x + w - 1, y + h/2);
            break;
        case 'Q':
            gDrawRect(g, x, y, x + w - 1, y + h - 1);
            gLine(g, int(x + w*0.65f), int(y + h*0.65f), x + w - 1, y + h - 1);
            break;
        case 'R':
            gLineVertical(g, x, y, y + h);
            gDrawRect(g, x, y, x + w - 1, y + h/2);
            gLine(g, x + w/2, y + h/2, x + w - 1, y + h);
            break;
        case 'S':
            gLineHorizontal(g, y, x, x + w - 1);
            gLineHorizontal(g, y + h/2, x, x + w - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            gLineVertical(g, x, y, y + h/2);
            gLineVertical(g, x + w - 1, y + h/2, y + h);
            break;
        case 'T':
            gLineVertical(g, x + w/2, y, y + h);
            gLineHorizontal(g, y, x, x + w);
            break;
        case 'U':
            gLineVertical(g, x, y, y + h - 1);
            gLineVertical(g, x + w - 1, y, y + h - 1);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            break;
        case 'V':
            gLine(g, x, y, x + w/2, y + h - 1);
            gLine(g, x + w/2, y + h - 1, x + w - 1, y);
            break;
        case 'W':
            gLineVertical(g, x, y, y + h - 1);
            gLineVertical(g, x + w - 1, y, y + h - 1);
            gLine(g, x, y + h, x + w/2, y + h/2);
            gLine(g, x + w/2, y + h/2, x + w - 1, y + h);
            break;
        case 'X':
            gLine(g, x, y, x + w, y + h);
            gLine(g, x, y + h, x + w, y);
            break;
        case 'Y':
            gLine(g, x, y, x + w/2, y + h/2);
            gLine(g, x + w/2, y + h/2, x + w - 1, y);
            gLineVertical(g, x + w/2, y + h/2, y + h - 1);
            break;
        case 'Z':
            gLineHorizontal(g, y, x, x + w - 1);
            gLine(g, x, y + h - 1, x + w - 1, y);
            gLineHorizontal(g, y + h - 1, x, x + w - 1);
            break;
        case 'a':
            break;
        case 'b':
            break;
    }

}

void gLoadFonts()
{
    MemoryInputStream fontStream5(tahoma::fontbin_bin, tahoma::fontbin_binSize, false );

#ifdef USE_OLD_JUCE

    Typeface* tf = new Typeface(fontStream5);

    taho = new Font(*tf);

    delete tf;

#else

    CustomTypeface* tf = new CustomTypeface(fontStream5);

    taho = new Font(tf);

#endif

    taho->setHeight(20.0f);

    taho->setHorizontalScale(1.0f);

    MemoryInputStream fontStream6(tiny::fontbin_bin, tiny::fontbin_binSize, false );

#ifdef USE_OLD_JUCE

    tf = new Typeface(fontStream6);

    ti = new Font(*tf);

    delete tf;

#else

    tf = new CustomTypeface(fontStream6);

    ti = new Font(tf);

#endif

    ti->setHeight(13.0f);

    ti->setHorizontalScale(1.0f);

    MemoryInputStream fontStream7(aripix::fontbin_bin, aripix::fontbin_binSize, false );

#ifdef USE_OLD_JUCE

    tf = new Typeface(fontStream7);

    ari = new Font(*tf);

    delete tf;

#else
    tf = new CustomTypeface(fontStream7);
    ari = new Font(tf);
#endif
    ari->setHeight(12.0f);
    ari->setHorizontalScale(1.0f);

    MemoryInputStream fontStream10(instrum::fontbin_bin, instrum::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream10);
    ins = new Font(*tf);
    delete tf;
#else
    tf = new CustomTypeface(fontStream10);
    ins = new Font(tf);
#endif
    ins->setHeight(10.0f);
    ins->setHorizontalScale(1.0f);

    MemoryInputStream fontStream11(tiny1::fontbin_bin, tiny1::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream11);
    ti1 = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream11);
    ti1 = new Font(tf);
#endif
    ti1->setHeight(13.0f);
    ti1->setHorizontalScale(1.0f);

    MemoryInputStream fontStream12(projs::fontbin_bin, projs::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream12);
    prj = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream12);
    prj = new Font(tf);
#endif
    prj->setHeight(12.0f);
    prj->setHorizontalScale(1.0f);

    MemoryInputStream fontStream13(roxx::fontbin_bin, roxx::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream13);
    rox = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream13);
    rox = new Font(tf);
#endif
    rox->setHeight(11.0f);
    rox->setHorizontalScale(1.0f);

    MemoryInputStream fontStream14(dpix::fontbin_bin, dpix::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream14);
    dix = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream14);
    dix = new Font(tf);
#endif
    dix->setHeight(13.0f);
    dix->setHorizontalScale(1.0f);

    MemoryInputStream fontStream15(fixed::fontbin_bin, fixed::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream15);
    fix = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream15);
    fix = new Font(tf);
#endif
    fix->setHeight(9.0f);
    fix->setHorizontalScale(1.0f);

    MemoryInputStream fontStream16(Bold::fontbin_bin, Bold::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream16);
    bld = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream16);
    bld = new Font(tf);
#endif
    bld->setHeight(11.0f);
    bld->setHorizontalScale(1.0f);

    MemoryInputStream fontStream17(visitor::fontbin_bin, visitor::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
    tf = new Typeface(fontStream17);
    vis = new Font(*tf);
#else
    tf = new CustomTypeface(fontStream17);
    vis = new Font(tf);
#endif
    vis->setHeight(9.0f);
    vis->setHorizontalScale(1.0f);
}


