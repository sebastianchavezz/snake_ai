//src/agent.cpp
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>  // Include for rand() and srand()
#include <ctime>    // Include for seeding rand()
#include <fstream>

using namespace std;

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
    vector<Tail> tail;
};

struct State {
  int is_left;  // 1 if food is to the left of the snake's head, 0 otherwise
  int is_right; // 1 if food is to the right of the snake's head, 0 otherwise
  int is_up;    // 1 if food is above the snake's head, 0 otherwise
  int is_down;  // 1 if food is below the snake's head, 0 otherwise
};

class QlearningAgent{
private:
    vector<vector<int>> qTable;
    int numStates;
    double epsilon;
    double epsilonDiscount;
    double minEpsilon;
    int numActions;

public:
    QlearningAgent(int numStates, int numActions, double initialepsilon, double epsilonDiscount, double minEpsilon) {
        this->numStates = numStates;
        this->epsilonDiscount = epsilonDiscount;
        this->epsilon = initialepsilon;
        this->minEpsilon = minEpsilon;
        this->numActions;

        qTable.resize(numStates, vector<int>(numActions, 0)); // Initialize Q-table
    }
    void updateEpsilon(){
        epsilon = max(epsilon * epsilonDiscount, minEpsilon);
    }

    int getState(const State& state) {
        // Calculate a unique state index based on the current state
        int stateIndex = 0;

        // Adjust calculation based on your specific state representation

        // **Example calculation (replace with your actual logic):**
        stateIndex = state.is_left * 2 + state.is_right;
        stateIndex += (state.is_up * 4) + state.is_down * 8;

        // Ensure the index stays within valid range
        return stateIndex % numStates;
    }

    void updateQValue(int state, int action, int reward, int nextState, double learningRate, double discountValue){
        qTable[state][action] += learningRate*(reward + discountValue* getMaxQValue(nextState) - qTable[state][action]); 
    }

    int getAction(int state, double epsilon) {
        int action;

        // Use epsilon-greedy strategy
        if (static_cast<double>(rand()) / RAND_MAX < epsilon) {
            // Exploration: choose a random action
            action = rand() % qTable[state].size();
        } else {
            // Exploitation: choose the action with the highest Q-value
            action = 0;
            for (int i = 0; i < qTable[state].size(); ++i) {
                if (qTable[state][i] > qTable[state][action]) {
                    action = i;
                }
            }
        }

        //cout << "action= " << action << endl;
        return action;
    }

    int getMaxQValue(int state){
        int maxQValue = qTable[state][0];
        for (int i = 0; i < qTable[state].size(); ++i){
            if (qTable[state][i] > maxQValue){
                maxQValue = qTable[state][i];
            }
        }
        return maxQValue;
    }

    void saveWeights(const char* filename) {
        ofstream outFile(filename, ios::binary);
        if (!outFile.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        for (int i = 0; i < numStates; ++i) {
            outFile.write(reinterpret_cast<char*>(&qTable[i][0]), numActions * sizeof(int));
        }

        outFile.close();
        cout << "Weights saved to file: " << filename << endl;
    }

    // Method to load Q-table from a file
    void loadWeights(const char* filename) {
        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open()) {
            cerr << "Error opening file for reading: " << filename << endl;
            return;
        }

        for (int i = 0; i < numStates; ++i) {
            inFile.read(reinterpret_cast<char*>(&qTable[i][0]), numActions * sizeof(int));
        }

        inFile.close();
        cout << "Weights loaded from file: " << filename << endl;
    }
};

class Game {
private:
    Snake snake;
    Fruit fruit;
    int point;

    void checkCollision() {
    // Check if the head collides with the screen borders
        if (snake.head_y >= SCREEN_HEIGHT)
            snake.head_y = 0;
        else if (snake.head_y < 0)
            snake.head_y = SCREEN_HEIGHT - snake.segmentSize;
        else if (snake.head_x >= SCREEN_WIDTH)
            snake.head_x = 0;
        else if (snake.head_x < 0)
            snake.head_x = SCREEN_WIDTH - snake.segmentSize;
    }

    void update() {
        // Check if Snake ate the fruit
        if (snake.head_x == fruit.x && snake.head_y == fruit.y) {
            Tail tailSegment = {snake.head_x, snake.head_y};
            snake.tail.push_back(tailSegment);
            this->point += 10;
            spawnFruit();
        }
        //check if fruit pos == snake pos
        // if so, add the head of snake to the tail
        // Move the head by its direction
        // for tail segment in the tail; move
        
        
        // Update Snake's position based on its current direction
        switch (snake.direction) {
            case UP:
                snake.head_y -= snake.segmentSize;
                break;
            case DOWN:
                snake.head_y += snake.segmentSize;
                break;
            case LEFT:
                snake.head_x -= snake.segmentSize;
                break;
            case RIGHT:
                snake.head_x += snake.segmentSize;
                break;
            default:
                break;
        }

        checkCollision();

        for (size_t i = snake.tail.size() - 1; i > 0; --i){
            snake.tail[i] = snake.tail[i - 1];
            // check collision with tail 
            if (snake.head_x == snake.tail[i].x && snake.head_y == snake.tail[i].y){
                this->quit = true;
                cout << "GAME OVER, amount of points: " << this->point <<endl;
            }

        }
        Tail headSegment = {snake.head_x, snake.head_y};
        snake.tail[0] = headSegment;
    }


    void spawnFruit() {
        // Keep generating a new fruit position until it doesn't collide with the snake
        do {
            fruit.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
            fruit.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
        } while (fruitCollision());
        fruit.segmentSize = CELL_SIZE;
    }

    bool fruitCollision() {
        // Check if the new fruit position collides with any part of the snake
        for (const auto& tailSegment : snake.tail) {
            if (fruit.x == tailSegment.x && fruit.y == tailSegment.y) {
                return true; }
        }
        return false;
    }


public:
    bool quit;
    Game() : quit(false) {
        initSnake();
        spawnFruit();
        this->point = 0;
    }
    void initSnake() {
        snake.head_x = SCREEN_HEIGHT / 2;
        snake.head_y = SCREEN_WIDTH / 2;
        snake.segmentSize = CELL_SIZE;
        snake.direction = RIGHT;
        Tail headSegment = {snake.head_x, snake.head_y};
        snake.tail.push_back(headSegment);
    }
    void setGameOver(){
        this->quit = false;
    }
    bool isGameOver(){
        return this->quit;
    }
    
    State getCurrentState() const {
        State currentState;

        // Check the relative position of the head and the fruit
        currentState.is_left = (snake.head_x < fruit.x) ? 1 : 0;
        currentState.is_right = (snake.head_x > fruit.x) ? 1 : 0;
        currentState.is_up = (snake.head_y < fruit.y) ? 1 : 0;
        currentState.is_down = (snake.head_y > fruit.y) ? 1 : 0;

        return currentState;
    }

    void performAction(int action) {
        // Perform the action and return the reward
        Direction newDirection = snake.direction;

        switch (action) {
            case 0:
                newDirection = UP;
                //cout << "UP" << endl;
                break;
            case 1:
                newDirection = DOWN; 
                //cout << "DOWN" << endl;
                break;
            case 2:
                newDirection = LEFT;
                //cout << "LEFT" << endl;
                break;
            case 3:
                newDirection = RIGHT;
                //cout << "RIGHT" << endl;
                break;
            default:
                break;
        }
        //cout << "x: " << snake.head_x << "y: " << snake.head_y << endl;
        if ((newDirection == UP && snake.direction != DOWN) ||
            (newDirection == DOWN && snake.direction != UP) ||
            (newDirection == LEFT && snake.direction != RIGHT) ||
            (newDirection == RIGHT && snake.direction != LEFT)) {
            snake.direction = newDirection;
        }

        update();
    }

    int getReward() const {
        // Return the reward based on the current game state
        return this->point;
    }
};

class Training {
private:
    QlearningAgent* agent;
    Game* env;
    int state;
    double learningRate;
    double discountRate;
    double eps;
    int total_points;

public:
    Training(QlearningAgent* qAgent, double lr, double dr, double epsilon)
        : agent(qAgent), learningRate(lr), discountRate(dr), eps(epsilon), state(0) {
            agent->updateEpsilon();
            this->total_points = 0;
    }

    void train(int numEpisodes) {
        cout << "STARTING TRAINING" << endl;
        for (int episode = 1; episode <= numEpisodes; ++episode) {
            Game game;
            env = &game;
            state = agent->getState(env->getCurrentState());
            env->initSnake();
            int steps_without_food = 0;
            env->quit = false;
            while (!env->isGameOver()) {
                // Your training logic goes here
                int action = agent->getAction(state, eps);
                //cout << "action :" << action << endl;
                env->performAction(action);
                int nextState = agent->getState(env->getCurrentState());
                //cout << "nextState: " << nextState << endl;
                int reward = env->getReward();
                
                agent->updateQValue(state, action, reward, nextState, learningRate, discountRate);

                state = nextState;
                if (reward == 0){
                    steps_without_food = steps_without_food + 1;
                }else{
                    steps_without_food = 0;
                }
                //cout << env->isGameOver() << endl;
                //cout << "steps without food: "<< steps_without_food<<endl; 
                if (steps_without_food > 100){
                    break;
                }
                //cout << env->isGameOver() << endl;
                //cout << "Points: " << env->getReward() << endl;
            }
            total_points += env->getReward();
            cout << "Episode: " << episode << ", Total Points: " << total_points << endl;
            agent->updateEpsilon();
        }
        agent->saveWeights("q_table.bin");
    }
};

int main() {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));

    // Create the Q-learning agent
    int numStates = 4;
    int numActions = 4;   // Example value, replace with the actual number of actions
    double eps= 1.0;
    double eps_dis = 0.9992;
    double min_eps = 0.001;
    QlearningAgent qAgent(numStates, numActions, eps, eps_dis, min_eps);

    // Create the game environment
    Game game;

    // Set the learning parameters
    double learningRate = 0.001;   // Example value, replace with the desired learning rate
    double discountRate = 0.9;  // Example value, replace with the desired discount rate
    double epsilon = 0.1;       // Example value, replace with the desired epsilon value
    
    // Create the training instance
    Training training(&qAgent, learningRate, discountRate, epsilon);

    // Specify the number of episodes for training
    int numEpisodes = 100000;  // Example value, replace with the desired number of episodes
    
    // Train the agent
    training.train(numEpisodes);

    return 0;
}
