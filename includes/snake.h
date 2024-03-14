//snake.h
#ifndef SNAKE
#define SNAKE

#include <random>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>  // Include for rand() and srand()
#include <ctime>    // Include for seeding rand()
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Tail{
    int x, y;
};

struct Fruit {
    int x, y;
    int segmentSize;
};

struct Snake {
    int head_x;
    int head_y;
    int segmentSize;
    Direction direction;
    int xdir;
    int ydir;
    std::vector<Tail> tail;
};

#endif