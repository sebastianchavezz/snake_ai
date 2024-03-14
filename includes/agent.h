//agent.h
#ifndef QLEARNING_AGENT_H
#define QLEARNING_AGENT_H

#include "snake.h"
#include "game_logic.h"

// Define enums and structs used in your code

enum Action{up, down, left, right, none};

struct State {
    // need to find a way to make a State unique, the values can only be 2, -1, 1 or 0
    int c1, c2, c3, c4, c5, c6, c7, c8;
    //this values can be 0 to 640 for x and 0 to 480 for the y
    int x, y;
};
struct State_action{
    State state;
    Action action;
};
struct action_value{
    Action action;
    float value;
};
struct key_value{
    State_action pair;
    float value;
};

// Define function prototypes
bool operator==(State state1, State state2);
bool operator==(State_action sa1, State_action sa2);
bool comp_action_value(action_value pair1, action_value pair2);
bool comp_reward(float reward1, float reward2);

class QlearningAgent {
private:
    State last_state;
    Action last_action;
    double epsilon;
    double eps_dis;
    double min_eps;
    double lastDistance;

public:
    std::vector<key_value> qTable;
    QlearningAgent(double epsilon, double eps_dis, double min_eps);
    std::vector<key_value> getQTable();
    void updateEpsilon();
    void init_lastDistance();
    void update_lastDistance(double lastDistance);
    void loadQValues(const std::string& filename);
    void saveQValues(const std::string& filename);
    State getState(Snake snake, Fruit fruit, double distance);
    void updateQValue(State state, Action action, int reward, State nextState, float learningRate, float discountValue);
    float getq(State_action pair);
    Action getAction(State state);
    int getReward(Snake snake, Fruit fruit, double distance, int amount_without_food, bool dead);
    float getMaxQValue(State state);
    void performActionOnSnake(Action action, GameLogic* game);
};

#endif // QLEARNING_AGENT_H
