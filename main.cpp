#include <iostream>
#include <vector>
#include "grid.h"
#include "driver.h"
using namespace std;

const int GENERATIONS = 5000;
const int TIME_STEPS = 60;
const int REPLAY_SECONDS_PER_GEN = 3;
const int POPULATION_SIZE = 150;
const int BRAIN_SIZE = 3;
const int BOARD_SIZE = 40;
const float MUTATION_FACTOR = 0.1;
const int BOUNDARY_CTR_X = 0 + 2;
const int BOUNDARY_CTR_Y = BOARD_SIZE - 2;
// const int SAVED_GENERATIONS = 10;
// const int SAVED_GENS[SAVED_GENERATIONS] = {0,1,2,3,5,8,13,21,34,55};
int main() {
    evolver d;
    d.set_generations(GENERATIONS);
    d.set_time_steps(TIME_STEPS);
    d.set_replay_time(REPLAY_SECONDS_PER_GEN);
    d.set_world_size(BOARD_SIZE);
    d.set_brain_size(BRAIN_SIZE);
    d.set_mutation_rate(MUTATION_FACTOR);
    coordinate origin = {BOUNDARY_CTR_X, BOUNDARY_CTR_Y};
    coordinate origin2 = {BOUNDARY_CTR_Y, BOUNDARY_CTR_X};
    d.set_boundary_rectangle(origin, 3, 3);
    d.set_boundary_rectangle(origin2, 6, 6);
    d.add_creatures(POPULATION_SIZE);
    d.save_squares();
    d.run();
    
    d.play_replay(cout);
    return 0;
}

