#include "ImagePNG.h"

#include <vector>
#include <algorithm>


bool cmp(std::pair<int, int>& a, std::pair<int, int>& b)
{
    return abs(a.first * b.second) < abs(b.first * a.second);
}

void getIntersections(std::vector<std::pair<int, int>>& intersections, int y, std::vector<epng::Coord>& vertices)
{
    for (int i = 0; i < vertices.size(); i++) {
        int next = (i + 1) % vertices.size();

        if ((vertices[i].y > y && vertices[next].y <= y) || 
            (vertices[next].y > y && vertices[i].y <= y)) {
            
            intersections.push_back( 
                {
                (vertices[next].y - vertices[i].y) * vertices[i].x + (y - vertices[i].y) * (vertices[next].x - vertices[i].x), 
                (vertices[next].y - vertices[i].y)
                }
                );
        }
    }
}

void epng::ImagePNG::fillPolygon(std::vector<Coord>& vertices, Color& fill_color)
{
    int y_min = INT_MAX;
    int y_max = 0;
    for (int i = 0; i < vertices.size(); i++) {
        y_min = std::min(y_min, vertices[i].y);
        y_max = std::max(y_max, vertices[i].y);
    }

    for (int y = y_min; y <= y_max; y++) {
        std::vector<std::pair<int, int>> intersections;
        getIntersections(intersections, y, vertices);
        std::sort(intersections.begin(), intersections.end(), cmp);

        for (int i = 0; i < intersections.size(); i+=2) {
            
            int x_start = intersections[i].first / intersections[i].second;
            if ( abs((x_start + 1) * intersections[i].second) >= abs(intersections[i].first)) {
                x_start++;
            }

            int x_end = intersections[i+1].first / intersections[i+1].second;

            for (int x = x_start; x <= x_end; x++) {
                setColor(x, y, fill_color);
            }
        }
    }
}


void epng::ImagePNG::drawPolygon(std::vector<Coord> vertices, int thickness, 
    Color color, bool fill, Color fill_color)
{
    if (fill) {
        fillPolygon(vertices, fill_color);
    }

    for (int i = 0; i < vertices.size(); i++) {
        drawLine(vertices[i].x, vertices[i].y, vertices[(i+1)%vertices.size()].x, vertices[(i+1)%vertices.size()].y, thickness, color);
    }
}

bool epng::ImagePNG::inPolygon(int x0, int y0, std::vector<Coord>& vertices)
{
    int intersections_counter = 0;
    for (int i = 0; i < vertices.size(); i++) {
        int next = (i + 1) % vertices.size();
        
        if (((vertices[i].y > y0 && vertices[next].y <= y0) || 
            (vertices[next].y > y0 && vertices[i].y <= y0)) &&
            abs(x0 * (vertices[next].y - vertices[i].y)) < abs((vertices[next].y - vertices[i].y) * vertices[i].x + (y0 - vertices[i].y) * (vertices[next].x - vertices[i].x))) {
                intersections_counter++;
            }
    }
    return (intersections_counter % 2);
}