

#include "36_globals.h"
#include "36_grid.h"
#include "36_keyboard.h"
#include "36_draw.h"
#include "36_textcursor.h"



TextCursor::TextCursor()
{
    grid = NULL;

    tick = 0;
    line = 0;
}

void TextCursor::setPos(float newTick, int newLine)
{
    tick = newTick;
    line = newLine;

    updPos();
}

void TextCursor::updPos()
{
    grid = (Grid*)parent;

    if (grid)
    {
        if (grid->getDisplayMode() == GridDisplayMode_Bars)
        {
            if (grid->mode == GridMode_Default)
            {
                int x = grid->getXfromTick(tick) - grid->getX1();
                int y = grid->getYfromLine(line) - int(grid->getLineHeight()) - grid->getY1() + 1;

                setXY(x, y, x, y + int(grid->getLineHeight()) - 1);

                setVisible(true);
            }
        }
    }
}

void TextCursor::drawSelf(Graphics& g)
{
    gSetMonoColor(g, 1, .7f);
    gFillRect(g, x1, y1, x1, y2);
}

float TextCursor::getTick()
{
    return tick;
}

int TextCursor::getLine()
{
    return line;
}


