#include "ts.h"
#include "utils.h"
#include "murmur3.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <climits>
#include <cstdint>
#include <map>
using std::map;

unsigned LOW_MASK = 0xffffffff;

TSketch::TSketch(int size, int level_0, int threshold, double R_w, double R_l) :
level(level_0), R_w(R_w), R_l(R_l)
{
	sprintf(name, "TrustySketch");

	if (size <= 0)
	{
		panic("ROW_SIZE must be POSITIVE integers.");
	}

    int now_size = size, now_bound = threshold;
    num_bucket = 0;
    for (int i = 0; i < level; ++i) {
        if (now_size < 1)
        {
            level = i;
            printf("level has been limited to %d\n", level);
            break;
        }
        bucket[i] = new TSBucket[now_size];
        num_bucket += now_size;
        row_size[i] = now_size;
        // if (now_bound < 1)  now_bound = 1;
        err_bound[i] = min(now_bound, (int)(LOW_MASK>>1));

        now_size = (int)ceil((double)now_size / R_w);
        now_bound = (int)floor((double)now_bound / R_l);
    }
    // err_bound[level-1] = 1e9;
}

TSketch::~TSketch()
{
	for (int i = 0; i < level; ++i)
        if (bucket[i])
		    delete [] bucket[i];
}

void
TSketch::init()
{
	for (int i = 0; i < level; ++i)
	{
        memset(bucket[i], 0, row_size[i] * sizeof(TSBucket));
	}
}

void
TSketch::insert(int v)
{
    int sv = (v & LOW_MASK);
    for (register int i = 0; i < level; ++i) {
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size[i];
        TSBucket &bkt = bucket[i][pos];
        bool matched = bkt.fp == sv;
        if (bkt.locked && !matched)
            continue;

        int est = (matched? (bkt.hi_cnt - bkt.lo_cnt) : bkt.lo_cnt);
        // est = (est + bkt.hi_cnt) >> 1;

        // high part - cm sketch
        if (!matched)
        {
            if (bkt.hi_cnt - (bkt.lo_cnt<<1) <= 0)
                bkt.fp = sv; // matched = true
            else
                bkt.lo_cnt++;
        }
        bkt.hi_cnt++;

        // int err = (bkt.hi_cnt - bkt.lo_cnt) >> 1;
        if (bkt.lo_cnt >= err_bound[i])
        {
            bkt.locked = true;
        }
        break;
    }
}

int
TSketch::query_freq(int v)
{
    int sv = (v & LOW_MASK);
    int sum = 0;
    for (register int i = 0; i < level; ++i) {
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size[i];
        TSBucket &bkt = bucket[i][pos];
        bool matched = bkt.fp == sv;

        int est = (matched? (bkt.hi_cnt - bkt.lo_cnt) : bkt.lo_cnt);

        if (bkt.locked && !matched)
        {
            sum += est;
            continue;
        }

        // hit
        return sum + est;
    }
    return sum;
}

int
TSketch::query_freq_low(int v)
{
    int sv = (v & LOW_MASK);
    for (int i = 0; i < level; ++i) {
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size[i];
        TSBucket &bkt = bucket[i][pos];

        if (bkt.fp != sv && bkt.locked)
        {
            continue;
        }

        if (bkt.fp != sv)
        {
            return 0;
        }

        // hit
        return bkt.lo_cnt;
	}
	return 0;
}

int
TSketch::query_freq_level(int v)
{
    int sv = (v & LOW_MASK);
    for (int i = 0; i < level; ++i) {
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size[i];
        TSBucket &bkt = bucket[i][pos];

        if (bkt.fp == sv)
        {
            return i;
        }
        if (!bkt.locked)
        {
            return -1;
        }
	}
	return level;
}

int
TSketch::query_hash(int v)
{
    int sv = (v & LOW_MASK);
    for (int i = 0; i < level; ++i) {
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size[i];
        TSBucket &bkt = bucket[i][pos];

        if (bkt.fp == sv || !bkt.locked)
        {
            return i+1;
        }
	}
	return level;
}

void
TSketch::status()
{
	printf("level: %d   bucket: %d(lvl.1 %d)   threshold(lvl.1): %d   R_w: %.2lf   R_l: %.2lf\n", level, num_bucket, row_size[0], err_bound[0], R_w, R_l);
}