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

enum Action{up, down, left, right, none};


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
    return pair1.value > pair2.value;
}

bool comp_reward(float reward1, float reward2){
    return reward1 < reward2;
}
class QlearningAgent{
private:
State last_state;
Action last_action;
double epsilon;
double eps_dis;
double min_eps;
double lastDistance;

public:
vector<key_value> qTable;
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
        //cout << "FROM DISTANCE: " << this->lastDistance << endl;
        this->lastDistance =  lastDistance;
        //cout << "updated distance: "<<this->lastDistance<<endl;
    }

    void loadQValues(const string& filename) { 
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "Error: Unable to open file " << filename << endl;
            return;
        }

        qTable.clear(); // Clear the existing Q-table

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            // Parse the state components
            State state;
            state.c1 = stoi(tokens[0]);
            state.c2 = stoi(tokens[1]);
            state.c3 = stoi(tokens[2]);
            state.c4 = stoi(tokens[3]);
            state.c5 = stoi(tokens[4]);
            state.c6 = stoi(tokens[5]);
            state.c7 = stoi(tokens[6]);
            state.c8 = stoi(tokens[7]);
            state.x = stoi(tokens[8]);
            state.y = stoi(tokens[9]);

            // Parse the action and value
            Action action = static_cast<Action>(stoi(tokens[10]));
            float value = stof(tokens[11]);

            // Add the state-action pair and value to the Q-table
            qTable.push_back({{state, action}, value});
        }

        inFile.close();
        cout << "Q-values loaded from " << filename << endl;
    }

    void saveQValues(const string& filename)  {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "Error: Unable to open file " << filename << endl;
            return;
        }

        for (const auto& keyValue : qTable) {
            const auto& state = keyValue.pair.state;
            const auto& action = keyValue.pair.action;
            const auto& value = keyValue.value;
            outFile << state.c1 << "," << state.c2 << "," << state.c3 << "," << state.c4 << ","
                    << state.c5 << "," << state.c6 << "," << state.c7 << "," << state.c8 << ","
                    << state.x << "," << state.y << "," << action << "," << value << endl;
        }

        outFile.close();
        cout << "Q-values saved to " << filename << endl;
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
//              cout << "(x: "<< x << ") - (y: "<<y<<")"<<endl;
//              cout << "(snake_x: "<< snake.head_x << ") - (snake_y: "<< snake.head_y<<")"<<endl;
                int x_dir = x;
                int y_dir = y;
                //realative to the head of the snake
                int delta_snake_x = snake.head_x + x_dir * CELL_SIZE;
                int delta_snake_y = snake.head_y + y_dir * CELL_SIZE;
//              cout << "----------------------------------------" <<endl;
                // loop through the whole field to find food
                while (delta_snake_x < SCREEN_WIDTH && delta_snake_x > 0 && delta_snake_y < SCREEN_HEIGHT && delta_snake_y> 0){
//                  cout << "x_dir, y_dir: " << x_dir << " ,  " << y_dir << endl;
//                  cout << "DELTA Position: (" << delta_snake_x << ", " << delta_snake_y << ")" << endl; 
//                  cout << "Fruit Position: (" << fruit.x << ", " << fruit.y << ")" << endl; 
//                  cout << "loop Position: (" << x_dir * CELL_SIZE << ", " << y_dir * CELL_SIZE << ")" << endl; 
                    if (fruit.x == delta_snake_x && fruit.y == delta_snake_y){
                        new_state[index] = 2;
                       //cout << "fruittt" << endl;
                        calculated = true;
                        break;
                    }
                    x_dir += x;
                    y_dir += y;

                    //update the relative values of the snake head
                    delta_snake_x = snake.head_x + x_dir * CELL_SIZE;
                    delta_snake_y = snake.head_y + y_dir * CELL_SIZE;
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

    void updateQValue(State state, Action action, int reward, State nextState, float learningRate, float discountValue){
        State_action sa = {state, action};
        bool found = false;
        // for every Q(s, a) pair 
        float maxNextQValue = getMaxQValue(nextState);
        for (auto& item: qTable){
            // check if state and action are being saved somewhere
            if (item.pair == sa){
                // if so update the value -> Q(s, a)
                float oldValue = item.value;
                //float newValue = (1 - learningRate) * oldValue + learningRate * (reward + discountValue * maxNextQValue);
                float newValue = oldValue + learningRate * (reward + discountValue * maxNextQValue - oldValue);
                //cout << "newValue: "<< newValue <<", oldValue: "<< oldValue << endl;
                item.value = newValue;
                found = true;
                break;
            }
        }
        // if we dont find the Q(s, a) -> add a new pair to the Table
        if (!found) {
            qTable.push_back({sa, learningRate * (reward + discountValue * maxNextQValue)});
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

    Action getAction(State state) {
        // Exploration
        if ((random() % 100) < this->epsilon)
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
                default:
                    return Action::none;
            }
        }
        vector<action_value> qvals;
        qvals.push_back({Action::down, this->getq({state, Action::down})});
        qvals.push_back({Action::up, this->getq({state, Action::up})});
        qvals.push_back({Action::left, this->getq({state, Action::left})});
        qvals.push_back({Action::right, this->getq({state, Action::right})});
        qvals.push_back({Action::none, this->getq({state, Action::none})});
        action_value *x, *y;
        sort(qvals.begin(), qvals.end(), comp_action_value);
//      for (const auto& qval : qvals) {
//          cout << "Action: " << qval.action << ", Value: " << qval.value << endl;
//      }
        return qvals[0].action;
    }

    int getReward(Snake snake, Fruit fruit,double distance, int amount_without_food, bool dead){
        int reward = 0;

        // Distance-based rewards
        if (distance < this->lastDistance) {
            // Reward the snake for moving closer to the fruit
            reward += 100;
            //cout << "CLOSER: "<< reward << endl;
        } else if (distance > this->lastDistance) {
            // Penalize the snake for moving away from the fruit
            reward -= 10;
            //cout << "AWAY: "<< reward << endl;
        }

        // Eating fruit reward
        if (snake.head_x == fruit.x && snake.head_y == fruit.y) {
            // Reward the snake for eating the fruit
            reward += 2000;
        }

        // Death penalty
        if (dead) {
            // Penalize the snake heavily for dying
            reward -= 50;
            //cout << "--DEAD--: " << reward << endl;
        }
        // Encourage continuous movement and exploration
        if (amount_without_food > 0){
            reward -= amount_without_food;
        }
        return reward;
}
//  float getMaxQValue(State state){ 
//      vector<float> qvals;
//      qvals.push_back(this->getq({state, Action::down}));
//      qvals.push_back(this->getq({state, Action::up}));
//      qvals.push_back(this->getq({state, Action::left}));
//      qvals.push_back(this->getq({state, Action::right}));
//      sort(qvals.begin(), qvals.end(), comp_reward);
//      return qvals[0];
//  }
    float getMaxQValue(State state){
        // returns the MAX Q value for a given state
        float maxQValue = 0.0;
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
                this->quit = true;
                //cout << "GAME OVER, amount of points: " << this->point <<endl;
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
    float learningRate;
    float discountRate;
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
                Action chosenAction = agent->getAction(nextState);

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
                if (amount_without_food > 200){
                    break;
                }
                //printf("reward %d\n", total_reward);
                currentState = nextState;
                //env->render();
                //SDL_Delay(50);
                agent->update_lastDistance(distance);

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
        agent->saveQValues("longest_weights.txt");
    }
    

};

  int main() {
    // Seed the random number generator
    srand(time(0));
    // Create the Q-learning agent
    double epsilon = 0.7;       // Example value, replace with the desired epsilon value
    double eps_dis = 0.99;
    double min_eps = 0.1;
    QlearningAgent qAgent(epsilon, eps_dis, min_eps);
    qAgent.loadQValues("longest_weights.txt");
    cout << "amount weights: "<< qAgent.qTable.size() << endl;
    // Set the learning parameters
    double learningRate = 0.1;   // Example value, replace with the desired learning rate
    double discountRate = 0.9;  // Example value, replace with the desired discount rate
    // Create the training instance
    Training training(&qAgent, learningRate, discountRate, epsilon);
    // Specify the number of episodes for training
    int numEpisodes = 1500;  // Example value, replace with the desired number of episodes
    // Train the agent
    training.train(numEpisodes);
    return 0;
  }

//int main() {
//  // Seed the random number generator
//  srand(static_cast<unsigned>(time(0)));
//  
//  // Create the Q-learning agent
//  double epsilon = 0.1;       // Example value, replace with the desired epsilon value
//  double eps_dis = 0.9992;
//  double min_eps = 0.001;
//  QlearningAgent qAgent(epsilon, eps_dis, min_eps);

//  // Load the Q-values from a file
//  qAgent.loadQValues("longest_weights.txt");
//  
//  // Create a game instance
//  Game game;
//  
//  // Let the snake play using the learned Q-values
//  while (!game.isGameOver()) {
//      // Get the current state of the game
//      State currentState = game.getCurrentState();
//      
//      // Choose an action using the learned Q-values
//      Action chosenAction = qAgent.getAction(currentState);
//      
//      // Perform the chosen action
//      game.performAction(chosenAction);
//      
//      // Render the game
//      game.render();
//      
//      // Delay to control the frame rate
//      SDL_Delay(50);
//  }

//  return 0;