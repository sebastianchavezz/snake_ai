#include "snake.h"

class GameLogic {
private:

    void checkCollision();
    void spawnFruit();
    bool fruitCollision();

public:
    bool gameOver;
    Snake snake;
    Fruit fruit;
    int point;
    bool is_food_eaten;
    GameLogic();
    void update();
    void left();
    void right();
    void up();
    void down();
    bool isFoodEaten();
    int getReward() const;
};