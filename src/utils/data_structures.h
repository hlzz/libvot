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

}	// end of namespace tw


#endif	//DATA_STRUCTURES_HEADER