#include "../includes/agent.h"
#include "../includes/game_logic.h"


GameLogic::GameLogic() : gameOver(false) {
    snake.head_x = SCREEN_HEIGHT / 2;
    snake.head_y = SCREEN_WIDTH / 2;
    snake.segmentSize = CELL_SIZE;
    snake.direction = RIGHT;
    snake.xdir = 1;
    snake.ydir = 0;
    Tail headSegment = {snake.head_x, snake.head_y};
    snake.tail.push_back(headSegment);
    this->point = 0;
    is_food_eaten = false;
    spawnFruit();
}

void GameLogic::checkCollision() {
    if (snake.head_y >= SCREEN_HEIGHT)
        snake.head_y = 0;
    else if (snake.head_y < 0)
        snake.head_y = SCREEN_HEIGHT - snake.segmentSize;
    else if (snake.head_x >= SCREEN_WIDTH)
        snake.head_x = 0;
    else if (snake.head_x < 0)
        snake.head_x = SCREEN_WIDTH - snake.segmentSize;
}

void GameLogic::spawnFruit() {
    do {
        fruit.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
        fruit.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
    } while (fruitCollision());
    fruit.segmentSize = CELL_SIZE;
}

bool GameLogic::fruitCollision() {
    for (const auto& tailSegment : snake.tail) {
        if (fruit.x == tailSegment.x && fruit.y == tailSegment.y) {
            return true;
        }
    }
    return false;
}
void GameLogic::left(){
    Direction newDirection = snake.direction;
    newDirection = LEFT;
    if (newDirection == LEFT && snake.direction != RIGHT) snake.direction = newDirection;
}
void GameLogic::right(){
    Direction newDirection = snake.direction;
    newDirection = RIGHT;
    if (newDirection == RIGHT && snake.direction != LEFT) snake.direction = newDirection;
}
void GameLogic::up(){
    Direction newDirection = snake.direction;
    newDirection = UP;
    if (newDirection == UP && snake.direction != DOWN) snake.direction = newDirection;
}
void GameLogic::down(){
    Direction newDirection = snake.direction;
    newDirection = DOWN;
    if (newDirection == DOWN && snake.direction != UP) snake.direction = newDirection;
}

void GameLogic::update() {
// Game logic goes here
    // Check if Snake ate the fruit
    if (snake.head_x == fruit.x && snake.head_y == fruit.y) {
        Tail tailSegment = {snake.head_x, snake.head_y};
        snake.tail.push_back(tailSegment);
        this->point += 10;
        spawnFruit();
        is_food_eaten = true;
    }
    
    // Update Snake's position based on its current direction
    switch (snake.direction) {
        case UP:
            snake.ydir = -1;
            snake.xdir = 0;
            break;
        case DOWN:
            snake.ydir = 1;
            snake.xdir = 0;
            break;
        case LEFT:
            snake.xdir = -1;
            snake.ydir = 0;
            break;
        case RIGHT:
            snake.xdir = 1;
            snake.ydir = 0;
            break;
        default:
            break;
    }
    snake.head_x += snake.xdir * snake.segmentSize;
    snake.head_y += snake.ydir * snake.segmentSize;

    checkCollision();

    for (size_t i = snake.tail.size() - 1; i > 0; --i){
        snake.tail[i] = snake.tail[i - 1];
        // check collision with tail 
        if (snake.head_x == snake.tail[i].x && snake.head_y == snake.tail[i].y){
            this->gameOver = true;
            //cout << "GAME OVER, amount of points: " << this->point <<endl;
        }

    }
    Tail headSegment = {snake.head_x, snake.head_y};
    snake.tail[0] = headSegment;
}

bool GameLogic::isFoodEaten() {
    return is_food_eaten;
}
