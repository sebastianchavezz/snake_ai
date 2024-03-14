#ifndef RENDER
#define RENDER

#include "snake.h"

class GameRenderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;

public:
    GameRenderer();
    ~GameRenderer();
    void render(const Snake& snake, const Fruit& fruit);
};

#endif