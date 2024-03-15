#include "../includes/snake.h"
#include "../includes/agent.h"
#include "../includes/gym.h"
#include "../includes/game_logic.h"
#include "../includes/game_render.h"

using namespace std;
int main(){
    // Seed the random number generator
    srand(time(0));
    // Create the Q-learning agent
    double epsilon = 0.7;       // Example value, replace with the desired epsilon value
    double eps_dis = 0.99;
    double min_eps = 0.1;
    GameRenderer* render = new GameRenderer();
    QlearningAgent qAgent(epsilon, eps_dis, min_eps);
    qAgent.loadQValues("weights/longest_weights.txt");
    cout << "amount weights: "<< qAgent.qTable.size() << endl;
    // Set the learning parameters
    double learningRate = 0.1;   // Example value, replace with the desired learning rate
    double discountRate = 0.9;  // Example value, replace with the desired discount rate
    // Create the training instance
    Gym gym(&qAgent, learningRate, discountRate, epsilon);
    // Specify the number of episodes for training
    int numEpisodes = 1500;  // Example value, replace with the desired number of episodes
    // Train the agent
    gym.train(numEpisodes, render);
    return 0;
}