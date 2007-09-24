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

/**@file gmm_dense_lu.h
   @author  Andrew Lumsdaine, Jeremy G. Siek, Lie-Quan Lee
   @date June 5, 2003.
   @brief LU factorizations and determinant computation for dense matrices.
*/
#ifndef GMM_DENSE_LU_H
#define GMM_DENSE_LU_H

#include "gmm_dense_Householder.h"
#include "gmm_opt.h"

namespace gmm {


  /** LU Factorization of a general (dense) matrix (real or complex).
  
  This is the outer product (a level-2 operation) form of the LU
  Factorization with pivoting algorithm . This is equivalent to
  LAPACK's dgetf2. Also see "Matrix Computations" 3rd Ed.  by Golub
  and Van Loan section 3.2.5 and especially page 115.
  
  The pivot indices in ipvt are indexed starting from 1
  so that this is compatible with LAPACK (Fortran).
  */
  template <typename DenseMatrix, typename Pvector>
  size_type lu_factor(DenseMatrix& A, Pvector& ipvt) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    typedef typename number_traits<T>::magnitude_type R;
    size_type info(0), i, j, jp, M(mat_nrows(A)), N(mat_ncols(A));
    size_type NN = std::min(M, N);
    std::vector<T> c(M), r(N);
    
    GMM_ASSERT2(ipvt.size()+1 >= NN, "IPVT too small");
    for (i = 0; i+1 < NN; ++i) ipvt[i] = i;
      
    if (M || N) {
      for (j = 0; j+1 < NN; ++j) {
	R max = gmm::abs(A(j,j)); jp = j;
	for (i = j+1; i < M; ++i)		   /* find pivot.          */
	  if (gmm::abs(A(i,j)) > max) { jp = i; max = gmm::abs(A(i,j)); }
	ipvt[j] = jp + 1;
	
	if (max == R(0)) { info = j + 1; break; }
        if (jp != j) for (i = 0; i < N; ++i) std::swap(A(jp, i), A(j, i));
	
        for (i = j+1; i < M; ++i) { A(i, j) /= A(j,j); c[i-j-1] = -A(i, j); }
        for (i = j+1; i < N; ++i) r[i-j-1] = A(j, i);  // avoid the copy ?
	rank_one_update(sub_matrix(A, sub_interval(j+1, M-j-1),
				 sub_interval(j+1, N-j-1)), c, conjugated(r));
      }
      ipvt[j] = j + 1;
    }
    return info;
  }
  
  /** LU Solve : Solve equation Ax=b, given an LU factored matrix.*/
  //  Thanks to Valient Gough for this routine!
  template <typename DenseMatrix, typename VectorB, typename VectorX,
	    typename Pvector>
  void lu_solve(const DenseMatrix &LU, const Pvector& pvector, 
		VectorX &x, const VectorB &b) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    copy(b, x);
    for(size_type i = 0; i < pvector.size(); ++i) {
      size_type perm = pvector[i]-1;     // permutations stored in 1's offset
      if(i != perm) { T aux = x[i]; x[i] = x[perm]; x[perm] = aux; }
    }
    /* solve  Ax = b  ->  LUx = b  ->  Ux = L^-1 b.                        */
    lower_tri_solve(LU, x, true);
    upper_tri_solve(LU, x, false);
  }

  template <typename DenseMatrix, typename VectorB, typename VectorX>
  void lu_solve(const DenseMatrix &A, VectorX &x, const VectorB &b) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    dense_matrix<T> B(mat_nrows(A), mat_ncols(A));
    std::vector<int> ipvt(mat_nrows(A));
    gmm::copy(A, B);
    size_type info = lu_factor(B, ipvt);
    GMM_ASSERT1(!info, "Singular system, pivot = " << info);
    lu_solve(B, ipvt, x, b);
  }
  
  template <typename DenseMatrix, typename VectorB, typename VectorX,
	    typename Pvector>
  void lu_solve_transposed(const DenseMatrix &LU, const Pvector& pvector, 
			   VectorX &x, const VectorB &b) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    copy(b, x);
    lower_tri_solve(transposed(LU), x, false);
    upper_tri_solve(transposed(LU), x, true);
    for(size_type i = pvector.size(); i > 0; --i) {
      size_type perm = pvector[i-1]-1;    // permutations stored in 1's offset
      if(i-1 != perm) { T aux = x[i-1]; x[i-1] = x[perm]; x[perm] = aux; }
    }

  }


  ///@cond DOXY_SHOW_ALL_FUNCTIONS
  template <typename DenseMatrixLU, typename DenseMatrix, typename Pvector>
  void lu_inverse(const DenseMatrixLU& LU, const Pvector& pvector,
		  DenseMatrix& AInv, col_major) {
    typedef typename linalg_traits<DenseMatrixLU>::value_type T;
    std::vector<T> tmp(pvector.size(), T(0));
    std::vector<T> result(pvector.size());
    for(size_type i = 0; i < pvector.size(); ++i) {
      tmp[i] = T(1);
      lu_solve(LU, pvector, result, tmp);
      copy(result, mat_col(AInv, i));
      tmp[i] = T(0);
    }
  }

  template <typename DenseMatrixLU, typename DenseMatrix, typename Pvector>
  void lu_inverse(const DenseMatrixLU& LU, const Pvector& pvector,
		  DenseMatrix& AInv, row_major) {
    typedef typename linalg_traits<DenseMatrixLU>::value_type T;
    std::vector<T> tmp(pvector.size(), T(0));
    std::vector<T> result(pvector.size());
    for(size_type i = 0; i < pvector.size(); ++i) {
      tmp[i] = T(1); // to be optimized !!
      // on peut sur le premier tri solve reduire le systeme
      // et peut etre faire un solve sur une serie de vecteurs au lieu
      // de vecteur a vecteur (accumulation directe de l'inverse dans la
      // matrice au fur et a mesure du calcul ... -> evite la copie finale
      lu_solve_transposed(LU, pvector, result, tmp);
      copy(result, mat_row(AInv, i));
      tmp[i] = T(0);
    }
  }
  ///@endcond  

  /** Given an LU factored matrix, build the inverse of the matrix. */
  template <typename DenseMatrixLU, typename DenseMatrix, typename Pvector>
  void lu_inverse(const DenseMatrixLU& LU, const Pvector& pvector,
		  const DenseMatrix& AInv_) {
    DenseMatrix& AInv = const_cast<DenseMatrix&>(AInv_);
    lu_inverse(LU, pvector, AInv, typename principal_orientation_type<typename
	       linalg_traits<DenseMatrix>::sub_orientation>::potype());
  }

  /** Given a dense matrix, build the inverse of the matrix, and
      return the determinant */
  template <typename DenseMatrix>
  typename linalg_traits<DenseMatrix>::value_type
  lu_inverse(const DenseMatrix& A_) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    DenseMatrix& A = const_cast<DenseMatrix&>(A_);
    dense_matrix<T> B(mat_nrows(A), mat_ncols(A));
    std::vector<int> ipvt(mat_nrows(A));
    gmm::copy(A, B);
    size_type info = lu_factor(B, ipvt);
    GMM_ASSERT1(!info, "Non invertible matrix, pivot = " << info);
    lu_inverse(B, ipvt, A);
    return lu_det(B, ipvt);
  }

  /** Compute the matrix determinant (via a LU factorization) */
  template <typename DenseMatrixLU, typename Pvector>
  typename linalg_traits<DenseMatrixLU>::value_type
  lu_det(const DenseMatrixLU& LU, const Pvector &pvector) {
    typedef typename linalg_traits<DenseMatrixLU>::value_type T;
    T det(1);
    for (size_type j = 0; j < std::min(mat_nrows(LU), mat_ncols(LU)); ++j)
      det *= LU(j,j);
    for(size_type i = 0; i < pvector.size(); ++i)
      if (i != size_type(pvector[i]-1)) { det = -det; }
    return det;
  }

  template <typename DenseMatrix>
  typename linalg_traits<DenseMatrix>::value_type
  lu_det(const DenseMatrix& A) {
    typedef typename linalg_traits<DenseMatrix>::value_type T;
    dense_matrix<T> B(mat_nrows(A), mat_ncols(A));
    std::vector<int> ipvt(mat_nrows(A));
    gmm::copy(A, B);
    lu_factor(B, ipvt);
    return lu_det(B, ipvt);
  }

}

#endif

