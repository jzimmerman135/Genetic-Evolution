// Example program
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class randnum {
    public:
    randnum() { x = 1; y=362436069; z=521288629; g_seed = 12;}
    int get_xor_random(int m) { return xorshf96() % m; }
    int get_fast_random(int m) { return fastrand() % m; }
    private:
    int g_seed;
    unsigned long x, y, z; 
    unsigned long xorshf96(void) {          //period 2^96-1
    unsigned long t;
        x ^= x << 16;
        x ^= x >> 5;
        x ^= x << 1;
    
       t = x;
       x = y;
       y = z;
       z = t ^ x ^ y;
    
      return z;
    }
    
    inline int fastrand() { 
        g_seed = (214013*g_seed+2531011); 
        return (g_seed>>16)&0x7FFF; 
    }
};

struct neuron {
    std::vector<float> input_weights;
    int index;
    float bias;
    float firing;
};

int main()
{
  randnum r;
  int range = 60;
  // int arr[range];
  // for (int i = 0; i < range ; i++) {
  //     arr[i] = 8;
  // }
  // for (int i = 0; i < 200000000; i++) {
  //       arr[r.get_fast_random(range)] += 1;
  // }
  // for (int i = 0; i < range; i++) {
  //    std::cout << i + 1 << ":  " << arr[i] << " || ";
  //    if (!((i + 1) % 10)) {
  //        std::cout << "\n";
  //    }
  //    arr[i] = 0;
  // }

  for (int i = 0; i < 10; i++) {
      cout << r.get_fast_random(range) << endl;;
  }

  return 0;
}

