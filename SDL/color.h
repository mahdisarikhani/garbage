#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    double h;
    double s;
    double v;
} HSV_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

RGB_t hsv2rgb(HSV_t hsv)
{
    double c = hsv.v * hsv.s;
    double h = hsv.h * 6;
    double x = c * (1 - fabs(fmod(h, 2) - 1));
    double r, g, b;
    if (h <= 1) {
        r = c;
        g = x;
        b = 0;
    } else if (h <= 2) {
        r = x;
        g = c;
        b = 0;
    } else if (h <= 3) {
        r = 0;
        g = c;
        b = x;
    } else if (h <= 4) {
        r = 0;
        g = x;
        b = c;
    } else if (h <= 5) {
        r = x;
        g = 0;
        b = c;
    } else if (h <= 6) {
        r = c;
        g = 0;
        b = x;
    } else {
        r = 0;
        g = 0;
        b = 0;
    }
    double m = hsv.v - c;
    RGB_t rgb = {.r = (r + m) * 255, .g = (g + m) * 255, .b = (b + m) * 255};
    return rgb;
}

void rainbow(RGB_t *colormap, size_t num, double a, double b)
{
    for (size_t i = 0; i < num; i++) {
        HSV_t hsv = {
            .h = a + ((double)i / (double)(num - 1)) * (b - a),
            .s = 1,
            .v = 1
        };
        RGB_t rgb = hsv2rgb(hsv);
        colormap[i] = rgb;
    }
}

void grayscale(RGB_t *colormap, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        uint8_t c = ((double)i / (double)(num - 1)) * 255;
        RGB_t rgb = {.r = c, .g = c, .b = c};
        colormap[i] = rgb;
    }
}

void reverse(RGB_t *colormap, size_t num)
{
    size_t end = num - 1;
    for (size_t i = 0; i < num / 2; i++) {
        RGB_t tmp = colormap[i];
        colormap[i] = colormap[end - i];
        colormap[end - i] = tmp;
    }
}

#endif
