#include <cstring>

#include "data_structures.h"

namespace tw
{
	UnionFind::UnionFind(size_t n)
	{
		father = new size_t[n];
		size = n;
		for(int i = 0; i < size; i++)
			father[i] = i;
	}

	UnionFind::~UnionFind()
	{
		delete [] father;
	}

	size_t UnionFind::Find(size_t x)
	{
		if(x != father[x])
			father[x] = Find(father[x]);	// path compression
		return father[x];
	}
	
	bool UnionFind::UnionSet(size_t x, size_t y)
	{
		x = Find(x);
		y = Find(y);
		if(x == y) return false;
		father[y] = x;
		return true;
	}


	int CompareIndexedFloat(const void *a, const void *b)
	{
		tw::IndexedFloat *i1, *i2;
		i1 = (tw::IndexedFloat *)a;
		i2 = (tw::IndexedFloat *)b;
		if(i1->value < i2->value)
			return 1;
		else 
			return -1;
	}
}	// end of namespace tw