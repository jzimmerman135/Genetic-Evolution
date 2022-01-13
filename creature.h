#include <iostream>
#include <string>
#include "grid.h"

#ifndef CREATURE_H
#define CREATURE_H



class creature {
public:
    struct neuron;
    //constructor for replicating a creature
    creature();
    //creature(grid* grid, std::string genome);
    void think();
    void go();
    
    //getters
    std::string genome() const;
    float m_factor() const;
    int pos_x() const;
    int pos_y() const;
    char symbol() const;
    int direction() const;
    neuron hidden(const int& index) const;
    neuron outputs(const int& index) const;
    int brain_size() const;
    bool is_linked() const;
    
    //setters
    void link_world(grid *&grid);
    void set_random_pos();
    void move_to_pos(const int& x, const int& y);
    void clear_pos();
    
    void set_hidden_layer_size(const int& neurons);
    void set_m_factor(const float& m_factor);
    void scramble_brain();
    void build_brain();
    void build_brain_from_genome(const std::string genome);
    void expand_brain(const creature& c);
    void copy_creature(const creature& copy);
    void mutate();
    void mutate(const creature& s);
    
    struct neuron {
        std::vector<float> input_weights;
        int index;
        float bias;
        float firing;
    };
private:  
    //senses
    /* 0 */ float blocked();
    /* 1 */ float wall();
    // /* 2 */ float personal_bubble();
    // /* 3 */ float density_in_front();
    /* 4 */ float x_pos();
    /* 5 */ float x_pos_inv();
    /* 6 */ float y_pos();
    /* 7 */ float y_pos_inv();
    /* 8 */ inline float facing_up();
    /* 9 */ inline float facing_right();
    /* 10 */ inline float facing_down();
    /* 11 */ inline float facing_left();
    // /* 8 - 10 */ float listen(const int channel);
    /* 12 */float clock();
    // 
    //actions
    /* 0 */ void turn_clockwise();
    /* 1 */ void turn_ctr_clockwise();
    /* 2 */ void turn_180();
    /* 3 */ void move_forward();
    // /* 4 - 6 */ void speak(const int channel,const float confidence);
    
    //private brain functions
    void calc_inputs();
    void calc_hidden();
    int calc_outputs();
    
    inline float mutation() const;
    inline void bounded_add(float& a, const float& b, const float& c, const float& d);

    grid* m_grid;
    int m_pos_x;
    int m_pos_y;
    char m_symbol;
    int m_direction;
    int m_clock;
    float m_mutation_factor;
    
    int NUM_HIDDEN;
    
    static const int NUM_INPUTS = 11;
    static const int NUM_OUTPUTS = 4;
    
    typedef void (creature::*Action)();
    static const Action actions[NUM_OUTPUTS];
    static const char creature_symbols[4];
    
    std::vector<neuron> m_hidden;
    std::vector<neuron> m_outputs;
    
    float m_inputs[NUM_INPUTS];
    float HUNDRED;
};



#endif