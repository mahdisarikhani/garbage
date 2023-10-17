#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LIVE_PREVIEW

#ifdef LIVE_PREVIEW
#include <SDL2/SDL.h>
#endif

typedef struct {
    size_t left;
    size_t right;
    size_t up;
    size_t down;
} neighbours_t;

typedef struct {
    double energy;
    double magnetization;
    double specific_heat;
} properties_t;

double drand(void)
{
    return (double)rand() / (double)RAND_MAX;
}

void initialize(int8_t *lattice, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        lattice[i] = (rand() % 2) * 2 - 1;
    }
}

neighbours_t get_neighbours(size_t i, size_t rows, size_t cols)
{
    size_t num = rows * cols;
    neighbours_t nbrs = {
        .left = (i % cols == 0) ? i - 1 + cols : i - 1,
        .right = ((i + 1) % cols == 0) ? i + 1 - cols : i + 1,
        .up = (i < cols) ? num + i - cols : i - cols,
        .down = (i + cols > num) ? (i % cols) : i + cols
    };
    return nbrs;
}

int8_t calc_energy(size_t i, int8_t *lattice, size_t rows, size_t cols)
{
    neighbours_t nbrs = get_neighbours(i, rows, cols);
    int8_t energy = -1 * lattice[i] * (lattice[nbrs.left] + lattice[nbrs.right] + lattice[nbrs.up] + lattice[nbrs.down]);
    return energy;
}

void update(int8_t *lattice, size_t rows, size_t cols, double beta)
{
    size_t i = rand() % (rows * cols);
    int8_t energy = calc_energy(i, lattice, rows, cols);
    double delta_energy = -2.0 * energy;
    if (delta_energy < 0.0 || exp(-1.0 * beta * delta_energy) > drand()) {
        lattice[i] *= -1;
    }
}

properties_t get_properties(int8_t *lattice, size_t rows, size_t cols, double beta)
{
    size_t num = rows * cols;
    double energy_total = 0.0;
    double energy_squared = 0.0;
    double magnetization = 0.0;
    for (size_t i = 0; i < num; i++) {
        int8_t energy = calc_energy(i, lattice, rows, cols);
        energy_total += energy;
        energy_squared += energy * energy;
        magnetization += lattice[i];
    }
    energy_total /= num;
    energy_squared /= num;
    magnetization /= num;
    properties_t props = {
        .energy = energy_total,
        .magnetization = magnetization,
        .specific_heat = (energy_squared - (energy_total * energy_total)) * (beta * beta)
    };
    return props;
}

#ifdef LIVE_PREVIEW
void draw_lattice(SDL_Renderer *renderer, int8_t *lattice, size_t rows, size_t cols)
{
    for (size_t i = 0; i < cols; i++) {
        for (size_t j = 0; j < rows; j++) {
            uint8_t c = (lattice[i + j * cols] + 1) / 2 * 0xFF;
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}
#endif

int main()
{
    const size_t ROWS = 128;
    const size_t COLS = 128;
    const size_t NUM = ROWS * COLS;
    double beta = 10.0;

#ifdef LIVE_PREVIEW
    const size_t WIN_WIDTH = 1024;
    const size_t WIN_HEIGHT = 1024;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ising", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, COLS, ROWS);
#endif

    srand(time(NULL));

    int8_t lattice[NUM];
    initialize(lattice, NUM);

    bool running = true;
    size_t counter = 0;
    while (running) {
#ifdef LIVE_PREVIEW
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        SDL_SetRenderTarget(renderer, texture);
        draw_lattice(renderer, lattice, ROWS, COLS);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
#endif

        for (size_t i = 0; i < NUM; i++) {
            update(lattice, ROWS, COLS, beta);
        }
        properties_t props = get_properties(lattice, ROWS, COLS, beta);
        printf("%lu %d %-9.2f %-9.2f %-9.2f\n", counter, (int)props.energy, props.energy, props.magnetization, props.specific_heat);
        counter++;
        if (counter > NUM && (int)props.energy <= -4) {
            running = false;
        }
    }

#ifdef LIVE_PREVIEW
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return 0;
}
