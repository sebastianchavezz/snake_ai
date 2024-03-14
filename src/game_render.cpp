#include "../includes/game_render.h"
#include "../includes/game_logic.h"


GameRenderer::GameRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // Handle SDL initialization failure
        exit(1);
    }

    window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        // Handle window creation failure
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        // Handle renderer creation failure
        exit(1);
    }
}

GameRenderer::~GameRenderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameRenderer::render(const Snake& snake, const Fruit& fruit) {
// Rendering logic goes here
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the Snake's tail segments
    for (const auto& tailSegment : snake.tail) {
        SDL_Rect tailRect = {tailSegment.x, tailSegment.y, snake.segmentSize, snake.segmentSize};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &tailRect);
    }

    // Render the Fruit
    SDL_Rect fruitRect = {fruit.x, fruit.y, fruit.segmentSize, fruit.segmentSize};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color for the fruit
    SDL_RenderFillRect(renderer, &fruitRect);

    // Update the window
    SDL_RenderPresent(renderer);
}
