#include "ImagePNG.h"

#include <algorithm>


void epng::ImagePNG::setSize(int width, int height)
{
    width_ = width;
    height_ = height;
    
    allocateMemmoryForRowPointers();
    allocateMemmoryForColoredPixels();
    
    clear();
}

void epng::ImagePNG::resize(int new_width, int new_height, int x0, int y0)
{
    ImagePNG copy_image = copy(0, 0, width_-1, height_-1);
    setSize(new_width, new_height);
    this->paste(copy_image, x0, y0);
}

epng::ImagePNG epng::ImagePNG::copy(int x0, int y0, int x1, int y1)
{
    if (x0 > x1) {
        std::swap(x0, x1);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
    }

    ImagePNG copy_image;
    copy_image.setSize(x1-x0+1, y1-y0+1);
    for (int y = 0; y < copy_image.getHeight(); y++) {
        for (int x = 0; x < copy_image.getWidth(); x++) {
            copy_image.setColor(x, y, getColor(x + x0, y + y0));
        }
    }
    return copy_image;
}

void epng::ImagePNG::paste(ImagePNG& src_image, int x0, int y0)
{
    for (int y = 0; y < src_image.getHeight(); y++) {
        for (int x = 0; x < src_image.getWidth(); x++) {
            setColor(x + x0, y + y0, src_image.getColor(x, y));
        }
    }
}

void epng::ImagePNG::rotate(int rotation_type)
{
    ImagePNG copy_image = copy(0, 0, width_-1, height_-1);

    if (rotation_type == TURN_180) {
        clear();
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                setColor(x, y, copy_image.getColor(width_ - x - 1, height_ - y - 1));
            }
        }
    }
    if (rotation_type == TURN_90_CLOCKWISE) {
        setSize(height_, width_);
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                setColor(x, y, copy_image.getColor(y, width_ - x - 1));
            }
        }
    }
    if (rotation_type == TURN_90_COUNTERCLOCKWISE) {
        setSize(height_, width_);
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                setColor(x, y, copy_image.getColor(height_ - y - 1, x));
            }
        }
    }
}

void epng::ImagePNG::reflect(int reflection_type)
{
    if (reflection_type == VERTICAL) {
        for (int y = 0; y < height_/2; y++) {
            for (int x = 0; x < width_; x++) {
                Color a = getColor(x, y);
                Color b = getColor(x, height_-y-1);
                setColor(x, y, b);
                setColor(x, height_-y-1, a);
            }
        }
    }
    if (reflection_type == HORIZONTAL) {
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_/2; x++) {
                Color a = getColor(x, y);
                Color b = getColor(width_-x-1, y);
                setColor(x, y, b);
                setColor(width_-x-1, y, a);
            }
        }
    }
    
}