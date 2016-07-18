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

/*! \file numeric.cpp
 * \brief This files defines some numeric utility functions.
 *
 */

#include "numeric.h"

namespace vot
{
Mat3 CrossProductMatrix(const Vec3 &x) {
	Mat3 X;
	X <<     0, -x(2),  x(1),
	        x(2),     0, -x(0),
	        -x(1),  x(0),     0;
	return X;
}

void MeanAndVarianceAlongRows(const Mat &A,
                              Vec *mean_pointer,
                              Vec *variance_pointer)
{
	Vec &mean = *mean_pointer;
	Vec &variance = *variance_pointer;
	int n = A.rows();
	int m = A.cols();
	mean.resize(n);
	variance.resize(n);

	for (int i = 0; i < n; ++i) {
		mean(i) = 0;
		variance(i) = 0;
		for (int j = 0; j < m; ++j) {
			double x = A(i, j);
			mean(i) += x;
			variance(i) += x * x;
		}
	}

	mean /= m;
	for (int i = 0; i < n; ++i) {
		variance(i) = variance(i) / m - Square(mean(i));
	}
}

} 	// end of namespace vot
