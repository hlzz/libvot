#ifndef DATA_STRUCTURES_HEADER
#define DATA_STRUCTURES_HEADER

namespace tw 
{
	class UnionFind 
	{
	public:
		UnionFind(size_t n);
		~UnionFind();
		size_t Find(size_t x);
		bool UnionSet(size_t x, size_t y);

		size_t *father;
		size_t size;
	};

	struct IndexedFloat
	{
		float value;
		size_t index;	
	};

	int CompareIndexedFloat(const void *a, const void *b); //!< compare function for tw::IndexedFloat

}	// end of namespace tw


#endif	//DATA_STRUCTURES_HEADER