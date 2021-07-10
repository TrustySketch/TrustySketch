#include "ss.h"
#include "utils.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <climits>
#include <cstdint>
#include <map>
using std::map;

SpaceSaving::SpaceSaving(int size) :
size(size)
{
	sprintf(name, "SpaceSaving");

	if (size <= 0)
	{
		panic("SIZE must be POSITIVE integers.");
	}

	bkt = new Bucket[size];
	ele = new Element[size];

	ele_idx.reserve(2 * size);
}

SpaceSaving::~SpaceSaving()
{
	if (bkt)
		delete [] bkt;
	if (ele)
		delete [] ele;
}

void
SpaceSaving::init()
{
	for (int i = 0; i < size; ++i)
	{
		bkt[i].next = &bkt[(i+1) % size];
		bkt[i].prev = &bkt[(i-1+size) % size];
	}
	bkt[1].prev = NULL;
	bkt[size-1].next = NULL;
	free_head = &bkt[1];

	bkt[0].value = 0;
	bkt[0].prev = bkt[0].next = NULL;
	bkt[0].son = &ele[0];
	tail = &bkt[0];

	for (int i = 0; i < size; ++i)
	{
		ele[i].next = &ele[(i+1) % size];
		ele[i].prev = &ele[(i-1+size) % size];
		ele[i].parent = tail;
	}
}

void
SpaceSaving::insert(int v)
{
	auto it = ele_idx.find(v);
	Element *now;

	if (it == ele_idx.end())
	{
		now = tail->son;
		ele_idx.erase(now->fp);
		ele_idx[v] = now;
		now->fp = v;
	}
	else // footprint found.
	{
		now = it->second;
	}

	Bucket *prt = now->parent;
	
	// only one element
	if (now->next == now)
	{
		if (prt->next == NULL || prt->next->value > prt->value+1)
		{
			prt->value += 1;
		}
		else
		{
			if (prt->next->value != prt->value+1)
			{
				printf("error!\n");
			}

			Element* old_head = prt->next->son;
			Element* old_tail = old_head->prev;
			old_tail->next = now;
			old_head->prev = now;
			now->next = old_head;
			now->prev = old_tail;
			now->parent = prt->next;
			prt->next->son = now;

			// modify bucket chain
			prt->next->prev = prt->prev;
			if (prt->prev != NULL)
				prt->prev->next = prt->next;

			// add to free bucket list
			if (tail == prt)
				tail = prt->next;
			prt->next = free_head;
			free_head = prt;
		}
	}
	else
	{
		if (prt->son == now)
			prt->son = now->next;
		now->next->prev = now->prev;
		now->prev->next = now->next;

		if (prt->next == NULL || prt->next->value > prt->value+1)
		{
			if (free_head == NULL)
			{
				printf("No more free bucket!\n");
			}

			// new bucket
			// printf("new bucket!\n");
			Bucket *new_bkt = free_head;
			free_head = free_head->next;

			new_bkt->prev = prt;
			new_bkt->next = prt->next;
			if (prt->next != NULL)
				prt->next->prev = new_bkt;
			prt->next = new_bkt;
			new_bkt->value = prt->value + 1;
			new_bkt->son = now;
			now->next = now->prev = now;
			now->parent = new_bkt;
		}
		else
		{
			if (prt->next->value != prt->value+1)
			{
				printf("Order error!\n");
				exit(0);
			}

			Element* old_head = prt->next->son;
			Element* old_tail = old_head->prev;
			old_tail->next = now;
			old_head->prev = now;
			now->next = old_head;
			now->prev = old_tail;
			now->parent = prt->next;
			prt->next->son = now;
		}		
	}
}

int
SpaceSaving::query_freq(int v)
{
	auto it = ele_idx.find(v);

	if (it == ele_idx.end()) // mouse flow
	{
		return tail->value;
	}
	else
		return it->second->parent->value;
}

int
SpaceSaving::query_freq_low(int v)
{
	auto it = ele_idx.find(v);

	if (it == ele_idx.end()) // mouse flow
	{
		return 0;
	}
	else
		return it->second->parent->value - tail->value;
}


void
SpaceSaving::status()
{
	Bucket *now = tail;
	printf("bucket: %d\n", size);
}
