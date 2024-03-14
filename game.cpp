
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
    }
    void initSnake() {
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