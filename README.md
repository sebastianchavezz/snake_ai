# Snake Game with Reinforcement Learning

## Overview
This project is a simple implementation of the classic Snake game in C++, accompanied by a reinforcement learning algorithm from scratch. It utilizes the Bellman equation for Q-learning to enable the snake agent to learn optimal strategies for navigating the game environment.

## Project Structure
.
├── includes
│ ├── agent.h
│ ├── game_logic.h
│ ├── game_render.h
│ ├── gym.h
│ └── snake.h
├── src
│ ├── agent.cpp
│ ├── game_logic.cpp
│ ├── game_render.cpp
│ ├── gym.cpp
│ └── main.cpp
├── weights
│ └── longest_weights.txt
└── Makefile

markdown
Copy code

## Components
- **Snake Game Logic (`game_logic.cpp`, `game_logic.h`):** Implements the core logic for the Snake game, including movement, collision detection, and fruit generation.
- **Game Renderer (`game_render.cpp`, `game_render.h`):** Handles rendering the game state onto the screen.
- **Reinforcement Learning Agent (`agent.cpp`, `agent.h`):** Defines the Q-learning agent responsible for learning and making decisions in the game.
- **Gym (`gym.cpp`, `gym.h`):** Provides the training environment for the reinforcement learning agent.
- **Main Program (`main.cpp`):** The entry point for the application, where the game and training process are orchestrated.

## Usage
1. **Compilation:** Run `make` to compile the project using the provided Makefile.
2. **Execution:** After compilation, execute the program using the generated executable.

## Running the Game
To run the Snake game with reinforcement learning, follow these steps:
1. Set the desired parameters such as epsilon, learning rate, and discount rate in the `main.cpp` file.
2. Run the program, which will start training the agent over a specified number of episodes.
3. Monitor the training progress and observe how the agent learns to play the game more effectively over time.

## Note
This project serves as a beginner-friendly introduction to C++ programming and reinforc
