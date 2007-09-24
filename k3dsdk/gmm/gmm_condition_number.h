// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Copyright (C) 2003-2007 Yves Renard
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

/**@file gmm_condition_number.h
   @author  Yves Renard <Yves.Renard@insa-lyon.fr>, Julien Pommier <Julien.Pommier@insa-toulouse.fr>
   @date August 27, 2003.
   @brief computation of the condition number of dense matrices.
*/
#ifndef GMM_CONDITION_NUMBER_H__
#define GMM_CONDITION_NUMBER_H__

#include "gmm_dense_qr.h"

namespace gmm {

  /** computation of the condition number of dense matrices using SVD.

      Uses symmetric_qr_algorithm => dense matrices only.

      @param M a matrix.
      @param emin smallest (in magnitude) eigenvalue
      @param emax largest eigenvalue.
   */
  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  condition_number(const MAT& M, 
	  typename number_traits<typename
	  linalg_traits<MAT>::value_type>::magnitude_type& emin,
	  typename number_traits<typename
	  linalg_traits<MAT>::value_type>::magnitude_type& emax) {
    typedef typename linalg_traits<MAT>::value_type T;
    typedef typename number_traits<T>::magnitude_type R;

    size_type m = mat_nrows(M), n = mat_ncols(M);
    emax = emin = R(0);
    std::vector<R> eig(m+n);

    if (m+n == 0) return R(0);
    if (is_hermitian(M)) {
      eig.resize(m);
      gmm::symmetric_qr_algorithm(M, eig);
    }
    else {
      dense_matrix<T> B(m+n, m+n); // not very efficient ??
      gmm::copy(conjugated(M), sub_matrix(B, sub_interval(m, n), sub_interval(0, m)));
      gmm::copy(M, sub_matrix(B, sub_interval(0, m),
					  sub_interval(m, n)));
      gmm::symmetric_qr_algorithm(B, eig);
    }
    emin = emax = gmm::abs(eig[0]);
    for (size_type i = 1; i < eig.size(); ++i) {
      R e = gmm::abs(eig[i]); 
      emin = std::min(emin, e);
      emax = std::max(emax, e);
    }
    // cout << "emin = " << emin << " emax = " << emax << endl;
    if (emin == R(0)) return gmm::default_max(R());
    return emax / emin;
  }

  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  condition_number(const MAT& M) { 
    typename number_traits<typename
      linalg_traits<MAT>::value_type>::magnitude_type emax, emin;
    return condition_number(M, emin, emax);
  }

  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  Frobenius_condition_number_sqr(const MAT& M) { 
    typedef typename linalg_traits<MAT>::value_type T;
    typedef typename number_traits<T>::magnitude_type R;
    size_type m = mat_nrows(M), n = mat_ncols(M);
    dense_matrix<T> B(std::min(m,n), std::min(m,n));
    if (m < n) mult(M,gmm::conjugated(M),B);
    else       mult(gmm::conjugated(M),M,B);
    R trB = abs(mat_trace(B));
    lu_inverse(B);
    return trB*abs(mat_trace(B));
  }

  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  Frobenius_condition_number(const MAT& M)
  { return sqrt(Frobenius_condition_number_sqr(M)); }

  /** estimation of the condition number (TO BE DONE...)
   */
  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  condest(const MAT& M, 
	  typename number_traits<typename
	  linalg_traits<MAT>::value_type>::magnitude_type& emin,
	  typename number_traits<typename
	  linalg_traits<MAT>::value_type>::magnitude_type& emax) {
    return condition_number(M, emin, emax);
  }
  
  template <typename MAT> 
  typename number_traits<typename 
  linalg_traits<MAT>::value_type>::magnitude_type
  condest(const MAT& M) { 
    typename number_traits<typename
      linalg_traits<MAT>::value_type>::magnitude_type emax, emin;
    return condest(M, emin, emax);
  }
}

#endif
