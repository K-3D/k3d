// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Copyright (C) 2002-2007 Yves Renard
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

/**@file gmm.h
   @author  Yves Renard <Yves.Renard@insa-lyon.fr>
   @date October 13, 2002.
   @brief Include common gmm files.
*/
#ifndef GMM_H__
#define GMM_H__

#include "gmm_kernel.h"
#include "gmm_dense_lu.h"
#include "gmm_dense_qr.h"

#include "gmm_iter_solvers.h"
#include "gmm_condition_number.h"
#include "gmm_inoutput.h"

#include "gmm_lapack_interface.h"
#include "gmm_superlu_interface.h"


#include "gmm_domain_decomp.h"

#endif //  GMM_H__
