#include "hg.h"
#include "utils.h"
#include "murmur3.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <climits>
#include <cstdint>
#include <map>
using std::map;

HeavyGuardian::HeavyGuardian(int size, int num_g, int num_ct, double beta) :
size(size), num_g(num_g), num_ct(num_ct), beta(beta)
{
	sprintf(name, "HeavyGuardian");

	if (size <= 0)
	{
		panic("SIZE must be POSITIVE integers.");
	}

	cnt = new int[size * num_g];
	fp = new int[size * num_g];
	ext = new int[size * num_ct];
}

HeavyGuardian::~HeavyGuardian()
{
	if (cnt)
		delete [] cnt;
	if (fp)
		delete [] fp;
	if (ext)
		delete [] ext;
}

void
HeavyGuardian::init()
{
	memset(cnt, 0, size*num_g * sizeof(int));
	memset(fp, 0, size*num_g * sizeof(int));
	memset(ext, 0, size*num_ct * sizeof(int));
}

void
HeavyGuardian::insert(int v)
{
    int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), 123) % size;
    int base = pos * num_g, k, i;
    
    bool flag = false;
    for (i = 0, k = base; i < num_g; i++, k++)
    {
        int c = cnt[k];
        if (fp[k] == v)
        {
            cnt[k]++;
            flag = true;
            break;
        }
    }

    if (!flag)
    {
        int X, MIN = INT_MAX;
        for (i = 0, k = base; i < num_g; i++, k++)
        {
            int c=cnt[k];
            if (c<MIN) {MIN=c; X=k;}
        }

        if (!(rand() % int(pow(beta,cnt[X]))))
        {
            cnt[X]--;
            if (cnt[X]<=0)
            {
                fp[X] = v;
                cnt[X] = 0;
            }
            else
            {
                int p = pos % num_ct + pos * num_ct;
                if (ext[p]<16) ext[p]++;
            }
        }
    }
}

int
HeavyGuardian::query_freq(int v)
{
    int pos = MurmurHash3_x86_32((void*)&v, sizeof(int), 123) % size;
    int base = pos * num_g, k, i;

    for (i = 0, k = base; i < num_g; i++, k++)
    {
        int c = cnt[k];
        if (fp[k] == v)
        	return max(1, cnt[k]);
    }
    int p = pos % num_ct + pos * num_ct;
    return max(1, ext[p]);
}

void
HeavyGuardian::status()
{
	printf("bucket: %d   G: %d   CT: %d   Beta:%.3lf\n", size, num_g, num_ct, beta);
}
