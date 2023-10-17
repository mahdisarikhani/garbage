#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "color.h"

typedef struct {
    double x, y;
} point_t;

point_t to_complex(SDL_Point p, size_t width, size_t height)
{
    size_t scale = (width < height) ? width : height;
    size_t xoffset = (width - scale) / 2;
    size_t yoffset = (height - scale) / 2;
    point_t fp = {
        .x = (double)(p.x - xoffset) / (double)scale * 4.0 - 2.0,
        .y = (double)(p.y - yoffset) / (double)scale * 4.0 - 2.0
    };
    return fp;
}

SDL_Point to_pixel(point_t p, size_t width, size_t height)
{
    size_t scale = (width < height) ? width : height;
    size_t xoffset = (width - scale) / 2;
    size_t yoffset = (height - scale) / 2;
    SDL_Point fp = {
        .x = (p.x + 2.0) / 4.0 * scale + xoffset,
        .y = (p.y + 2.0) / 4.0 * scale + yoffset
    };
    return fp;
}

void next_point(point_t *p, point_t c)
{
    double xtemp = p->x * p->x - p->y * p->y + c.x;
    p->y = 2.0 * p->x * p->y + c.y;
    p->x = xtemp;

    // This implementation is about 10x slower than above implementation,
    // because of using `pow()` and `cpow()` functions.
    // double complex z = CMPLX(p->x, p->y);
    // z = cpow(z, 3);
    // p->x = creal(z) + c.x;
    // p->y = cimag(z) + c.y;
}

void mandelbrot_set(size_t *set, size_t width, size_t height, size_t max_iteration)
{
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            SDL_Point init = {.x = i, .y = j};
            point_t c = to_complex(init, width, height);
            point_t p = {.x = 0.0, .y = 0.0};
            size_t iteration = 0;
            while (p.x * p.x + p.y * p.y <= 4.0 && iteration < max_iteration) {
                next_point(&p, c);
                iteration++;
            }
            set[i + j * width] = iteration;
        }
    }
}

void draw_mandelbrot_set(SDL_Renderer *renderer, size_t *set, size_t width, size_t height, RGB_t *colormap)
{
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            RGB_t color = colormap[set[i + j * width]];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void draw_mandelbrot_path(SDL_Renderer *renderer, SDL_Point mouse_pos, size_t max_iteration, size_t width, size_t height)
{
    point_t c = to_complex(mouse_pos, width, height);
    point_t p = {.x = 0.0, .y = 0.0};
    point_t prev_point = c;
    size_t iteration = 0;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    while (p.x * p.x + p.y * p.y <= 4.0 && iteration < max_iteration) {
        next_point(&p, c);
        SDL_Point prev_pixel = to_pixel(prev_point, width, height);
        SDL_Point current_pixel = to_pixel(p, width, height);
        SDL_RenderDrawLine(renderer, prev_pixel.x, prev_pixel.y, current_pixel.x, current_pixel.y);
        prev_point = p;
        iteration++;
    }
}

int main()
{
    const size_t WIDTH = 1024;
    const size_t HEIGHT = 1024;
    const size_t NUM = WIDTH * HEIGHT;
    const size_t MAX_ITERATION = 200;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    size_t *set = malloc(sizeof(size_t) * NUM);
    mandelbrot_set(set, WIDTH, HEIGHT, MAX_ITERATION);

    RGB_t colormap[MAX_ITERATION + 1];
    rainbow(colormap, MAX_ITERATION + 1, 1 / 6.0, 4 / 6.0);
    // grayscale(colormap, MAX_ITERATION + 1);
    reverse(colormap, MAX_ITERATION + 1);

    SDL_Point mouse_pos = {.x = 0, .y = 0};
    bool running = true;
    bool draw_path = false;
    while (running) {
        draw_mandelbrot_set(renderer, set, WIDTH, HEIGHT, colormap);
        if (draw_path) {
            draw_mandelbrot_path(renderer, mouse_pos, MAX_ITERATION, WIDTH, HEIGHT);
            draw_path = false;
        }
        SDL_RenderPresent(renderer);

        while (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_pos.x = event.button.x;
                    mouse_pos.y = event.button.y;
                    draw_path = true;
                } else {
                    draw_path = false;
                }
                break;
            }
        }
    }

    free(set);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
