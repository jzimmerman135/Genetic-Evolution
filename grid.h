#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <ctime>
#include <cstdlib>

#ifndef GRID_H
#define GRID_H

struct block {
    block();
    char symbol;
    bool empty;
    static const int channels = 3;
    std::vector<float> airwaves;
};

struct coordinate {
    int x;
    int y;
};

class grid {
public:
    grid(const int width);
    ~grid();
    
    void set_block(int& x, int& y, const char& symbol);
    void clear_block(int& x, int& y);
    
    // void send_broadcast(const int& x, const int& y, const int& channel,
    //                                                 const int& confidence);
    
    //getters
    int width() const;
    block get_block(int x, int y);
    bool is_rewarded(int x, int y);
    
    void set_rewards(std::vector<coordinate>& positions, const int& origin);
    
    void clear_grid_state();
    std::string get_grid_state() const;
    std::string show_grid_rewards() const;
    
    int get_random(const int m);
    
    bool grid_ready() const;
    int index_from_pos(int& x, int& y) const;
private:
    
    
    block *m_grid;
    int m_width;
    std::unordered_set<int> m_reward_positions;
    int m_reward_origin;
    
    //random generation
    int g_seed;
    inline int fastrand();
};

#endif