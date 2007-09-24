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
#include "diff_geom.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gmm/gmm.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include <iostream>
#include <map>

namespace libk3dquadremesh
{

namespace detail {
	#include <modules/qslim/MxMath.h>
	#include <modules/qslim/MxTriangle.h>
	Vec3 triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
	{
		Vec3 a = v2 - v1;
		Vec3 b = v3 - v1;
		return a^b;
	}

	double triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3)
	{
		return 0.5 * norm(triangle_raw_normal(v1, v2, v3));
	}

	Vec3 triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
	{
		Vec3 n = triangle_raw_normal(v1, v2, v3);
		n.Normalize();
		
		return n;
	}

	void diff_geom::initialize() {
		tangent_basis_i.resize(mesh->num_verts);
		tangent_basis_j.resize(mesh->num_verts);
		tangent_basis_k.resize(mesh->num_verts);
		tensor.resize(mesh->num_verts);
		mean_coords.resize(mesh->num_edges);
		mean_weights.resize(mesh->num_edges);

		for(size_t i = 0; i < mesh->num_edges; ++i) {
			mean_weights[i] = mean_weight(i);
		}

		for(size_t i = 0; i < mesh->num_edges; ++i) {
			mean_coords[i] = mean_coord(i);
		}

		for(size_t i = 0; i < mesh->num_verts; ++i) {
			mesh_info::Vert v = mesh->getVert(i);
			mesh_info::Edge e = v.edge();
	
			Vec3 ihat = e.dir();
			ihat.Normalize();
			
			Vec3 khat = normal(i);
			Vec3 jhat = khat ^ ihat;
			ihat = jhat ^ khat;

			ihat.Normalize();
			jhat.Normalize();
			khat.Normalize();

			tangent_basis_i[i] = ihat;
			tangent_basis_j[i] = jhat;
			tangent_basis_k[i] = khat;
		}

		for(size_t i = 0; i < mesh->num_verts; ++i) {
			principal_curve_tensor(i, tensor[i]);
		}
	}
	
	void diff_geom::dump_draw_data(k3d::mesh& OutputMesh) 
	{
		k3d::typed_array<k3d::vector3>* p1_p = new k3d::typed_array<k3d::vector3>;
		boost::shared_ptr<k3d::typed_array<k3d::vector3> > p1(p1_p);

		k3d::typed_array<k3d::vector3>* p2_p = new k3d::typed_array<k3d::vector3>;
		boost::shared_ptr<k3d::typed_array<k3d::vector3> > p2(p2_p);

		p1->resize(mesh->num_verts);
		p2->resize(mesh->num_verts);


		Vec3 pc1,pc2, tens;

		for(size_t i = 0; i < mesh->num_verts; ++i)
		{
			if(std::abs(tensor[i].first) <= 1E-10 && std::abs(tensor[i].second) <= 1E-10) {
				p1->at(i).n[0] = 0;
				p1->at(i).n[1] = 0;
				p1->at(i).n[2] = 0;
				
				p2->at(i).n[0] = 0;
				p2->at(i).n[1] = 0;
				p2->at(i).n[2] = 0;

				continue;
			}
			double angle = 0.5*atan2(tensor[i].second , tensor[i].first);

			Vec3 temp_i = tangent_basis_i[i];
			Vec3 temp_j = tangent_basis_j[i];

			temp_i *= cos(angle);
			temp_j *= sin(angle);
			pc1 = temp_i + temp_j;

			temp_i = tangent_basis_i[i];
			temp_j = tangent_basis_j[i];
			temp_i *= cos(angle + k3d::pi_over_2());
			temp_j *= sin(angle + k3d::pi_over_2());
			pc2 = temp_i + temp_j;
			p1->at(i).n[0] = pc1[0];
			p1->at(i).n[1] = pc1[1];
			p1->at(i).n[2] = pc1[2];
			
			p2->at(i).n[0] = pc2[0];
			p2->at(i).n[1] = pc2[1];
			p2->at(i).n[2] = pc2[2];

		}

		OutputMesh.vertex_data["PGPPrincCurv1"] = p1;
		OutputMesh.vertex_data["PGPPrincCurv2"] = p2;
	}

	void diff_geom::smooth(double h, int steps, bool four_symm = false) 
	{
		if(steps == 0) return;
		double n = four_symm ? 4 : 2;		
		std::vector<double> B(2*mesh->num_verts);
		std::vector<double> x(2*mesh->num_verts);
		
		
		gmm::row_matrix< gmm::wsvector<double> > M1(2*mesh->num_verts, 2*mesh->num_verts);
		for(vert_t v = 0; v < mesh->num_verts; v++) {
			mesh_info::Edge e = mesh->getVert(v).edge();
			edge_t first = e();
			
			M1(2*v, 2*v) = 1.0+h;
			M1(2*v+1, 2*v+1) = 1.0+h;
			
			do {
				vert_t w = e.comp().vert()();

				Vec3 g_v = project(v, e.dir());
				Vec3 g_w = project(w, e.dir());

				double angle_i = atan2(g_v[1], g_v[0]);
				double angle_j = atan2(g_w[1], g_w[0]);
				while(angle_i < 0) angle_i += k3d::pi_times_2();
				while(angle_j < 0) angle_j += k3d::pi_times_2();
				double angle = angle_i - angle_j;
				while(angle < 0) angle += k3d::pi_times_2();

				double c = cos(n*angle);
				double s = sin(n*angle);

				M1(2*v, 2*w) = M1(2*v+1, 2*w+1) = -h*mean_coords[e()]*c;
				M1(2*v, 2*w+1) = h*mean_coords[e()]*s;
				M1(2*v+1, 2*w) = -h*mean_coords[e()]*s;			
			
				e = e.comp().next();
			} while(e() != first);		
		}

		gmm::csr_matrix<double> M; // pre compute this?
		gmm::clean(M1, 1E-8);
		gmm::copy(M1, M);

		if(four_symm) {
			for(int v = 0; v < mesh->num_verts; v++) {
				double angle = (0.5)*atan2(tensor[v].second, tensor[v].first);
				B[2*v]   = cos(n*angle);
				B[2*v+1] = sin(n*angle);
			} 
		} else {
			for(int v = 0; v < mesh->num_verts; v++) {
				B[2*v]   = tensor[v].first;
				B[2*v+1] = tensor[v].second;
			} 
		}


	
		gmm::diagonal_precond<gmm::csr_matrix<double> > PR(M);
		//gmm::identity_matrix PR;
		int i = 0;
		for(i = 0; i < steps; ++i) {
			gmm::iteration iter(1E-8);
			iter.set_noisy(0);
			iter.set_maxiter(150);
			gmm::bicgstab(M, x, B, PR, iter);
			if(i+1 < steps) {
				double delta = 0;
				for(vert_t v = 0; v < 2*mesh->num_verts; v++) {
					delta += (B[v]-x[v])*(B[v]-x[v]);
				}
				if(delta < 1E-8) break;
			}
			B.swap(x);

			
		}

		for(vert_t v = 0; v < mesh->num_verts; v++) {
			double angle = (1.0/n)*atan2(B[2*v+1], B[2*v]);
			tensor[v].first  = cos(2*angle);
			tensor[v].second = sin(2*angle);
		}

	}


	Vec3 diff_geom::normal(vert_t vert) 
	{
		//Vec3 mc = mean_curv[vert];
		Vec3 n;

		mesh_info::Edge e = mesh->getVert(vert).edge();
		edge_t first = e();
		n[0] = 0;
		n[1] = 0;
		n[2] = 0;
		int count = 0;
		do {
			n += triangle_normal(e.face()[0].start(), e.face()[1].start(), e.face()[2].start());
			count ++;
			e = e.comp().next();
		} while(e() != first);
		n *= (1.0/count);

		n.Normalize();		
		return n;
	}

	
	Vec3 diff_geom::project(vert_t vert, const Vec3& x) 
	{
		Vec3 dir = tangent_basis_k[vert];
		dir *= (tangent_basis_k[vert]*x);
		dir = x - dir;
		return Vec3(dir*tangent_basis_i[vert], dir*tangent_basis_j[vert], 0);
	}

	double diff_geom::principal_curve_tensor(vert_t vert, tensor_t& iso)
	{
		double d_x, d_y, w, k, len_square;

		bool print = false;
		mesh_info::Vert v = mesh->getVert(vert);
		mesh_info::Edge e = v.edge();
		vert_t first = e();

		Vec3 ihat = tangent_basis_i[vert];
		Vec3 jhat = tangent_basis_j[vert];
		Vec3 khat = tangent_basis_k[vert];

		Vec3 d;

		double len;
		int valence = 0;

		do {
			valence++;
			e = e.comp().next();
		} while(e() != first);

		gmm::dense_matrix<double> A(valence, 3);
		gmm::dense_matrix<double> AtA(3, 3);		
		std::vector<double> B(valence, 0.0);
		std::vector<double> AtB(3,0.0);
		std::vector<double> X(3,0.0);
		int i = 0;
		do {
			d = khat;
			d *= (khat*e.dir());
			d = e.dir() - d;
			d.Normalize();
			
			d_x = d*ihat;
			d_y = d*jhat; 
			
			len = sqrt(d_x*d_x + d_y*d_y);

			d_x /= len;
			d_y /= len;
			
			len_square = e.dir() * e.dir(); 

			w = mean_coords[e()];
			k = 2.0*(e.dir()*khat)/len_square;
			
			w = std::sqrt(std::abs(w));
			A(i, 0) = w* d_x * d_x;
			A(i, 1) = w* 2.0 *d_x * d_y;
			A(i, 2) = w* d_y * d_y;
			B[i] = w * k;
			i++;
			e = e.comp().next();
		} while(e() != first);

		std::vector<int> pivot; pivot.resize(3);

		gmm::mult(gmm::transposed(A), B, AtB);
		gmm::mult(gmm::transposed(A), A, AtA);

		gmm::lu_factor(AtA, pivot);
		double det = gmm::lu_det(AtA);

		if(std::abs(det) < 1E-10) {
			iso.first = 0;
			iso.second = 0;

			return 0;
		}

		gmm::lu_solve(AtA, pivot, X, AtB);
		
		Vec3 tens;
		tens[0] = X[0];
		tens[1] = X[1];
		tens[2] = X[2];

		iso = isotropic_tensor(tens);

		return 0.0;	
	}
	
	std::pair<double,double> diff_geom::isotropic_tensor(Vec3 tens) 
	{
		double lambda = 0.5*(tens[0] + tens[2]);
		Vec3 iso = Vec3(tens[0] - lambda, tens[1], 0);

		iso.Normalize();
		std::pair<double,double> ret;
		ret.first = iso[0];
		ret.second = iso[1];

		return ret;
	}


	double diff_geom::mean_weight(edge_t edge) 
	{
		mesh_info::Edge e(mesh->getEdge(edge));

		Vec3 e_v = e.dir();
		Vec3 enn_v = e.next().next().dir();
		Vec3 ecn_v = e.comp().next().dir();

		double inv_len = 1.0/e_v.Length();

		e_v *= inv_len;
		enn_v.Normalize();
		ecn_v.Normalize();

		double cos_a0 = -(e_v * enn_v);
		double cos_a1 = e_v * ecn_v;
		
		//double tan_half_a0 = std::sqrt((1-cos_a0)/(1+cos_a0));
		//double tan_half_a1 = std::sqrt((1-cos_a1)/(1+cos_a1));
		
		// Which one is faster/more accurate?
		 double tan_half_a0 = tan(0.5*acos(cos_a0));
		 double tan_half_a1 = tan(0.5*acos(cos_a1));
		
		return inv_len*(tan_half_a0 + tan_half_a1);
	}

	double diff_geom::mean_coord(edge_t edge)
	{
		mesh_info::Edge e = mesh->getEdge(edge);
		edge_t first = e();

		double sum = 0;

		do {
			sum += mean_weights[e()];
			e = e.comp().next();
		} while(e() != first);

		return mean_weights[first]/sum;
	}

};

}; // namespace pgp_module
