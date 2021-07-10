#ifndef CUS_HEADER
#define CUS_HEADER

#include "sketch.h"

class CUSketch : public Sketch
{
private:
	int size, num_hash, row_size;

	int *cnt;

public:
	CUSketch(int size, int num_hash);
	~CUSketch();
	void init();
	void insert(int v);
	int query_freq(int v);
	void status();
};

#endif