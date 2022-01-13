#include <iostream>
#include <vector>
#include <exception>
#include <unordered_set>
#include <ctime>
#include <cstdlib>
#include "grid.h"
using namespace std;

//initialize a block with empty character an and false reward value
block::block() {
    //cerr << "called " << "block" << "()" << endl;
    symbol = ' ';
    empty = true;
    for (int i = 0; i < channels; i++) {
        airwaves.push_back(0);
    }
}

//initialize grid based on width
grid::grid(const int width) {
    //cerr << "called " << "grid" << "()" << endl;
    m_width = width;
    m_grid = new block[width * width];
    g_seed = 12;
}

//destroy
grid::~grid() {
    //cerr << "called " << "~grid" << "()" << endl;
    delete [] m_grid;
}

//set a new symbol in grid block
void grid::set_block(int& x, int& y, const char& symbol) {
    //cerr << "called " << "set_block" << "()" << endl;
    int index = index_from_pos(x,y);
    m_grid[index].symbol = symbol;
    m_grid[index].empty = false;
}

void grid::clear_block(int& x, int& y) {
    //cerr << "called " << "clear_block" << "()" << endl;
    if (x == -1 && y == -1){
        return;
    }
    
    int i = index_from_pos(x, y);
    m_grid[i].symbol = ' ';
    m_grid[i].empty = true;
}

//return the block data at a coordinate
block grid::get_block(int x, int y) {
    //cerr << "called " << "get_block" << "()" << endl;
    return m_grid[index_from_pos(x, y)];
}

//creature is communicating, mark the area with a broadcast 
// void send_broadcast(const int x, const int y, const int& channel,
//                                               const int confidence) {
// }

//return width of grid as integer
int grid::width() const {
    //cerr << "called " << "width" << "()" << endl;
    return m_width;
}


bool grid::is_rewarded(int x, int y) {
    //cerr << "called " << "is_rewarded" << "()" << endl;
    return m_reward_positions.count(index_from_pos(x, y));
}

//set the positions that lead to reproduction
void grid::set_rewards(vector<coordinate>& positions, const int& origin) {
    //cerr << "called " << "set_rewards" << "()" << endl;
    m_reward_origin = origin;
    for (size_t i = 0; i < positions.size(); i++) {
        int index = index_from_pos(positions[i].x, positions[i].y);
        m_reward_positions.insert(index);
    }
}

//delete all symbols from grid
void grid::clear_grid_state() {
    //cerr << "called " << "clear_grid_state" << "()" << endl;
    for (int i = 0; i < (m_width * m_width); i++) {
        m_grid[i].symbol = ' ';
        for (int j = 0; j < m_grid[i].channels; j++) {
            m_grid[i].airwaves[j] = 0;
        }
    }
}

//return the state of the grid as one string
string grid::get_grid_state() const {
    //cerr << "called " << "get_grid_state" << "()" << endl;
    string state = "";
    for (int i = 0; i < m_width + 1; i++) {
        state += "**";
    }
    state += "*\n";
    
    for (int i = 0; i < (m_width * m_width); i++) {
        char item = m_grid[i].symbol;
        if (i % m_width == 0) {
            state += '*';
            state += ' ';
            state += item;
            state += ' ';
        }
        else if ((i + 1) % m_width == 0) {
            state += item;
            state += ' ';
            state += '*';
            state += '\n';
        }
        else {
            state += item;
            state += ' ';
        }
    }
    
    for (int i = 0; i < m_width + 1; i++) {
        state += "**";
    }
    state += "*\n";
    
    return state;
}

string grid::show_grid_rewards() const {
    //cerr << "called " << "show_grid_rewards" << "()" << endl;
    string state = "";
    for (int i = 0; i < m_width + 1; i++) {
        state += "**";
    }
    state += "*\n";
    
    for (int i = 0; i < (m_width * m_width); i++) {
        char item = ' ';
        if (m_reward_positions.count(i)) {
            item = '~';
        }
        if (i % m_width == 0) {
            state += '*';
            state += ' ';
            state += item;
            state += ' ';
        }
        else if ((i + 1) % m_width == 0) {
            state += item;
            state += ' ';
            state += '*';
            state += '\n';
        }
        else {
            state += item;
            state += ' ';
        }
    }
    
    for (int i = 0; i < m_width + 1; i++) {
        state += "**";
    }
    state += "*\n";
    
    return state;
}

int grid::get_random(const int m) {
    //cerr << "called " << "get_random" << "()" << endl;
    int res = fastrand() % m;
    return res;
}

bool grid::grid_ready() const {
    //cerr << "called " << "grid_ready" << "()" << endl;
    return m_width && !m_reward_positions.empty();
}

/********************** PRIVATE FUNCTIONS ************************************/
int grid::index_from_pos(int& x, int& y) const {
    //cerr << "called " << "index_from_pos" << "()" << endl;
    x *= (x >= 0);
    y *= (y >= 0);
    x -= (x >= m_width) * (x - m_width + 1);
    y -= (y >= m_width) * (y - m_width + 1);
    
    return (x) + (y * m_width);
}

inline int grid::fastrand() {
  g_seed = (214013*g_seed+2531011);
  return (g_seed>>16)&0x7FFF;
};