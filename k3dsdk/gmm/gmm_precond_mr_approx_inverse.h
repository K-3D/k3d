// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Copyright (C) 1997-2007 Yves Renard
//
// This file is a part of GETFEM++
//
// Getfem++ is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301,
// USA.
//
//========================================================================

/**@file gmm_precond_mr_approx_inverse.h
   @author  Andrew Lumsdaine <lums@osl.iu.edu>, Lie-Quan Lee     <llee@osl.iu.edu>
   @date June 5, 2003.
   @brief Approximate inverse via MR iteration.
*/

#ifndef GMM_PRECOND_MR_APPROX_INVERSE_H
#define GMM_PRECOND_MR_APPROX_INVERSE_H


#include "gmm_precond.h"

namespace gmm {

  /** Approximate inverse via MR iteration (see P301 of Saad book).
   */
  template <typename Matrix>
  struct mr_approx_inverse_precond {

    typedef typename linalg_traits<Matrix>::value_type value_type;
    typedef typename number_traits<value_type>::magnitude_type magnitude_type;
    typedef typename principal_orientation_type<typename
      linalg_traits<Matrix>::sub_orientation>::potype sub_orientation;
    typedef wsvector<value_type> VVector;
    typedef col_matrix<VVector> MMatrix;

    MMatrix M;
    size_type nb_it;
    magnitude_type threshold;

    void build_with(const Matrix& A);
    mr_approx_inverse_precond(const Matrix& A, size_type nb_it_,
			      magnitude_type threshold_)
      : M(mat_nrows(A), mat_ncols(A))
    { threshold = threshold_; nb_it = nb_it_; build_with(A); }
    mr_approx_inverse_precond(void)
    { threshold = magnitude_type(1E-7); nb_it = 5; }
    mr_approx_inverse_precond(size_type nb_it_, magnitude_type threshold_)
    { threshold = threshold_; nb_it = nb_it_; } 
    const MMatrix &approx_inverse(void) const { return M; }
  };

  template <typename Matrix, typename V1, typename V2> inline
  void mult(const mr_approx_inverse_precond<Matrix>& P, const V1 &v1, V2 &v2)
  { mult(P.M, v1, v2); }

  template <typename Matrix, typename V1, typename V2> inline
  void transposed_mult(const mr_approx_inverse_precond<Matrix>& P,
		       const V1 &v1,V2 &v2)
  { mult(gmm::conjugated(P.M), v1, v2); }

  template <typename Matrix>
  void mr_approx_inverse_precond<Matrix>::build_with(const Matrix& A) {
    gmm::resize(M, mat_nrows(A), mat_ncols(A));
    typedef value_type T;
    typedef magnitude_type R;
    VVector m(mat_ncols(A)),r(mat_ncols(A)),ei(mat_ncols(A)),Ar(mat_ncols(A)); 
    T alpha = mat_trace(A)/ mat_euclidean_norm_sqr(A);
    if (alpha == T(0)) alpha = T(1);
    
    for (size_type i = 0; i < mat_nrows(A); ++i) {
      gmm::clear(m); gmm::clear(ei); 
      m[i] = alpha;
      ei[i] = T(1);
      
      for (size_type j = 0; j < nb_it; ++j) {
	gmm::mult(A, gmm::scaled(m, T(-1)), r);
	gmm::add(ei, r);
	gmm::mult(A, r, Ar);
	T nAr = vect_sp(Ar,Ar);
	if (gmm::abs(nAr) > R(0)) {
	  gmm::add(gmm::scaled(r, gmm::safe_divide(vect_sp(r, Ar), vect_sp(Ar, Ar))), m);
	  gmm::clean(m, threshold * gmm::vect_norm2(m));
	} else gmm::clear(m);
      }
      if (gmm::vect_norm2(m) == R(0)) m[i] = alpha;
      gmm::copy(m, M.col(i));
    }
  }
}

#endif 

