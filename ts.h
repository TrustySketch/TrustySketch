#ifndef TS_HEADER
#define TS_HEADER

#include "sketch.h"
#include <set>
using std::set;

struct TSBucket {
    int hi_cnt, fp;
    short lo_cnt;
    bool locked;
    TSBucket() {
        hi_cnt = 0;
        lo_cnt = 0;
        fp = 0;
        locked = false;
    }
};

class TSketch : public Sketch
{
public:
	int level;
    int num_bucket, row_size[30], err_bound[30];
    double R_w, R_l;

	TSBucket *bucket[30];

	TSketch(int size, int level_0, int threshold, double R_w = 2., double R_l = 2.);
	~TSketch();
	void init();
	void insert(int v);
	int query_freq(int v);
	int query_freq_low(int v);
	int query_freq_level(int v);
	int query_hash(int v);
    
	void status();
};

#endif