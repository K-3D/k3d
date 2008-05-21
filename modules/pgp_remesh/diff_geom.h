// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief 
		\author Ian South-Dickinson (ian.southd@gmail.com)
*/

#ifndef PGP_DIFF_GEOM_H
#define PGP_DIFF_GEOM_H

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>
#include <iostream>
#include <map>
#include <utility>
#include "mesh_info.h"

namespace libk3dquadremesh
{

namespace detail {
	#include <modules/qslim/MxMath.h>
	#include <modules/qslim/MxTriangle.h>
	Vec3 triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	double triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	Vec3 triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3);


	/// TODO: Add desc
	class diff_geom 
	{
	
	public:
		typedef std::pair<double,double> tensor_t;
		
		diff_geom() 
		{
			init = false;
			valid = false;
		}

		diff_geom(const mesh_info* Mesh) 
			: mesh(Mesh)
		{
			init = false;
			valid = true;
		}

		~diff_geom() 
		{
		}


		/// initialize vectors and precompute values
		void initialize();

		/// Sends curvature directions to output mesh for drawing
		void dump_draw_data(k3d::mesh& OutputMesh);

		/// Take smoothing steps with timestep h, stable for all values of h
		void smooth(double h, int steps, bool four_symm);
		
		/// Returns normal of tangent plane at vertex
		Vec3 normal(vert_t vert);
		
		/// Calculate principal curvature tensor, returns error from previously calculated gaussian curvature
		double principal_curve_tensor(vert_t vert, tensor_t& tens);
				
		/// Converts symmetric tensor into isotropic tensor
		tensor_t isotropic_tensor(Vec3 tensor);
				 
		/// Project the vector into the tangent plane at vertex vert
		Vec3 project(vert_t vert, const Vec3& x);
		
		/// Mean weight of half edge
		double mean_weight(edge_t edge);

		/// Mean coordinates of half edge
		double mean_coord(edge_t e);		
		
		/// Mesh that is being worked with
		const mesh_info* mesh;
		
		/// Stores precomputed mean coordinates
		std::vector<double> mean_coords;

		/// Stores precomputed mean weights
		std::vector<double> mean_weights;

		/**
		 * Holds first column of each matrix in the isotropic curvature tensor field (curvature magnitudes have been normalize).
		 * Each vector is (cos(2*theta), sin(2*theta)), where theta is the angle between the tensor and the
		 * x-axis in the local tangent plane.
		 */
		std::vector< tensor_t > tensor;

		bool init, valid;
	
		/// X-axis basis for tangent plane at each vertex 
		std::vector<Vec3> tangent_basis_i;
		/// Y-axis basis for tangent plane at each vertex  
		std::vector<Vec3> tangent_basis_j;
		/// Z-axis basis for tangent plane at each vertex 
		std::vector<Vec3> tangent_basis_k;

//		gmm::csr_matrix<double> smoothing_mat;
	};
};

}; // namespace pgp_module
#endif
