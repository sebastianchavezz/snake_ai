#include "../includes/gym.h"
#include "../includes/game_render.h"
#include "../includes/agent.h"
#include "../includes/snake.h"
#include "../includes/game_logic.h"

using namespace std;

Gym::Gym(QlearningAgent* agent, double lr, double dis_rate, double eps){
    this->agent = agent;
    this->learningRate = lr;
    this->discountRate = dis_rate;
    this->eps = eps;
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

void Gym::train(int numEpisodes, GameRenderer* render){
        cout << "STARTING TRAINING" << endl;
        for (int episode = 1; episode <= numEpisodes; ++episode) {
            GameLogic *game_logic = new GameLogic();
            long long total_reward = 0;
            int amount_without_food = 0;
            // init lastDistance
            agent->init_lastDistance();
            //init currentState
            double distance1 = sqrt(pow(game_logic->snake.head_x - game_logic->fruit.x, 2) + pow(game_logic->snake.head_y - game_logic->fruit.y, 2));
            State currentState = agent->getState(game_logic->snake, game_logic->fruit, distance1);

            // Reset the game_logicironment to the initial state
            // Perform actions until the game is over
            while (!game_logic->gameOver) {
                // Get the current state from the game_logicironment

                double distance = sqrt(pow(game_logic->snake.head_x - game_logic->fruit.x, 2) + pow(game_logic->snake.head_y - game_logic->fruit.y, 2));
                State nextState = agent->getState(game_logic->snake, game_logic->fruit, distance);

                // Choose an action using epsilon-greedy policy
                Action chosenAction = agent->getAction(nextState);

                // Perform the chosen action in the game_logicironment
                agent->performActionOnSnake(chosenAction, game_logic);

                //update screen
                game_logic->update();
                // Get the reward for the action
                int reward = agent->getReward(game_logic->snake, game_logic->fruit ,distance, amount_without_food,game_logic->gameOver);
                total_reward += reward;

                agent->updateQValue(currentState, chosenAction, reward, nextState, learningRate, discountRate);

                // Update epsilon (exploration rate)
                agent->updateEpsilon();

                if (game_logic->isFoodEaten()) {
                    amount_without_food = 0;
                    eatcount++;
                    // reinit to false
                    game_logic->is_food_eaten = false;
                } else {
                    amount_without_food++;
                }
                if (amount_without_food > 200){
                    break;
                }
                //printf("reward %d\n", total_reward);
                currentState = nextState;
                agent->update_lastDistance(distance);
                render->render(game_logic->snake, game_logic->fruit, 10);
                

            }
            deathcount++;
            longest_tail = max(static_cast<int>(longest_tail), static_cast<int>(game_logic->snake.tail.size()));

            printStatistics(episode, longest_tail,total_reward,eatcount, deathcount, currentState, agent->getQTable(), game_logic->fruit);
            // Print the total reward obtained in this episode
            //cout << "Episode " << episode << " Total Reward: " << total_reward << endl;
            // Update the total points
            this->total_points += total_reward;
        }

        // Print the average reward obtained over all episodes
        cout << "Average Reward: " << this->total_points / numEpisodes << endl;
        agent->saveQValues("longest_weights.txt");
}