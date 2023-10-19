#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "color.h"

typedef struct {
    double x, y;
} point_t;

point_t to_complex_space(size_t x, size_t y, size_t width, size_t height) {
    size_t scale = (width < height) ? width : height;
    size_t xoffset = (width - scale) / 2;
    size_t yoffset = (height - scale) / 2;
    point_t p = {.x = (double)(x - xoffset) / (double)scale * 4.0 - 2.0,
                 .y = (double)(y - yoffset) / (double)scale * 4.0 - 2.0};
    return p;
}

size_t julia_set(point_t p, point_t c, size_t max_iter) {
    size_t iter = 0;
    while (p.x * p.x + p.y * p.y <= 4.0 && iter < max_iter) {
        double xtemp = p.x * p.x - p.y * p.y + c.x;
        p.y = 2.0 * p.x * p.y + c.y;
        p.x = xtemp;
        iter++;
    }
    return iter;
}

void draw_julia_set(SDL_Renderer *renderer, point_t c, size_t width, size_t height, size_t max_iter, RGB_t *colormap) {
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            point_t p = to_complex_space(i, j, width, height);
            size_t iter = julia_set(p, c, max_iter);
            RGB_t color = colormap[iter];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

int main(void) {
    const size_t WIN_WIDTH = 1024;
    const size_t WIN_HEIGHT = 1024;
    const size_t WIDTH = 512;
    const size_t HEIGHT = 512;
    const size_t MAX_ITERATION = 100;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Julia Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

    RGB_t colormap[MAX_ITERATION + 1];
    // rainbow(colormap, MAX_ITERATION + 1, 2.0 / 6.0, 1.0);
    grayscale(colormap, MAX_ITERATION + 1);
    reverse(colormap, MAX_ITERATION + 1);

    double e = 0.0;
    bool running = true;
    while (running) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        SDL_SetRenderTarget(renderer, texture);
        point_t c = {.x = 0.7885 * cos(e), .y = 0.7885 * sin(e)};
        draw_julia_set(renderer, c, WIDTH, HEIGHT, MAX_ITERATION, colormap);
        e = fmod(e + 0.01, 2.0 * M_PI);
        if (e > 2 * M_PI) {
            running = false;
        }
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
