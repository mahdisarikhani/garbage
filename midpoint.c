/*
 * References:
 * - https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 * - https://people.cs.clemson.edu/~dhouse/courses/405/notes/ppm-files.pdf
 */

#include <stdint.h>
#include <stdio.h>

void write_ppm_image(uint32_t *pixels, size_t width, size_t height)
{
    FILE *file = fopen("circle.ppm", "w+");
    fprintf(file, "P6 %lu %lu 255\n", width, height);
    for (size_t i = 0; i < width * height; i++) {
        fprintf(file, "%c%c%c", (pixels[i] >> 16) & 0xFF, (pixels[i] >> 8) & 0xFF, pixels[i] & 0xFF);
    }
    fclose(file);
}

void draw_circle(uint32_t *pixels, int width, int height, int radius, uint32_t foreground)
{
    int x = 0;
    int y = radius;
    int r = radius;
    int cx = width / 2;
    int cy = height / 2;
    while (x <= y) {
        pixels[(cx + x) + (cy + y) * width] = foreground;
        pixels[(cx - x) + (cy + y) * width] = foreground;
        pixels[(cx + x) + (cy - y) * width] = foreground;
        pixels[(cx - x) + (cy - y) * width] = foreground;
        pixels[(cx + y) + (cy + x) * width] = foreground;
        pixels[(cx - y) + (cy + x) * width] = foreground;
        pixels[(cx + y) + (cy - x) * width] = foreground;
        pixels[(cx - y) + (cy - x) * width] = foreground;
        if (2 * (x * x + y * y - r * r + 2 * x + 1) + (1 - 2 * y) > 0) {
            y--;
        }
        x++;
    }
}

void draw_solid_circle(uint32_t *pixels, int width, int height, int radius, uint32_t foreground)
{
    int x = 0;
    int y = radius;
    int r = radius;
    int cx = width / 2;
    int cy = height / 2;
    while (x <= y) {
        for (int i = -y; i <= y; i++) {
            pixels[(cx + i) + (cy + x) * width] = foreground;
            pixels[(cx + i) + (cy - x) * width] = foreground;
        }
        if (2 * (x * x + y * y - r * r + 2 * x + 1) + (1 - 2 * y) > 0) {
            for (int i = -x; i <= x; i++) {
                pixels[(cx + i) + (cy + y) * width] = foreground;
                pixels[(cx + i) + (cy - y) * width] = foreground;
            }
            y--;
        }
        x++;
    }
}

int main()
{
    const size_t RADIUS = 256;
    const size_t WIDTH = 2 * RADIUS + 1;
    const size_t HEIGHT = 2 * RADIUS + 1;
    uint32_t pixels[WIDTH * HEIGHT];
    for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
        pixels[i] = 0xFFFFFF;
    }
    for (size_t r = RADIUS; r > 0; r--) {
        draw_circle(pixels, WIDTH, HEIGHT, r, (r * 257 * 257 * 257 - 1) / RADIUS);
    }
    // draw_circle(pixels, WIDTH, HEIGHT, RADIUS, 0xFF0000);
    // draw_solid_circle(pixels, WIDTH, HEIGHT, RADIUS, 0x00FFFF);
    write_ppm_image(pixels, WIDTH, HEIGHT);

    return 0;
}
