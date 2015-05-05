#include <iostream>
#include <limits>
#include <cassert>
#include <cmath>

#include "clustering.h"
#include "global_params.h"

namespace tw
{
	/** Random sample k indexes for [1...n] without replacement */
	void GetIntialCenters(size_t n, int k, size_t *initial_idx)
	{
		for(int i = 0; i < k; i++)
		{
			size_t tmp = rand() % n;
			int j;
			for(j = 0; j < i; j++)
			{
				if(tmp == initial_idx[j])
				{
					i--; j = -1;
					break;
				}
			}
			if(j != -1)	initial_idx[i] = tmp;
		}
	}

	inline void CopyDes2Double(double *dst, DTYPE *src, int length)
	{
		for(int i = 0; i < length; i++)
			dst[i] = (double) src[i];
	}

	inline double DisDes2Double(double *dst, DTYPE *src, int length)
	{
		double dis = 0.0;	
		for(int i = 0; i < length; i++)
		{
			double d = dst[i] - (double)src[i];
			dis += d * d;
		}
		dis = sqrt(dis);
		return dis;
	}

	size_t ComputeAssignment(size_t num, int dim, int k, DTYPE **p, double *means, int *assignment, double &error)
	{			
		size_t change_num = 0;
		for(int i = 0; i < num; i++)
		{
			double min_dis = std::numeric_limits<double>::max();
			int min_idx = -1;
			for(int j = 0; j < k; j++)
			{
				double dis_tmp = DisDes2Double(means + j * dim, p[i], dim);

				if(min_dis > dis_tmp)
				{
					min_dis = dis_tmp;
					min_idx = j;
				}
			}
			if(assignment[i] != min_idx)
			{
				assignment[i] = min_idx;
				change_num++;
			}
		}
		return change_num;
	}

	void ComputeMeans(size_t num, int dim, int k, DTYPE **p, int *assignment_curr, double *means_curr)
	{
		size_t *counts = new size_t [k];
		for(int i = 0; i < k; i++)
		{
			counts[i] = 0;
			for(int j = 0; j < dim; j++)
				means_curr[i * dim + j] = 0;
		}

		// accumulation
		for(int i = 0; i < num; i++)
		{
			int lable = assignment_curr[i];
			counts[lable]++;
			for(int j = 0; j < dim; j++)
				means_curr[lable * dim + j] += p[i][j];
		}
		//for(int i = 0; i < k; i++) {showi(counts[i]); }

		// normalization
		for(int i = 0; i < k; i++)
		{
			if(counts[i] == 0) continue;
			for(int j = 0; j < dim; j++)
			{
				means_curr[i * dim + j] /= counts[i];
			}
		}

		delete [] counts;
	}

	double Kmeans(size_t num, int dim, int k, DTYPE **p, double *means, int *assignment)
	{
		if(num < k)
		{
			std::cerr << "[Error] Number of keys is less than the number of clusters, Exit...\n"; 
			return -1;
		}
		double min_dis = std::numeric_limits<double>::max();
		double *means_curr;
		size_t *initial_idx;
		int *assignment_curr;
		double change_pct_threshold = 0.005;	// VocabTree2 parameter
		int total_iter = 5;

		means_curr = new double [k * dim];
		initial_idx = new size_t [k];
		assignment_curr = new int [num];

		if(means_curr == NULL || initial_idx == NULL || assignment_curr == NULL)
		{
			std::cerr << "[Error] Memory allocation fails in kmeans\n";
			return -1;
		}

		// initial assignment
		for(int i = 0; i < num; i++)
		{	
			assignment_curr[i] = -1;	
		}

		while(total_iter--)
		{
			double dis = 0;
			tw::GetIntialCenters(num, k, initial_idx);
			for(int i = 0; i < k; i++)
			{
				CopyDes2Double(means_curr + i * dim, p[initial_idx[i]], dim);
			}

			size_t change_num = ComputeAssignment(num, dim, k, p, means_curr, assignment_curr, dis);
			double change_pct = (double) change_num / num;
			assert(change_num == num);

			while(change_pct > change_pct_threshold)
			{
				// recompute means
				ComputeMeans(num, dim, k, p, assignment_curr, means_curr);

				// recompute assignments
				change_num = ComputeAssignment(num, dim, k, p, means_curr, assignment_curr, dis);
				change_pct = (double) change_num / num;
			}

			ComputeMeans(num, dim, k, p, assignment_curr, means_curr);
			if(dis < min_dis)
			{
				min_dis = dis;
				memcpy(means, means_curr, sizeof(double) * k * dim);
				memcpy(assignment, assignment_curr, sizeof(int) * num);
			}
		}

		delete [] means_curr;
		delete [] initial_idx;
		delete [] assignment_curr;

		return 0;
	}
}
