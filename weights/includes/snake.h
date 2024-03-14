#ifndef SNAKE
#define SNAKE
//src/agent.cpp
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

#include "agent.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

struct State {
    int c1, c2, c3, c4, c5, c6, c7, c8, x, y;
};
struct Tail {
    int x, y;
};
struct Snake {
    int head_x, head_y, xdir, ydir;
    std::vector<Tail> tail;
};
struct Fruit {
    int x, y;
};

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
    vector<Tail> tail;
};

#endif