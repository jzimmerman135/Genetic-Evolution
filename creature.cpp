#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "creature.h"
#include "grid.h"
using namespace std;

creature::creature() {
    //cerr << "called " << "creature" << "()" << endl;
    m_grid = nullptr;
    m_pos_x = -1;
    m_pos_y = -1;
    m_mutation_factor = 0.05;    
    m_direction = 0;
    m_symbol = creature_symbols[m_direction];
    m_clock = 0;
    NUM_HIDDEN = 3;
    HUNDRED = 100;
}

const char creature::creature_symbols[] = {'^','>','v','<'};

/******************************** RUNNERS ************************************/

void creature::think() {
    //cerr << "called " << "think" << "()" << endl;
    calc_inputs();
    calc_hidden();
    return;
}

void creature::go() {
    //cerr << "called " << "go" << "()" << endl;
    // find largest output
    // call action
    int i = calc_outputs();
    (this->*actions[i])();
    return;
}

/********************************* GETTERS ***********************************/

string creature::genome() const {
    //cerr << "called " << "genome" << "()" << endl;
    string genome = "******";
    for (int i = 0; i < NUM_HIDDEN; i++) {
        for (int j = 0; j < NUM_INPUTS + i; j++) {
            genome += "HIDDEN " + to_string(i) + " WEIGHT" + to_string(j) + ": ";
            genome += to_string(m_hidden[i].input_weights[j]) + '\n'; 
        }
        genome += "HIDDEN " + to_string(i) + " BIAS: ";
        genome += to_string(m_hidden[i].bias) + '\n'; 
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN; j++) {
            genome += "OUTPUT " + to_string(i) + " WEIGHT" + to_string(j) + ": ";
            genome += to_string(m_outputs[i].input_weights[j]) + '\n'; 
        }
        genome += "OUTPUT " + to_string(i) + " BIAS: ";
        genome += to_string(m_outputs[i].bias) + '\n'; 
    }
    genome += "MUTATION_FACTOR: ";
    genome += to_string(m_mutation_factor) + '\n'; 
    genome += "******\n";
    return genome;
}

float creature::m_factor() const {
    //cerr << "called " << "m_factor" << "()" << endl;
    return m_mutation_factor;
}

int creature::pos_x() const {
    //cerr << "called " << "pos_x" << "()" << endl;
    return m_pos_x;
}

int creature::pos_y() const {
    //cerr << "called " << "pos_y" << "()" << endl;
    return m_pos_y;
}

char creature::symbol() const {
    //cerr << "called " << "symbol" << "()" << endl;
    return m_symbol;
}

int creature::direction() const {
    //cerr << "called " << "direction" << "()" << endl;
    return m_direction;
}

creature::neuron creature::hidden(const int& index) const {
    //cerr << "called " << "hidden" << "()" << endl;
    return m_hidden[index];
}

creature::neuron creature::outputs(const int& index) const {
    //cerr << "called " << "outputs" << "()" << endl;
    return m_outputs[index];
}

int creature::brain_size() const {
    //cerr << "called " << "brain_size" << "()" << endl;
    return NUM_HIDDEN;
}

bool creature::is_linked() const {
    //cerr << "called " << "is_linked" << "()" << endl;
    return m_grid != nullptr;
}

/********************************* SETTERS ***********************************/

void creature::link_world(grid *&grid) {
    //cerr << "called " << "link_world" << "()" << endl;
    m_grid = grid;
    m_direction = m_grid->get_random(4);
    m_symbol = creature_symbols[m_direction];
}

//sets the member position variables
//sets the position to a random location if pos is occupied
void creature::set_random_pos() {
    //cerr << "called " << "set_random_pos" << "()" << endl;
    coordinate pos;
    int width = m_grid->width();
    int index = m_grid->get_random(width * width);
    pos.x = index % width;
    pos.y = index / width;
    while (not m_grid->get_block(pos.x, pos.y).empty) {
        index = m_grid->get_random(width * width);
        pos.x = index % width;
        pos.y = index / width;
    }
    
    clear_pos();
    m_pos_x = pos.x;
    m_pos_y = pos.y;
    m_direction = m_grid->get_random(4);
    m_symbol = creature_symbols[m_direction];
    m_grid->set_block(m_pos_x, m_pos_y, m_symbol);
    return;
}

void creature::set_hidden_layer_size(const int& neurons) {
    NUM_HIDDEN = neurons;
}

void creature::set_m_factor(const float& m_factor) {
    m_mutation_factor = m_factor;
}

void creature::move_to_pos(const int& x, const int& y) {
    //cerr << "called " << "move_to_pos" << "()" << endl;
    if (not m_grid->get_block(x,y).empty) {
        return;
    }
    
    clear_pos();
    m_pos_x = x; 
    m_pos_y = y;
    m_grid->set_block(m_pos_x, m_pos_y, m_symbol);
}

void creature::clear_pos() {
    //cerr << "called " << "clear_pos" << "()" << endl;
    m_grid->clear_block(m_pos_x, m_pos_y);
}

void creature::mutate() {
    //cerr << "called " << "mutate" << "()" << endl;
    
    //mutate brain stuff
    for (int i = 0; i < NUM_HIDDEN; i++) {
        for (int j = 0; j < NUM_INPUTS + i; j++) {
            //bounded_add(m_hidden[i].input_weights[j], mutation(), -1, 1);
            m_hidden[i].input_weights[j] += mutation();
        }
        //bounded_add(m_hidden[i].bias, mutation(), -NUM_INPUTS - i, NUM_INPUTS + i);
        m_hidden[i].bias += mutation(); 
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN; j++) {
            //bounded_add(m_outputs[i].input_weights[j], mutation(), -1, 1);
            m_outputs[i].input_weights[j] += mutation();
        }
        //bounded_add(m_outputs[i].bias, mutation(), -NUM_HIDDEN, NUM_HIDDEN);
        m_outputs[i].bias += mutation();
    }
    m_mutation_factor += mutation() / (m_mutation_factor * 50); // +,-,or= m_factor / m_factor;
    set_random_pos();
    return;
}

void creature::mutate(const creature& s) {
    //cerr << "called " << "mutate" << "( creature& src )" << endl;
    if (NUM_HIDDEN < s.brain_size())
        expand_brain(s);
    
    for (int i = 0; i < NUM_HIDDEN; i++) {
        for (int j = 0; j < NUM_INPUTS + i; j++) {
            m_hidden[i].input_weights[j] = s.hidden(i).input_weights[j];
            m_hidden[i].input_weights[j] += mutation();
        }
        m_hidden[i].bias = s.hidden(i).bias;
        m_hidden[i].bias += mutation();
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN; j++) {
            m_outputs[i].input_weights[j] = s.outputs(i).input_weights[j];
            m_outputs[i].input_weights[j] += mutation();
        }
        m_outputs[i].bias = s.outputs(i).bias;
        m_outputs[i].bias += mutation();
    }
    m_mutation_factor = s.m_factor();
    m_mutation_factor += mutation() / (m_mutation_factor * 50); 
    set_random_pos();
    return;
}

void creature::scramble_brain() {
    //cerr << "called " << "scramble_brain" << "()" << endl;
    for (int i = 0; i < NUM_HIDDEN; i++) {
        for (int j = 0; j < NUM_INPUTS + i; j++) {
            m_hidden[i].input_weights[j] = (m_grid->get_random(201) - 100) / HUNDRED;
        }
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN; j++) {
            m_outputs[i].input_weights[j] = (m_grid->get_random(201) - 100) / HUNDRED;
        }
    }
}

void creature::build_brain() {
    //cerr << "called " << "build_brain" << "()" << endl;    
    for (int i = 0; i < NUM_HIDDEN; i++) {
        neuron curr;
        curr.index = i;
        curr.bias = 1;
        curr.firing = 0;
        for (int j = 0; j < (NUM_INPUTS + i); j++) {
            curr.input_weights.push_back((m_grid->get_random(201) - 100) / HUNDRED);
        }
        
        m_hidden.push_back(curr);
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        neuron curr;
        curr.index = i;
        curr.bias = 1;
        curr.firing = 0;
        for (int j = 0; j < NUM_HIDDEN; j++) {
            curr.input_weights.push_back((m_grid->get_random(201) - 100) / HUNDRED);
        }
        
        m_outputs.push_back(curr);
    }
}

void creature::build_brain_from_genome(const string genome) {
    //cerr << "called " << "build_brain" << "()" << endl;
    (void) genome;
    return;
}

/********************************* PRIVATE ***********************************/

void creature::expand_brain(const creature& s) {
    if (s.brain_size() <= NUM_HIDDEN)
        return;
    
    for (int i = NUM_HIDDEN; i < s.brain_size(); i++) {
        m_hidden.push_back(s.hidden(i));
        for (int j = 0; j < NUM_OUTPUTS; j++) {
            m_outputs[i].input_weights.push_back((m_grid->get_random(201) - 100) / HUNDRED);
        }
    }
    NUM_HIDDEN = s.brain_size();
}

const creature::Action creature::actions[] = {
    &creature::move_forward,
    &creature::turn_clockwise,
    &creature::turn_ctr_clockwise,
    &creature::turn_180
};

void creature::calc_inputs() {
    //cerr << "called " << "calc_inputs" << "()" << endl;
    m_inputs[0] = blocked();
    m_inputs[1] = wall();
    m_inputs[2] = x_pos();
    m_inputs[3] = x_pos_inv();
    m_inputs[4] = y_pos();
    m_inputs[5] = y_pos_inv();
    m_inputs[6] = facing_up();
    m_inputs[7] = facing_right();
    m_inputs[8] = facing_down();
    m_inputs[9] = facing_left();
    m_inputs[10] = clock();
}

void creature::calc_hidden() {
    //cerr << "called " << "calc_hidden" << "()" << endl;
    int total;
    for (int i = 0; i < NUM_HIDDEN; i++) {
        total = 0;
        for (int j = 0; j < NUM_INPUTS; j++) {
            total += m_hidden[i].input_weights[j] * m_inputs[j];
        }
        for (int j = NUM_INPUTS; j < NUM_INPUTS + i; j++) {
            total += m_hidden[i].input_weights[j] * m_hidden[j].firing;
        }
        m_hidden[i].firing = tanh(m_hidden[i].bias + total);
    }
    return;
}

int creature::calc_outputs() {
    //cerr << "called " << "calc_outputs" << "()" << endl;
    float max = 0;
    int total, res = 0;
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        total = 0;
        for (int j = 0; j < NUM_HIDDEN; j++) {
            total += m_outputs[i].input_weights[j] * m_hidden[j].firing;
        }
        m_outputs[i].firing = m_outputs[i].bias + total;
        if (m_outputs[i].firing > max) {
            max = m_outputs[i].firing;
            res = i;
        }
    }
    return res;
}

inline float creature::mutation() const{
    return (m_grid->get_random(3) - 1) * m_mutation_factor;
}

// c < a+b < d
inline void creature::bounded_add(float& a, const float& b, const float& c, const float& d) {
    a += b * ((a + b) < d && (a + b) > c); 
}

/********************************* SENSES ***********************************/

float creature::blocked() {
    //cerr << "called " << "blocked" << "()" << endl;
    if (m_direction == 0)
        return 1 - m_grid->get_block(m_pos_x, m_pos_y - 1).empty;
    else if (m_direction == 1)
        return 1 - m_grid->get_block(m_pos_x + 1, m_pos_y).empty;
    else if (m_direction == 2)
        return 1 - m_grid->get_block(m_pos_x, m_pos_y + 1).empty;
    else if (m_direction == 3)
        return 1 - m_grid->get_block(m_pos_x - 1, m_pos_y).empty;

    return 0;
}

float creature::wall() {
    //cerr << "called " << "wall" << "()" << endl;    
    if (m_direction == 0)
        return 1 - (m_pos_y - 1 >= 0);
    else if (m_direction == 1)
        return 1 - (m_pos_x + 1 < m_grid->width());
    else if (m_direction == 2)
        return 1 - (m_pos_x - 1 >= 0);
    else if (m_direction == 3)
        return 1 - (m_pos_y + 1 < m_grid->width());
    
    return 0;
}

float creature::x_pos() {
    //cerr << "called " << "x_pos" << "()" << endl;
    return static_cast<float>(m_pos_x) / static_cast<float>(m_grid->width());
}

float creature::x_pos_inv() {
    //cerr << "called " << "x_pos_inv" << "()" << endl;
    return 1 - x_pos();
}

float creature::y_pos() {
    //cerr << "called " << "y_pos" << "()" << endl;
    return static_cast<float>(m_pos_y) / static_cast<float>(m_grid->width());
}

float creature::y_pos_inv() {
    //cerr << "called " << "y_pos_inv" << "()" << endl;
    return 1 - y_pos();
}

inline float creature::facing_up() {
    //cerr << "called " << "facing_up" << "()" << endl;
    return (m_direction == 0);
}
inline float creature::facing_right() {
    //cerr << "called " << "facing_right" << "()" << endl;
    return (m_direction == 1);
}
inline float creature::facing_down() {
    //cerr << "called " << "facing_down" << "()" << endl;
    return (m_direction == 2);
}
inline float creature::facing_left() {
    //cerr << "called " << "facing_left" << "()" << endl;
    return (m_direction == 3);
}

float creature::clock() {
    //cerr << "called " << "clock" << "()" << endl;
    ++m_clock %= 2;
    return m_clock;
}

/********************************* ACTIONS ***********************************/

void creature::move_forward() {
    //cerr << "called " << "move_forward" << "()" << endl;
    if (m_direction == 0)
        move_to_pos(m_pos_x, m_pos_y - 1);
    else if (m_direction == 1)
        move_to_pos(m_pos_x + 1, m_pos_y);
    else if (m_direction == 2)
        move_to_pos(m_pos_x, m_pos_y + 1);
    else if (m_direction == 3)
        move_to_pos(m_pos_x - 1, m_pos_y);
}

void creature::turn_clockwise() {
    //cerr << "called " << "turn_clockwise" << "()" << endl;
    ++m_direction %= 4;
    m_symbol = creature_symbols[m_direction];
}

void creature::turn_ctr_clockwise() {
    //cerr << "called " << "turn_ctr_clockwise" << "()" << endl;
    m_direction += 3;
    m_direction %= 4;
    m_symbol = creature_symbols[m_direction];
}

void creature::turn_180() {
    //cerr << "called " << "turn_180" << "()" << endl;
    m_direction += 2;
    m_direction %= 4;
    m_symbol = creature_symbols[m_direction];
}