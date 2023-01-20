
#include "36_auxkeys.h"
#include "36_grid.h"
#include "36_draw.h"
#include "36_instrpanel.h"
#include "36_instr.h"
#include "36_text.h"





void AuxKeys::drawVerticalKey(Graphics& g, bool vert, int coord1, int coord2, int length, int key, bool pressed)
{
    int kx1, kx2, ky1, ky2;

    if(vert)
    {
        kx1 = coord1;
        kx2 = coord1 + length;
        ky1 = coord2;
        ky2 = coord2 + keyWidth;
    }
    else
    {
        ky1 = coord1;
        ky2 = coord1 + length;
        kx1 = coord2;
        kx2 = coord2 + keyWidth;
    }

    if(key == 1 || key == 3 || key == 6 || key == 8 || key == 10)
    {
        // black key

        setc(g, .2f);

        if(pressed)
        {
            setc(g, .2f);
        }

        if(vert)
        {
            kx2 -= int(length*.4f);
        }
        else
        {
            ky2 -= int(length*.4f);
        }

        gFillRect(g, kx1, ky1, kx2, ky2);

        g.excludeClipRegion(kx1, ky1, kx2 - kx1 + 1, ky2 - ky1 + 1);
    }
    else
    {
        // white key

        setc(g, .95f);

        if(pressed)
            setc(g, .8f);

        if((key == 0 || key == 2 || key == 5 || key == 7 || key == 9))
        {
            if(vert)
            {
                ky1 -= keyWidth/2;
            }
            else
            {
                kx2 += keyWidth/2;
            }
        }

        if((key == 2 || key == 4 || key == 7 || key == 9 || key == 11))
        {
            if(vert)
            {
                ky2 += keyWidth/2;
            }
            else
            {
                kx1 -= keyWidth/2;
            }
        }

        if(vert)
        {
            ky1++;
            ky2--;
        }
        else
        {
            kx1++;
            kx2--;
        }

        gFillRect(g, kx1, ky1, kx2, ky2);
    }
}

void AuxKeys::drawVerticalKeys(Graphics& g, int x, int y, int w, int h)
{
    fill(g, .6f);

    int key = isVertical ? 119 : 0;

    int coord = isVertical ? getY1(): getX1();

    g.saveState();

    while(1)
    {
        if(isVertical && coord > getY2() + keyWidth)
        {
            break;
        }

        if(!isVertical && coord > getX2() + keyWidth)
        {
            break;
        }

        int pianoKey = key%12;

        drawVerticalKey(g, isVertical, isVertical ? x : y, coord, isVertical ? w : h, key%12, keyState[key]);

        if(pianoKey == 0) // draw note name
        {
            setc(g, .1f);

            //gNoteString(g, x + w - 16, yc - 2, coord + keyWidth - 1, false);
        }

        key += isVertical ? -1 : 1;

        if(key < 0 || key == 120)
        {
            break;
        }

        coord += keyWidth;
    }

    g.restoreState();
}

void AuxKeys::drawHorizontalKeys(Graphics& g, int x, int y, int w, int h)
{
    fill(g, .8f);

    g.saveState();

    setc(g, .1f);

    int xKey = x;

    while(xKey < (x + w - 1))
    {
        gLineVertical(g, xKey, y, y + h);

        xKey += keyWidth;
    }

    xKey = x;

    int xCoord = x;
    int bkKeyWidth = int(keyWidth*0.5f);
    int key = 0;

    while(1)
    {
        int pianoKey = key%12;

        if(pianoKey == 1 || pianoKey == 3 || pianoKey == 6 || pianoKey == 8 || pianoKey == 10)
        {
            if(key == playNote)
            {
                setc(g, .91f);
            }
            else
            {
                setc(g, .4f);
            }

            gFillRect(g, xKey, y, xKey + bkKeyWidth, int(y + h*0.618f - 1));

            g.excludeClipRegion(xKey, y, bkKeyWidth + 1, int(h*0.618f));
        }
        else
        {
            if(pianoKey == 0)
            {
                xKey = xCoord + keyWidth - int(bkKeyWidth*0.7f);

                if(key == 60)
                {
                    setc(g, .95f);
                }
                else
                {
                    setc(g, .81f);
                }

                gFillRect(g, xCoord + 1, y, xCoord + keyWidth - 1, y + h - 1);

                setc(g, .4f);

                gNoteString(g, xCoord + 5, y + h - 4, key, false);
            }
            else if(pianoKey == 2)
            {
                xKey = xCoord + keyWidth - int(bkKeyWidth*0.3f);
            }
            else if(pianoKey == 4)
            {
                xKey = xCoord + keyWidth - bkKeyWidth/2;
            }
            else if(pianoKey == 5)
            {
                xKey = xCoord + keyWidth - int(bkKeyWidth*0.7f);
            }
            else if(pianoKey == 7)
            {
                xKey = xCoord + keyWidth - bkKeyWidth/2;
            }
            else if(pianoKey == 9)
            {
                xKey = xCoord + keyWidth - int(bkKeyWidth*0.3f);
            }

            if(key == playNote)
            {
                setc(g, .91f);

                gFillRect(g, xCoord + 1, y, xCoord + keyWidth - 1, y + h - 1);
            }

            if(pianoKey == 0)
            {
                setc(g, .4f);

                gNoteString(g, xCoord + 5, y + h - 4, key, false);
            }

            xCoord += keyWidth;
        }

        key++;

        if(key == 120)
        {
            break;
        }
    }

    setc(g, .3f);

    gFillRect(g, xCoord, y, x + w - 1, y + h - 1);

    g.restoreState();
}

AuxKeys::AuxKeys(Grid * gr, int key_width,bool vert)
{ 
    playNote = -1;
    prevPlayedNote = -1;

    grid = gr; 
    keyWidth = key_width; 
    isVertical = vert; 

    for(int n = 0; n < 120; n++)
    {
        keyState[n] = false;
    }
}

void AuxKeys::drawself(Graphics& g)
{
    fill(g, 0.35f);

    setc(g, 0.4f);

    lineH(g, 0, 0, width - 1);

    if(isVertical)
        drawVerticalKeys(g, x1, y1, width, height);
    else
        drawHorizontalKeys(g, x1, y1, width, height);
}

void AuxKeys::handleChildEvent(Gobj * obj, InputEvent& ev)
{
    ///
}

void AuxKeys::handleMouseDown(InputEvent & ev)
{
    playNote = getNote(ev.mouseX - x1, ev.mouseY - y1);

    if(playNote >= 0)
    {
        MInstrPanel->getCurrInstr()->preview(playNote);
    }

    prevPlayedNote = playNote;

    redraw();
}

void AuxKeys::handleMouseUp(InputEvent & ev)
{
    playNote = -1;
    prevPlayedNote = -1;

    redraw();
}

int AuxKeys::getNote(int mx, int my)
{
    int wKey = BaseNote;

    if(!isVertical)
    {
        int key = (mx)/keyWidth;
        int oct = key/7;

        wKey = key%7;

        if(wKey == 1)
        {
            wKey = 2;
        }
        else if(wKey == 2)
        {
            wKey = 4;
        }
        else if(wKey == 3)
        {
            wKey = 5;
        }
        else if(wKey == 4)
        {
            wKey = 7;
        }
        else if(wKey == 5)
        {
            wKey = 9;
        }
        else if(wKey == 6)
        {
            wKey = 11;
        }

        int bkHeight = int(height*0.618f);
        int bkKeyWidth = int(keyWidth*0.5f);

        if(my <= bkHeight)
        {
            int keyX = mx - int(mx/keyWidth)*keyWidth;

            if(wKey == 0)
            {
                if (keyX > (keyWidth - bkKeyWidth*0.7))
                        wKey = 1;
            }
            else if(wKey == 2)
            {
                if (keyX < bkKeyWidth*0.3)
                        wKey = 1;
                else if (keyX > (keyWidth - bkKeyWidth*0.3))
                        wKey = 3;
            }
            else if(wKey == 4)
            {
                if (keyX < bkKeyWidth*0.7)
                        wKey = 3;
            }
            else if(wKey == 5)
            {
                if (keyX > (keyWidth - bkKeyWidth*0.7))
                        wKey = 6;
            }
            else if(wKey == 7)
            {
                if (keyX < bkKeyWidth*0.3)
                        wKey = 6;
                else if (keyX > (keyWidth - bkKeyWidth*0.5))
                        wKey = 8;
            }
            else if(wKey == 9)
            {
                if (keyX < bkKeyWidth*0.5)
                        wKey = 8;
                else if (keyX > (keyWidth - bkKeyWidth*0.3))
                        wKey = 10;
            }
            else if(wKey == 11)
            {
                if (keyX < bkKeyWidth*0.7)
                        wKey = 10;
            }
        }

        wKey = oct*12 + wKey;

        if (wKey > 119) wKey = -1;
    }

    return wKey;
}

void AuxKeys::handleMouseDrag(InputEvent & ev)
{
    playNote = getNote(ev.mouseX - x1, ev.mouseY - y1);

    if(playNote >= 0 && playNote != prevPlayedNote)
    {
        MInstrPanel->getCurrInstr()->preview(playNote);

        prevPlayedNote = playNote;

        redraw();
    }
}



