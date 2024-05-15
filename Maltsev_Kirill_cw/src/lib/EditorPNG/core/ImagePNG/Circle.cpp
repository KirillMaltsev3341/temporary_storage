#include "ImagePNG.h"

#include <algorithm>


bool checkOnCircleLine(int x, int y, int x0, int y0, int radius, int thickness)
{
    bool flag1 = (x-x0)*(x-x0) + (y-y0)*(y-y0) <= (radius+thickness/2)*(radius+thickness/2);
    bool flag2 = (x-x0)*(x-x0) + (y-y0)*(y-y0) >= (std::max(0, radius-thickness/2))*(std::max(0, radius-thickness/2));
    return flag1 && flag2;
}

bool checkInCircle(int x, int y, int x0, int y0, int radius, int thickness)
{
    bool flag = (x-x0)*(x-x0) + (y-y0)*(y-y0) <= (radius-thickness/2)*(radius-thickness/2);
    return flag;
}


void epng::ImagePNG::drawCircle(int x0, int y0, int radius, int thickness, 
    Color color, bool fill, Color fill_color)
{
    for (int y = std::max(0, y0-radius-thickness/2); y <= std::min(height_-1, y0+radius+thickness/2); y++) {
        for (int x = std::max(0, x0-radius-thickness/2); x <= std::min(width_-1, x0+radius+thickness/2); x++) {
            if (fill && checkInCircle(x, y, x0, y0, radius, thickness)) {
                setColor(x, y, fill_color);
            }
            if (checkOnCircleLine(x, y, x0, y0, radius, thickness)) {
                setColor(x, y, color);
            }
        }
    }

    drawBresenhamCircle(x0, y0, radius-thickness/2, color);
    drawBresenhamCircle(x0, y0, radius+thickness/2, color);
}