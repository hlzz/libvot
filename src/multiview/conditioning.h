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
Copyright (c) 2010 libmv authors.

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

/*! \file conditioning.h
 * \brief normalization
 *
 */

#ifndef VOT_MULTIVIEW_CONDITIONING_H
#define VOT_MULTIVIEW_CONDITIONING_H

#include "numeric/numeric.h"

namespace vot
{
// Point conditioning (non isotropic)
void PreconditionerFromPoints(const Mat &points, Mat3 *T);
// Point conditioning (isotropic)
void IsotropicPreconditionerFromPoints(const Mat &points, Mat3 *T);

void ApplyTransformationToPoints(const Mat &points,
                                 const Mat3 &T,
                                 Mat *transformed_points);

void NormalizePoints(const Mat &points,
                     Mat *normalized_points,
                     Mat3 *T);

void NormalizeIsotropicPoints(const Mat &points,
                              Mat *normalized_points,
                              Mat3 *T);

/// Use inverse for unnormalize
struct UnnormalizerI {
	// Denormalize the results. See HZ page 109.
	static void Unnormalize(const Mat3 &T1, const Mat3 &T2, Mat3 *H);
};

/// Use transpose for unnormalize
struct UnnormalizerT {
	// Denormalize the results. See HZ page 109.
	static void Unnormalize(const Mat3 &T1, const Mat3 &T2, Mat3 *H);
};

}  // namespace vot

#endif  // VOT_MULTIVIEW_CONDITIONING_H
