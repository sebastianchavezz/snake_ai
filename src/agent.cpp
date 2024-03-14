
#include "../includes/agent.h"

using namespace std;

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

bool operator==(State_action sa1, State_action sa2){
    return (sa1.state == sa2.state && sa1.action == sa2.action);
}


bool comp_action_value(action_value pair1, action_value pair2) {
    return pair1.value > pair2.value;
}

bool comp_reward(float reward1, float reward2){
    return reward1 < reward2;
}
QlearningAgent::QlearningAgent(double epsilon, double eps_dis, double min_eps) {
    this->epsilon = epsilon;
    this->eps_dis = eps_dis;
    this->min_eps = min_eps;
    init_lastDistance();
}
vector<key_value> QlearningAgent::getQTable()
{
    return qTable;
}

void QlearningAgent::updateEpsilon(){
    this->epsilon = max(this->epsilon * this->eps_dis, this->min_eps);
}
void QlearningAgent::init_lastDistance(){
    this->lastDistance = 0.0;
}

void QlearningAgent::update_lastDistance(double lastDistance){
    //cout << "FROM DISTANCE: " << this->lastDistance << endl;
    this->lastDistance =  lastDistance;
    //cout << "updated distance: "<<this->lastDistance<<endl;
}

void QlearningAgent::loadQValues(const string& filename) { 
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

void QlearningAgent::saveQValues(const string& filename)  {
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


State QlearningAgent::getState(Snake snake, Fruit fruit, double distance){
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

void QlearningAgent::updateQValue(State state, Action action, int reward, State nextState, float learningRate, float discountValue){
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
float QlearningAgent::getq(State_action pair){
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

Action QlearningAgent::getAction(State state) {
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

int QlearningAgent::getReward(Snake snake, Fruit fruit,double distance, int amount_without_food, bool dead){
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
float QlearningAgent::getMaxQValue(State state){
    // returns the MAX Q value for a given state
    float maxQValue = 0.0;
    for (const auto& item : qTable) {
        if (item.pair.state == state && item.value > maxQValue) {
            maxQValue = item.value;
        }
    }
    return maxQValue;
}

void QlearningAgent::performActionOnSnake(Action action, GameLogic* game){
    switch (action) {
        case Action::up:
            game->up();
            break;
        case Action::down:
            game->down();
            break;
        case Action::left:
            game->left();
            break;
        case Action::right:
            game->right();
            break;
        case Action::none:
            break;
        default:
        break;
    }
}
