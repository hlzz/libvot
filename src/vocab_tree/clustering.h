#ifndef CLUSTERING_HEADER
#define CLUSTERING_HEADER

#include "global_params.h"

namespace tw
{
	double Kmeans(size_t num, int dim, int k, DTYPE **p, double *means, int *assignment);
}

#endif	//CLUSTERING_HEADER