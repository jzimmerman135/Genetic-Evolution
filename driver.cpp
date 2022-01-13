#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <thread>
#include <chrono>
#include "driver.h"
#include "grid.h"
#include "creature.h"
using namespace std;

void population::clear() {
    //cerr << "called " << "population::clear" << "()" << endl;
    size = 0;
    generation = 0;
    survival = 0;
    population_genomes.clear();
    replay.clear();
}

evolver::evolver() {
    //cerr << "called " << "evolver" << "()" << endl;
    m_num_creatures = 0;
    m_num_generations = 0;
    m_time_steps = 0;
    m_survival_rate = 0;
    m_brain_size = 0;
    m_replay_time = 3000;
    m_summary = "";
}

evolver::~evolver() {
    //cerr << "called " << "~evolver" << "()" << endl;
    if (world != nullptr)
        delete world;
}

/********************************* SETTERS ***********************************/

//length of a generation
void evolver::set_time_steps(const int& steps) {
    //cerr << "called " << "set_time_steps" << "()" << endl;
    m_time_steps = steps;
}

//generations in simulation
void evolver::set_generations(const int& num_generations) {
    //cerr << "called " << "set_generations" << "()" << endl;
    m_num_generations = num_generations + 1;
}

//initialize the grid
void evolver::set_world_size(const int& width) {
    //cerr << "called " << "set_world_size" << "()" << endl;
    world = new grid(width);
}

void evolver::set_replay_time(const int& seconds) {
    m_replay_time = seconds * 1000;
} 

//which generations to include in the replay
void evolver::set_generations_to_save(const int* generations, int len) {
    //cerr << "called " << "set_generations_to_save" << "()" << endl;
    for (int i = 0; i < len; i++) {
        m_generations_to_save.push_back(generations[i]);
    }
    
}

void evolver::save_fibonaccis() {
    m_generations_to_save.push_back(0);
    int a, b, i;
    a = 1;
    for (i = 1; i < m_num_generations; i++) {
        m_generations_to_save.push_back(i);
        m_generations_to_save.push_back(m_num_generations - i);
        b = i;
        i = a;
        a = i + b;
    }
    m_generations_to_save.push_back(m_num_generations - 1);    
}

void evolver::save_squares() {
    m_generations_to_save.push_back(0);
    for (int i = 2; i < m_num_generations; i *= 2) {
        m_generations_to_save.push_back(i);
        m_generations_to_save.push_back(m_num_generations - i);
    }
    m_generations_to_save.push_back(m_num_generations - 1);
}

void evolver::set_brain_size(const int& neurons) {
    //cerr << "called " << "set_brain_size" << "()" << endl;
    m_brain_size = neurons;
}

void evolver::expand_brain_size(const int& neurons) {
    //cerr << "called " << "expand_brain_size" << "()" << endl;
    m_brain_size = neurons;
    creature curr;
    curr.link_world(world);
    curr.set_random_pos();
    curr.set_m_factor(m_mutation_rate);
    curr.set_hidden_layer_size(m_brain_size);
    curr.build_brain();
    for (int i = 0; i < m_num_creatures; i++) {
        m_creatures[i].expand_brain(curr);
        curr.scramble_brain();
    }
}

void evolver::set_mutation_rate(const float& m_factor) {
    m_mutation_rate = m_factor;
}

void evolver::set_boundary_rectangle(const coordinate& origin, 
                                        const int& width, const int& height) {
    std::vector<coordinate> coordinate_list;
    for (int i = 0; i < (width * 2); i++) {
        for (int j = 0; j < (height * 2); j++) {
            coordinate a = {(origin.x - width) + i, (origin.y - height) + j};
            coordinate_list.push_back(a);
        }
    }
    int x = origin.x;
    int y = origin.y;
    world->set_rewards(coordinate_list, world->index_from_pos(x,y));
}

//square
void evolver::set_boundary_square(const coordinate& origin, const int& radius) {
    //cerr << "called " << "set_boundary_square" << "()" << endl;
    set_boundary_rectangle(origin, radius, radius);
}

/********************************* GETTERS ***********************************/

float evolver::survival_rate() const {
    //cerr << "called " << "survival_rate" << "()" << endl;
    return m_survival_rate;
}

/***************************** RUN FUNCTIONS *********************************/

void evolver::add_creatures(const int& population_size) {
    //cerr << "called " << "add_creatures" << "()" << endl;
    for (int i = 0; i < population_size; i++) {
        creature curr;
        curr.link_world(world);
        curr.set_random_pos();
        curr.set_m_factor(m_mutation_rate);
        curr.set_hidden_layer_size(m_brain_size);
        curr.build_brain();
        m_creatures.push_back(curr);
    }
    m_num_creatures += population_size;
}

void evolver::run() {
    //cerr << "called " << "run" << "()" << endl;
    bool save_gen;
    population p;
    if (assert_neccesary_conditions())
        return;
            
    cout << "This is the reward boundary.\n" << world->show_grid_rewards();   
    for (int i = 0; i < m_num_generations; i++) { // simulation loop
        save_gen = false;
        p.clear();
        for (size_t j = 0; j < m_generations_to_save.size(); j++) {
            save_gen += (i == m_generations_to_save[j]);
        }
        for (int j = 0; j < m_time_steps; j++) { // run generation
            run_single_step();
            if (save_gen) {
                p.generation = i;
                p.size = m_num_creatures;
                p.replay.push_back(world->get_grid_state());
            }
        }
        world->clear_grid_state();
        reproduce(save_gen, p); // find survivors
        clean_up_casualties();
        if (save_gen) {
            p.survival = m_survival_rate;
            p.specimen = m_creatures[world->get_random(m_num_creatures)];
            print_summary(cout, i);
            m_saved_populations.push_back(p); 
        }
    }
}

void evolver::play_replay(ostream& output) {
    //cerr << "called " << "play_replay" << "()" << endl;
    output << "Hit enter to show replay, or type 'summary' or 'stop'.\n";
    string play;
    getline(cin, play);
    
    if (play == "summary") {
        output << "This is the reward boundary.\n";
        output << world->show_grid_rewards();
        output << m_summary;
        play_replay(output);
    }
    
    if (play == "stop" || play == "quit" || play == "q") {
        return;
    }
    for (size_t i = 0; i < m_saved_populations.size(); i++) {
        for (size_t j = 0; j < m_saved_populations[i].replay.size(); j++) {
            output << "GENERATION: " << m_saved_populations[i].generation 
            << "       population: " << m_saved_populations[i].size
            << "    survival rate: " << m_saved_populations[i].survival << "%"
            << "       brain size: " << m_saved_populations[i].specimen.brain_size()
            << "\n";
            
            output << m_saved_populations[i].replay[j];
            this_thread::sleep_for(chrono::milliseconds(m_replay_time/m_time_steps));
        }
    }
    play_replay(output);
}

void evolver::print_summary(ostream& output, int gen) {
    //cerr << "called " << "print_summary" << "()" << endl;
    m_summary += summary(gen);
    output << summary(gen);
}

string evolver::summary(const int& gen) {
    return "Generation " + to_string(gen) + " survival rate was: " 
    + to_string(static_cast<int>(m_survival_rate)) + "%\n";
}

void evolver::share_population_data(ostream& output, const int generation) {
    //cerr << "called " << "share_population_data" << "()" << endl;
    int found = 0; //found is false
    for (size_t i = 0; i < m_saved_populations.size(); i++) {
        if (generation == m_saved_populations[i].generation) {
            found = i + 1; // found is index of generation + 1
            break;
        }
    }
    if (not found)
        return;
    
    int num_genomes = m_saved_populations[--found].size;
    for (int j = 0; j < num_genomes; j++) {
        output << m_saved_populations[found].population_genomes[j];
    }
}

/*************************** PRIVATE FUNCTIONS ********************************/

void evolver::run_single_step() {
    //cerr << "called " << "run_single_step" << "()" << endl;
    for (int i = 0; i < m_num_creatures; i++) {
        m_creatures[i].think();
        m_creatures[i].go();
    }
}

void evolver::reproduce(const bool& save, population& p) {
    //cerr << "called " << "reproduce" << "()" << endl;
    for (int i = 0; i < m_num_creatures; i++){
        int x = m_creatures[i].pos_x();
        int y = m_creatures[i].pos_y();
        if (world->is_rewarded(x,y)) { //survived
            m_survivors.push_back(i); 
            m_creatures[i].mutate();
        }
        else { // tough go, kid :( 
            m_non_survivors.push_back(i); 
        }
        
        if (save) {
            p.population_genomes.push_back(m_creatures[i].genome());
        }
    }
    m_survival_rate = m_survivors.size() * 100 / m_creatures.size();
}

void evolver::clean_up_casualties() {
    //cerr << "called " << "clean_up_casualties" << "()" << endl;
    if (m_survivors.empty()){ //if creatures went extinct, start over
        for (int i = 0; i < m_num_creatures; i++) {
            m_creatures[i].scramble_brain();
            m_creatures[i].set_random_pos();
        }
    }
    else {
        for (size_t j = 0; j < m_non_survivors.size(); j++) { //copy a survivor
            int dead = m_non_survivors[j];
            int alive = m_survivors[j % m_survivors.size()];
            m_creatures[dead].mutate(m_creatures[alive]);
        }
    }
    
    m_survivors.clear();
    m_non_survivors.clear();
}

bool evolver::assert_neccesary_conditions() const {
    //cerr << "called " << "assert_neccesary_conditions" << "()" << endl;
    
    bool valid = (world != nullptr) && m_num_creatures && m_time_steps &&
            m_num_generations && !m_creatures.empty() && m_brain_size &&
            !m_generations_to_save.empty() && world->grid_ready();
    string error = "";
    
    if ((world == nullptr))
        error += "World is not set.\n";
    if (not m_num_creatures)
        error += "m_num_creatures is not set.\n";
    if (not m_time_steps)
        error += "m_time_steps is not set.\n";
    if (not m_num_generations)
        error += "m_num_generations is not set.\n";
    if (not m_num_creatures)
        error += "m_num_creatures is not set.\n";
    if (m_creatures.empty())
        error += "m_creatures is empty.\n";
    if (not m_brain_size) 
        error += "m_brain_size is not set.\n";
    if (m_generations_to_save.empty())
        error += "m_generations_to_save is empty.\n";
    if (not world->grid_ready())
        error += "world is not ready.\n";
        
    if (not valid) {
        cout << "Error: You are missing some neccesary start conditions\n";
        cout << error;
    }
    return !valid;
}