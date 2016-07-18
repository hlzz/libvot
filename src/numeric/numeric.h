/*
Copyright (c) 2016, Tianwei Shen
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

/*
Copyright (c) 2007, 2008, 2011 libmv authors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

/*! \file numeric.h
 * \brief This files defines some numeric utility functions.
 *
 */

#ifndef VOT_NUMERIC_NUMERIC_H
#define VOT_NUMERIC_NUMERIC_H

#include <Eigen/Dense>

namespace vot
{
typedef Eigen::VectorXf Vecf;
typedef Eigen::VectorXd Vec;
typedef Eigen::Vector2d Vec2;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector4d Vec4;
typedef Eigen::Vector2f Vec2f;
typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector4f Vec4f;
typedef Eigen::VectorXi VecXi;
typedef Eigen::Vector2i Vec2i;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Vector4i Vec4i;

typedef Eigen::MatrixXf Matf;
typedef Eigen::MatrixXd Mat;
typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> Matu;
typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> Vecu;
typedef Eigen::Matrix<unsigned int, 2, 1> Vec2u;
typedef Eigen::Matrix<double, 2, 2> Mat2;
typedef Eigen::Matrix<double, 2, 3> Mat23;
typedef Eigen::Matrix<double, 3, 3> Mat3;
typedef Eigen::Matrix<double, 3, 4> Mat34;
typedef Eigen::Matrix<double, 3, 5> Mat35;
typedef Eigen::Matrix<double, 4, 1> Mat41;
typedef Eigen::Matrix<double, 4, 3> Mat43;
typedef Eigen::Matrix<double, 4, 4> Mat4;
typedef Eigen::Matrix<double, 4, 6> Mat46;
typedef Eigen::Matrix<float, 2, 2> Mat2f;
typedef Eigen::Matrix<float, 2, 3> Mat23f;
typedef Eigen::Matrix<float, 3, 3> Mat3f;
typedef Eigen::Matrix<float, 3, 4> Mat34f;
typedef Eigen::Matrix<float, 3, 5> Mat35f;
typedef Eigen::Matrix<float, 4, 3> Mat43f;
typedef Eigen::Matrix<float, 4, 4> Mat4f;
typedef Eigen::Matrix<float, 4, 6> Mat46f;
typedef Eigen::Matrix<double, 3, 3, Eigen::RowMajor> RMat3;
typedef Eigen::Matrix<double, 4, 4, Eigen::RowMajor> RMat4;
typedef Eigen::Matrix<double, 2, Eigen::Dynamic> Mat2X;
typedef Eigen::Matrix<double, 3, Eigen::Dynamic> Mat3X;
typedef Eigen::Matrix<double, 4, Eigen::Dynamic> Mat4X;
typedef Eigen::Matrix<double, Eigen::Dynamic,  2> MatX2;
typedef Eigen::Matrix<double, Eigen::Dynamic,  3> MatX3;
typedef Eigen::Matrix<double, Eigen::Dynamic,  4> MatX4;
typedef Eigen::Matrix<double, Eigen::Dynamic,  5> MatX5;
typedef Eigen::Matrix<double, Eigen::Dynamic,  6> MatX6;
typedef Eigen::Matrix<double, Eigen::Dynamic,  7> MatX7;
typedef Eigen::Matrix<double, Eigen::Dynamic,  8> MatX8;
typedef Eigen::Matrix<double, Eigen::Dynamic,  9> MatX9;
typedef Eigen::Matrix<double, Eigen::Dynamic, 15> MatX15;
typedef Eigen::Matrix<double, Eigen::Dynamic, 16> MatX16;

typedef Eigen::Matrix<double, 5, 1>  Vec5;
typedef Eigen::Matrix<double, 6, 1>  Vec6;
typedef Eigen::Matrix<double, 7, 1>  Vec7;
typedef Eigen::Matrix<double, 8, 1>  Vec8;
typedef Eigen::Matrix<double, 9, 1>  Vec9;
typedef Eigen::Matrix<double, 10, 1> Vec10;
typedef Eigen::Matrix<double, 11, 1> Vec11;
typedef Eigen::Matrix<double, 12, 1> Vec12;
typedef Eigen::Matrix<double, 13, 1> Vec13;
typedef Eigen::Matrix<double, 14, 1> Vec14;
typedef Eigen::Matrix<double, 15, 1> Vec15;
typedef Eigen::Matrix<double, 16, 1> Vec16;
typedef Eigen::Matrix<double, 17, 1> Vec17;
typedef Eigen::Matrix<double, 18, 1> Vec18;
typedef Eigen::Matrix<double, 19, 1> Vec19;
typedef Eigen::Matrix<double, 20, 1> Vec20;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RMatf;

typedef Eigen::NumTraits<double> EigenDouble;
using Eigen::Map;
using Eigen::Dynamic;
using Eigen::Matrix;

// Solve the linear system Ax = 0 via SVD. Store the solution in x, such that
// ||x|| = 1.0. Return the singluar value corresponding to the solution.
// Destroys A and resizes x if necessary.
// TODO(maclean): Take the SVD of the transpose instead of this zero padding.
template <typename TMat, typename TVec>
double Nullspace(TMat *A, TVec *nullspace)
{
	Eigen::JacobiSVD<TMat> svd(*A, Eigen::ComputeFullV);
	(*nullspace) = svd.matrixV().col(A->cols()-1);
	if (A->rows() >= A->cols())
		return svd.singularValues()(A->cols()-1);
	else
		return 0.0;
}

// Solve the linear system Ax = 0 via SVD. Finds two solutions, x1 and x2, such
// that x1 is the best solution and x2 is the next best solution (in the L2
// norm sense). Store the solution in x1 and x2, such that ||x|| = 1.0. Return
// the singluar value corresponding to the solution x1.  Destroys A and resizes
// x if necessary.
template <typename TMat, typename TVec1, typename TVec2>
double Nullspace2(TMat *A, TVec1 *x1, TVec2 *x2)
{
	Eigen::JacobiSVD<TMat> svd(*A, Eigen::ComputeFullV);
	*x1 = svd.matrixV().col(A->cols() - 1);
	*x2 = svd.matrixV().col(A->cols() - 2);
	if (A->rows() >= A->cols())
		return svd.singularValues()(A->cols()-1);
	else
		return 0.0;
}

// In place transpose for square matrices.
template<class TA>
inline void TransposeInPlace(TA *A)
{
	*A = A->transpose().eval();
}

template<typename TVec>
inline double NormL1(const TVec &x)
{
	return x.array().abs().sum();
}

template<typename TVec>
inline double NormL2(const TVec &x)
{
	return x.norm();
}

template<typename TVec>
inline double NormLInfinity(const TVec &x)
{
	return x.array().abs().maxCoeff();
}

template<typename TVec>
inline double DistanceL1(const TVec &x, const TVec &y)
{
	return (x - y).array().abs().sum();
}

template<typename TVec>
inline double DistanceL2(const TVec &x, const TVec &y)
{
	return (x - y).norm();
}
template<typename TVec>
inline double DistanceLInfinity(const TVec &x, const TVec &y)
{
	return (x - y).array().abs().maxCoeff();
}

// Normalize a vector with the L1 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL1(TVec *x)
{
	double norm = NormL1(*x);
	*x /= norm;
	return norm;
}

// Normalize a vector with the L2 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL2(TVec *x)
{
	double norm = NormL2(*x);
	*x /= norm;
	return norm;
}

// Normalize a vector with the L^Infinity norm, and return the norm before it
// was normalized.
template<typename TVec>
inline double NormalizeLInfinity(TVec *x)
{
	double norm = NormLInfinity(*x);
	*x /= norm;
	return norm;
}

// Return the square of a number.
template<typename T>
inline T Square(T x)
{
	return x * x;
}

template<typename TTop, typename TBot, typename TStacked>
void VerticalStack(const TTop &top, const TBot &bottom, TStacked *stacked)
{
	assert(top.cols() == bottom.cols());
	int n1 = top.rows();
	int n2 = bottom.rows();
	int m = top.cols();

	stacked->resize(n1 + n2, m);
	stacked->block(0,  0, n1, m) = top;
	stacked->block(n1, 0, n2, m) = bottom;
}

// Return a diagonal matrix from a vector containg the diagonal values.
template <typename TVec>
inline Mat Diag(const TVec &x)
{
	return x.asDiagonal();
}

template<typename TMat>
inline double FrobeniusNorm(const TMat &A)
{
	return sqrt(A.array().abs2().sum());
}

template<typename TMat>
inline double FrobeniusDistance(const TMat &A, const TMat &B)
{
	return FrobeniusNorm(A - B);
}

inline Vec3 CrossProduct(const Vec3 &x, const Vec3 &y)
{
	return x.cross(y);
}

Mat3 CrossProductMatrix(const Vec3 &x);

void MeanAndVarianceAlongRows(const Mat &A,
                              Vec *mean_pointer,
                              Vec *variance_pointer);

// Solve the cubic polynomial
//
//   x^3 + a*x^2 + b*x + c = 0
//
// The number of roots (from zero to three) is returned. If the number of roots
// is less than three, then higher numbered x's are not changed. For example,
// if there are 2 roots, only x0 and x1 are set.
//
// The GSL cubic solver was used as a reference for this routine.
template<typename Real>
int SolveCubicPolynomial(Real a, Real b, Real c,
                         Real *x0, Real *x1, Real *x2)
{
	Real q = a * a - 3 * b;
	Real r = 2 * a * a * a - 9 * a * b + 27 * c;

	Real Q = q / 9;
	Real R = r / 54;

	Real Q3 = Q * Q * Q;
	Real R2 = R * R;

	Real CR2 = 729 * r * r;
	Real CQ3 = 2916 * q * q * q;

	if (R == 0 && Q == 0) {
		// Tripple root in one place.
		*x0 = *x1 = *x2 = -a / 3;
		return 3;

	}
	else if (CR2 == CQ3) {
		// This test is actually R2 == Q3, written in a form suitable for exact
		// computation with integers.
		//
		// Due to finite precision some double roots may be missed, and considered
		// to be a pair of complex roots z = x +/- epsilon i close to the real
		// axis.
		Real sqrtQ = sqrt(Q);
		if (R > 0) {
			*x0 = -2 * sqrtQ - a / 3;
			*x1 =      sqrtQ - a / 3;
			*x2 =      sqrtQ - a / 3;
		} else {
			*x0 =     -sqrtQ - a / 3;
			*x1 =     -sqrtQ - a / 3;
			*x2 =  2 * sqrtQ - a / 3;
		}
		return 3;

	}
	else if (CR2 < CQ3) {
		// This case is equivalent to R2 < Q3.
		Real sqrtQ = sqrt(Q);
		Real sqrtQ3 = sqrtQ * sqrtQ * sqrtQ;
		Real theta = acos(R / sqrtQ3);
		Real norm = -2 * sqrtQ;
		*x0 = norm * cos(theta / 3) - a / 3;
		*x1 = norm * cos((theta + 2.0 * M_PI) / 3) - a / 3;
		*x2 = norm * cos((theta - 2.0 * M_PI) / 3) - a / 3;

		// Put the roots in ascending order.
		if (*x0 > *x1) {
			std::swap(*x0, *x1);
		}
		if (*x1 > *x2) {
			std::swap(*x1, *x2);
			if (*x0 > *x1) {
				std::swap(*x0, *x1);
			}
		}
		return 3;
	}
	Real sgnR = (R >= 0 ? 1 : -1);
	Real A = -sgnR * pow(fabs(R) + sqrt(R2 - Q3), 1.0/3.0);
	Real B = Q / A;
	*x0 = A + B - a / 3;
	return 1;
}

// The coefficients are in ascending powers, i.e. coeffs[N]*x^N.
template<typename Real>
int SolveCubicPolynomial(const Real *coeffs, Real *solutions)
{
	if (coeffs[0] == 0.0) {
		// TODO(keir): This is a quadratic not a cubic. Implement a quadratic
		// solver!
		return 0;
	}
	Real a = coeffs[2] / coeffs[3];
	Real b = coeffs[1] / coeffs[3];
	Real c = coeffs[0] / coeffs[3];
	return SolveCubicPolynomial(a, b, c, solutions + 0, solutions + 1, solutions + 2);
}
}	// end of namespace vot

#endif // VOT_NUMERIC_NUMERIC_H
