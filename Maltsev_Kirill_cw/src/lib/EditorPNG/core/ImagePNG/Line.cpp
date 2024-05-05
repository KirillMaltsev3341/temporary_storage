#include "ImagePNG.h"

#include <stdlib.h>
#include <algorithm>

void ImagePNG::drawLineLow(int x0, int y0, int x1, int y1, 
    int thickness, Color color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        drawCircle(x, y, thickness/2, 1, color, true, color);        

        if (D > 0) {
            y += yi;
            D += 2 * (dy - dx);
        } else {
            D += 2 * dy;
        }
    }
}

void ImagePNG::drawLineHigh(int x0, int y0, int x1, int y1, 
    int thickness, Color color)
{

    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;
    for (int y = y0; y <= y1; y++) {
        drawCircle(x, y, thickness/2, 1, color, true, color);

        if (D > 0) {
            x += xi;
            D += 2 * (dx - dy);
        } else {
            D += 2 * dx;
        }
    }
}

void ImagePNG::drawLine(int x0, int y0, int x1, int y1, 
    int thickness, Color color)
{

    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        drawLineLow(x0, y0, x1, y1, thickness, color);
    } else {
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        drawLineHigh(x0, y0, x1, y1, thickness, color);
    }
    
}