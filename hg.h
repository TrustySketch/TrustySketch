#ifndef HG_HEADER
#define HG_HEADER

#include "sketch.h"
#include <set>
using std::set;

#define mp make_pair
#define ft first
#define sc second

class HeavyGuardian : public Sketch
{
private:
    int size, num_g, num_ct;
    double beta;

    int *cnt, *fp;
    int *ext;

public:
    HeavyGuardian(int size, int num_g, int num_ct, double beta);
    ~HeavyGuardian();
    void init();
    void insert(int v);
    int query_freq(int v);
    void status();  
};

#endif