#ifndef GYM
#define GYM

#include "agent.h"
#include "game_logic.h"

class Gym {
private:
    QlearningAgent* agent;
    GameLogic* env;
    double learningRate;
    double discountRate;
    double eps;
    int total_points;
    long long gencount;
    long long eatcount;
    long long deathcount;
    int longest_tail;

public:
    Gym(QlearningAgent* agent);
    void train(int numEpisodes);
    void test();
};

#endif