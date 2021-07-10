#include "cu.h"
#include "utils.h"
#include "murmur3.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

CUSketch::CUSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
	sprintf(name, "CUSketch");

	if (size <= 0 || num_hash <= 0)
	{
		panic("SIZE & NUM_HASH must be POSITIVE integers.");
	}
	cnt = new int[size];

	row_size = size / num_hash;
}

CUSketch::~CUSketch()
{
	if (cnt)
		delete [] cnt;
}

void
CUSketch::init()
{
	memset(cnt, 0, size * sizeof(int));
}

void
CUSketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
CUSketch::insert(int v)
{
	int minp = INT_MAX;
	int i = 0, base = 0;
	int sav_pos[20];

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), i) % row_size + base;
		sav_pos[i] = pos;
		minp = min(cnt[pos], minp);
	}

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = sav_pos[i];
		if (cnt[pos] == minp)
			cnt[pos]++;
	}	
}

int
CUSketch::query_freq(int v)
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
