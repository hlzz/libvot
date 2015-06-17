#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <stdio.h>
#include <cmath>

#include <Eigen/Dense>
#include <Eigen/SVD>

using namespace std;

struct point2d
{
	float x,y;
};

float EuclideanDistance(point2d x1, point2d x2)
{
	return (x1.x - x2.x) * (x1.x - x2.x) + (x1.y - x2.y) * (x1.y - x2.y);
}

int main(int argc, char **argv)
{
	int input_size;
	cout << "the size of the matrix: \n";
	cin >> input_size;
	const int MATRIX_SIZE = input_size;
	string matrix_filename = "euclidean_matrix";
	stringstream ss;
	ss << matrix_filename << "_" << MATRIX_SIZE;
	ss >> matrix_filename;
	FILE *matrix_file = fopen(matrix_filename.c_str(), "w");

	point2d points[MATRIX_SIZE];
	for(int i = 0; i < MATRIX_SIZE; i++)
	{
		points[i].x = rand() % 100;	
		points[i].y = rand() % 100;
	}

	Eigen::MatrixXf distance_matrix(MATRIX_SIZE, MATRIX_SIZE);
	for(int i = 0; i < MATRIX_SIZE; i++)
	{
		distance_matrix(i, i) = 0.0;
		for(int j = i+1; j < MATRIX_SIZE; j++)
		{
			distance_matrix(i, j) = EuclideanDistance(points[i], points[j]);
			distance_matrix(j, i) = distance_matrix(i, j);
		}
	}

	// output to the file
	for(int i = 0; i < MATRIX_SIZE; i++)
	{
		for(int j = 0; j < MATRIX_SIZE; j++)
		{
			cout << distance_matrix(i, j) << " ";
			fprintf(matrix_file, "%f ", distance_matrix(i, j));
		}
		cout << endl;
		fprintf(matrix_file, "\n");
	}

	Eigen::FullPivLU<Eigen::MatrixXf> lu_decomp(distance_matrix);
	//lu_decomp.setThreshold(1e-5);
	cout << "rank of distance matrix: " << lu_decomp.rank() << endl;

	//Eigen::JacobiSVD<Eigen::MatrixXf> svd(distance_matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
	//cout << svd.singularValues() << endl;

	fclose(matrix_file);
	return 0;
}