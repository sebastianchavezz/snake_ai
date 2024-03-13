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
    int xdir;
    int ydir;
    vector<Tail> tail;
};

enum Action{up, down, left, right};


struct State {
    // need to find a way to make a State unique, the values can only be 2, -1, 1 or 0
    int c1, c2, c3, c4, c5, c6, c7, c8;
    //this values can be 0 to 640 for x and 0 to 480 for the y
    int x, y;
};


bool operator==(State state1, State state2){
    return (
        state1.c1 == state2.c1 &&
        state1.c2 == state2.c2 &&
        state1.c3 == state2.c3 &&
        state1.c4 == state2.c4 &&
        state1.c5 == state2.c5 &&
        state1.c6 == state2.c6 &&
        state1.c7 == state2.c7 &&
        state1.c8 == state2.c8 &&
        state1.x == state2.x && 
        state1.y == state2.y 
    );
}


struct State_action{
    State state;
    Action action;
};

bool operator==(State_action sa1, State_action sa2){
    return (sa1.state == sa2.state && sa1.action == sa2.action);
}

struct action_value{
    Action action;
    float value;
};
struct key_value{
    State_action pair;
    float value;
};

bool comp_action_value(action_value pair1, action_value pair2) {
    return pair1.value < pair2.value;
}
class QlearningAgent{
private:
vector<key_value> qTable;
State last_state;
Action last_action;
double epsilon;
double eps_dis;
double min_eps;
double lastDistance;

public:
    QlearningAgent(double epsilon, double eps_dis, double min_eps) {
        this->epsilon = epsilon;
        this->eps_dis = eps_dis;
        this->min_eps = min_eps;
        init_lastDistance();
    }
    vector<key_value> getQTable(){
        return qTable;
    }
    
    void updateEpsilon(){
        this->epsilon = max(this->epsilon * this->eps_dis, this->min_eps);
    }
    void init_lastDistance(){
        this->lastDistance = 0.0;
    }

    void update_lastDistance(double lastDistance){
        this->lastDistance =  lastDistance;
    }

    State getState(Snake snake, Fruit fruit, double distance){
        int new_state[9];
        int index = 0;
        //double distance = sqrt(pow(snake.x - fruit.x, 2) + pow(snake.y - fruit.y, 2));

        // array arround the head
        for(int y= -1; y <= 1; y++ ){
            for (int x = -1; x <= 1; x++){
                // the head has a x,y of 0,0 so skip it 
                bool calculated = false;
                if (y == 0 && x == 0) continue;
                int x_block = snake.head_x + x * CELL_SIZE;
                int y_block = snake.head_y + y * CELL_SIZE;

                for (Tail tail: snake.tail){
                    // check where the tail is.
                    if (x_block == tail.x && y_block == tail.y){
                        new_state[index] = 0;
                        calculated = true;
                    }
                }
                if (fruit.x == x_block && fruit.y == y_block){
                    new_state[index] = 2;
                    calculated = true;
                }
                if (!calculated){
                    double new_distance = sqrt(pow(x_block - fruit.x, 2) + pow(y_block - fruit.y, 2));
                    if(new_distance <= distance) new_state[index] = 1;
                    else new_state[index] = -1;
                }
                index++;
            }
        }
        return State{new_state[0],new_state[1],new_state[2],new_state[3],new_state[4],new_state[5],new_state[6],new_state[7], snake.xdir, snake.ydir};

    }

    void updateQValue(State state, Action action, int reward, State nextState, double learningRate, double discountValue){
        State_action sa = {state, action};
        bool found = false;
        // for every Q(s, a) pair 
        for (auto& item: qTable){
            // check if state and action are being saved somewhere
            if (item.pair == sa){
                // if so update the value -> Q(s, a)
                float newValue = (1 - learningRate) * item.value + learningRate * (reward + discountValue* getMaxQValue(nextState));
                item.value = newValue;
                found = true;
                break;
            }
        }
        // if we dont find the Q(s, a) -> add a new pair to the Table
        if (!found) {
            qTable.push_back({sa, learningRate * (reward + discountValue * getMaxQValue(nextState))});
        }
    }
    float getq(State_action pair){
        int x = 0;
        for (auto item: qTable){
            if(item.pair == pair){
                break;
            }
            x++;
        }
        if(x >= qTable.size()) return 0.0;
        return qTable.at(x).value;
    }

    Action getAction(State state, double epsilon) {
        // Exploration
        if ((random() % 100) < epsilon)
        {
            switch (random() % 4)
            {
                case 0:
                    return Action::left;
                case 1:
                    return Action::right;
                case 2:
                    return Action::up;
                case 3:
                    return Action::down;
            }
        }
        vector<action_value> qvals;
        qvals.push_back({Action::down, this->getq({state, Action::down})});
        qvals.push_back({Action::up, this->getq({state, Action::up})});
        qvals.push_back({Action::left, this->getq({state, Action::left})});
        qvals.push_back({Action::right, this->getq({state, Action::right})});
        action_value *x, *y;
        sort(qvals.begin(), qvals.end(), comp_action_value);
        vector<action_value> maxqs;
        for (auto qs :qvals)
        {
            if (qs.value == qvals[0].value)
            {
                maxqs.push_back(qs);
            }
        }
        unsigned long index = random() % maxqs.size();
        return maxqs[index].action;
    }

    int getReward(Snake snake, Fruit fruit,double distance, int amount_without_food, bool dead){
        int reward = 0;
        if(dead){
            reward -= 5000;
        }

        if (snake.head_x == fruit.x && snake.head_y == fruit.y){
            reward += 1000;
        }
        // Reward for moving closer to the food
        if (distance < this->lastDistance) {
            reward += 10;
        }
        // Reward for not moving away from the food
        if (distance == this->lastDistance) {
            reward += 5;
        }
        // Penalty for not eating food for a while
    
        // Additional reward or penalty conditions can be added as needed
        return reward;
    }

    float getMaxQValue(State state){
        // returns the MAX Q value for a given state
        float maxQValue = -numeric_limits<float>::infinity();
        for (const auto& item : qTable) {
            if (item.pair.state == state && item.value > maxQValue) {
                maxQValue = item.value;
            }
        }
        return maxQValue;
    }
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int point;
    int frameDelay;


    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                Direction newDirection = snake.direction;

                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        newDirection = UP;
                        break;
                    case SDLK_DOWN:
                        newDirection = DOWN;
                        break;
                    case SDLK_LEFT:
                        newDirection = LEFT;
                        break;
                    case SDLK_RIGHT:
                        newDirection = RIGHT;
                        break;
                    case SDLK_SPACE:  // Toggle between two frame rates
                        if (frameDelay == 130) {
                            frameDelay = 15;
                        } else {
                            frameDelay = 130;
                        }
                        break;
                    default:
                        break;
                }

                if ((newDirection == UP && snake.direction != DOWN) ||
                    (newDirection == DOWN && snake.direction != UP) ||
                    (newDirection == LEFT && snake.direction != RIGHT) ||
                    (newDirection == RIGHT && snake.direction != LEFT)) {
                    snake.direction = newDirection;
                }
            }
        }
    }

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
Snake snake;
Fruit fruit;
bool is_food_eaten;

    Game() : quit(false), frameDelay(130) {
//      if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//          cerr << "SDL initialization failed: " << SDL_GetError() << endl;
//          exit(1);
//      }

//      window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

//      if (!window) {
//          cerr << "Window creation failed: " << SDL_GetError() << endl;
//          exit(1);
//      }

//      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//      if (!renderer) {
//          cerr << "Renderer creation failed: " << SDL_GetError() << endl;
//          exit(1);
//      }

        initSnake();
        spawnFruit();
    }
    
    void update() {
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
                snake.head_y -= snake.segmentSize;
                break;
            case DOWN:
                snake.ydir = 1;
                snake.xdir = 0;
                snake.head_y += snake.segmentSize;
                break;
            case LEFT:
                snake.xdir = -1;
                snake.ydir = 0;
                snake.head_x -= snake.segmentSize;
                break;
            case RIGHT:
                snake.xdir = 1;
                snake.ydir = 0;
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
    bool isFoodEaten(){
        return is_food_eaten;
    }

    void render() {
        // Clear the screen
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
    void initSnake() {
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
    }
    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    void setGameOver(){
            this->quit = false;
        }
        bool isGameOver(){
            return this->quit;
        }
        
        State getCurrentState() const {
            State currentState;
            // create a get State

            return currentState;
    }

    void performAction(Action action) {
        // Perform the action and return the reward
        Direction newDirection = snake.direction;

        switch (action) {
            case Action::up:
                newDirection = UP;
                //cout << "UP" << endl;
                break;
            case Action::down:
                newDirection = DOWN; 
                //cout << "DOWN" << endl;
                break;
            case Action::left:
                newDirection = LEFT;
                //cout << "LEFT" << endl;
                break;
            case Action::right:
                newDirection = RIGHT;
                //cout << "RIGHT" << endl;
                break;
            default:
                break;
        }
        //cout << "x: " << snake.head_x << "y: " << snake.head_y << endl;
        snake.direction = newDirection;

        update();
    }

    int getReward() const {
        // Return the reward based on the current game state
        return this->point;
    }
    void run() {
        while (!quit) {
            handleEvents();
            update();
            render();
            SDL_Delay(frameDelay);
        }
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
    long long gencount;
    long long eatcount;
    long long deathcount;
    int longest_tail;


public:
    Training(QlearningAgent* qAgent, double lr, double dr, double epsilon)
        : agent(qAgent), learningRate(lr), discountRate(dr), eps(epsilon), state(0) {
            agent->updateEpsilon();
            this->total_points = 0;
            deathcount = 0;
            eatcount =0;
            longest_tail = 0;
    }

    void printStatistics(long long gencount, int longest_tail,long long total_reward, long long eatcount, long long deathcount, State newstate, vector<key_value> qTable, Fruit fruit){
        if (gencount % 100 == 0) {
            //system("clear");
            printf("turncount:- %lld \nstate:- {%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}\n", gencount, newstate.c1,
                   newstate.c2, newstate.c3, newstate.c4, newstate.c5, newstate.c6, newstate.c7, newstate.c8,
                   newstate.x, newstate.y);
            printf("Fruit X    : %d\n", fruit.x);
            printf("Fruit Y    : %d\n", fruit.y);
            printf("Longest Tail     : %d\n",longest_tail);
            printf("eatcount   : %lld\n", eatcount);
            printf("total_reward   : %lld\n", total_reward);
            printf("deathcount : %lld\n", deathcount);
            printf("q length : %ld s \n", qTable.size());
            printf("=======================================================\n\n");
        }
    }

    void train(int numEpisodes) {
        cout << "STARTING TRAINING" << endl;
        for (int episode = 1; episode <= numEpisodes; ++episode) {
            Game game;
            env = &game;
            long long total_reward = 0;
            int amount_without_food = 0;
            // init lastDistance
            agent->init_lastDistance();
            //init currentState
            double distance1 = sqrt(pow(env->snake.head_x - env->fruit.x, 2) + pow(env->snake.head_y - env->fruit.y, 2));
            State currentState = agent->getState(env->snake, env->fruit, distance1);

            // Reset the environment to the initial state
            // Perform actions until the game is over
            while (!env->isGameOver()) {
                // Get the current state from the environment

                double distance = sqrt(pow(env->snake.head_x - env->fruit.x, 2) + pow(env->snake.head_y - env->fruit.y, 2));
                State nextState = agent->getState(env->snake, env->fruit, distance);

                // Choose an action using epsilon-greedy policy
                Action chosenAction = agent->getAction(nextState, this->eps);

                // Perform the chosen action in the environment
                env->performAction(chosenAction);

                // Get the reward for the action
                int reward = agent->getReward(env->snake, env->fruit ,distance, amount_without_food,env->isGameOver());
                total_reward += reward;

                agent->updateQValue(currentState, chosenAction, reward, nextState, learningRate, discountRate);

                // Update epsilon (exploration rate)
                agent->updateEpsilon();

                if (env->isFoodEaten()) {
                    amount_without_food = 0;
                    eatcount++;
                    // reinit to false
                    env->is_food_eaten = false;
                } else {
                    amount_without_food++;
                }
                if (amount_without_food > 1000){
                    break;
                }
                //printf("reward %d\n", total_reward);
                currentState = nextState;
                //env->render();
                //SDL_Delay(5);

            }
            deathcount++;
            longest_tail = max(static_cast<int>(longest_tail), static_cast<int>(env->snake.tail.size()));

            printStatistics(episode, longest_tail,total_reward,eatcount, deathcount, currentState, agent->getQTable(), env->fruit);

            // Print the total reward obtained in this episode
            //cout << "Episode " << episode << " Total Reward: " << total_reward << endl;
            // Update the total points
            this->total_points += total_reward;
        }

        // Print the average reward obtained over all episodes
        cout << "Average Reward: " << this->total_points / numEpisodes << endl;
    }

};

int main() {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));
    // Create the Q-learning agent
    double eps= 0.2;
    double eps_dis = 0.9992;
    double min_eps = 0.001;
    QlearningAgent qAgent(eps, eps_dis, min_eps);
    // Set the learning parameters
    double learningRate = 0.1;   // Example value, replace with the desired learning rate
    double discountRate = 0.9;  // Example value, replace with the desired discount rate
    double epsilon = 0.1;       // Example value, replace with the desired epsilon value
    // Create the training instance
    Training training(&qAgent, learningRate, discountRate, epsilon);
    // Specify the number of episodes for training
    int numEpisodes = 2000;  // Example value, replace with the desired number of episodes
    // Train the agent
    training.train(numEpisodes);
    return 0;
}
