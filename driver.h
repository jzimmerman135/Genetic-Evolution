#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include "grid.h"
#include "creature.h"

#ifndef DRIVER_H
#define DRIVER_H

struct population {
    int size; 
    int generation;
    int survival;
    creature specimen;
    std::vector<std::string> population_genomes;
    std::vector<std::string> replay;
    void clear();
};

class evolver {
public:
    evolver();
    ~evolver();
    // setters
    void set_time_steps(const int& steps);
    void set_generations(const int& num_generations);
    void set_world_size(const int& width);
    void set_brain_size(const int& neurons);
    void expand_brain_size(const int& neurons);
    void set_mutation_rate(const float& m_factor);
    void set_generations_to_save(const int* generations, int len);
    void set_replay_time(const int& seconds);
    void save_fibonaccis();
    void save_squares();
    void set_boundary_square(const coordinate& origin, const int& radius);
    void set_boundary_rectangle(const coordinate& origin, const int& width,
                                                           const int& height);
    // getters
    float survival_rate() const;
    
    
    void add_creatures(const int& population_size);
    void run();
    //run with pre-existing species
    // void run(population p);
    
    void play_replay(std::ostream& output);
    void print_summary(std::ostream& output, int gen);
    std::string summary(const int& gen);
    void share_population_data(std::ostream& output, const int generation);

private:
    bool assert_neccesary_conditions() const;
    
    void run_single_step();
    
    // void custom_default_species(population p);
    void reproduce(const bool& save, population& p);
    void clean_up_casualties();
    
    int m_num_creatures;
    int m_num_generations;
    int m_time_steps;
    int m_brain_size;
    float m_mutation_rate;
    int m_replay_time;
    float m_survival_rate;
    
    
    grid *world;    
    std::vector<creature> m_creatures;
    std::vector<int> m_survivors;
    std::vector<int> m_non_survivors;
    
    std::string m_summary;
    std::vector<int> m_generations_to_save;
    std::vector<population> m_saved_populations;    
};

#endif