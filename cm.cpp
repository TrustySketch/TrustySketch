#include "cm.h"
#include "utils.h"
#include "murmur3.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

CMSketch::CMSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
	sprintf(name, "CMSketch");

	if (size <= 0 || num_hash <= 0)
	{
		panic("SIZE & NUM_HASH must be POSITIVE integers.");
	}
	cnt = new int[size];
	
	row_size = size / num_hash;
}

CMSketch::~CMSketch()
{
	if (cnt)
		delete [] cnt;
}

void
CMSketch::init()
{
	memset(cnt, 0, size * sizeof(int));
}

void
CMSketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
CMSketch::insert(int v)
{
	int i = 0, base = 0;

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size + base;
		cnt[pos]++;
	}
}

int
CMSketch::query_freq(int v)
{
	int ans = INT_MAX;
	int i = 0, base = 0;

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size + base;
		ans = min(cnt[pos], ans);
	}

	return ans;
}
