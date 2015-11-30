/*
Copyright (c) 2015, Tianwei Shen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of libvot nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include <cstring>

#include "data_structures.h"

namespace tw
{
	UnionFind::UnionFind(size_t n)
	{
		father = new size_t[n];
		size = n;
		for(size_t i = 0; i < size; i++)
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
