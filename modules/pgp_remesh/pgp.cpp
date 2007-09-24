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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>
#include <iostream>
#include <map>
#include "pgp.h"
#include "mesh_info.h"
#include "diff_geom.h"

namespace libk3dquadremesh
{

namespace detail {
	void PGP::setup_vf(bool align) 
	{
		face_data.resize(mesh->num_faces);
		vert_data.resize(mesh->num_verts);
		edge_data.resize(mesh->num_edges);


		for(vert_t v = 0; v < mesh->num_verts; ++v) {
			double angle = 0.5*atan2(geom->tensor[v].second , geom->tensor[v].first);

			Vec3 temp_i = geom->tangent_basis_i[v];
			Vec3 temp_j = geom->tangent_basis_j[v];

			temp_i *= cos(angle);
			temp_j *= sin(angle);
			vert_data[v].dir = temp_i + temp_j;
		}

		for(face_t f = 0; f < mesh->num_faces; ++f) {
			mesh_info::Face face = mesh->getFace(f);
			per_face &pf = face_data[f];
			pf.edge[0] = face[0]();
			pf.edge[1] = face[1]();
			pf.edge[2] = face[2]();

			pf.vert[0] = mesh->edge_vert[pf.edge[2]];
			pf.vert[1] = mesh->edge_vert[pf.edge[0]];
			pf.vert[2] = mesh->edge_vert[pf.edge[1]];

			mesh_info::Vert v0 = mesh->getVert(pf.vert[0]);
			mesh_info::Vert v1 = mesh->getVert(pf.vert[1]);
			mesh_info::Vert v2 = mesh->getVert(pf.vert[2]);
			mesh_info::Edge e0 = mesh->getEdge(pf.edge[0]);
			mesh_info::Edge e1 = mesh->getEdge(pf.edge[1]);
			mesh_info::Edge e2 = mesh->getEdge(pf.edge[2]);

			Vec3 norm = triangle_normal(v0.pos(), v1.pos(), v2.pos());
			Vec3 i = e0.dir();
			i.Normalize();
			Vec3 j = norm^i;
			j.Normalize();

			pf.e[0].first = e0.dir()*i;
			pf.e[0].second = 0.0;

			pf.e[1].first = e1.dir()*i;
			pf.e[1].second = e1.dir()*j;

			pf.e[2].first = e2.dir()*i;
			pf.e[2].second = e2.dir()*j;

			pf.v[1] = vec2(0.0, 0.0);
			pf.v[2] = pf.v[1] + pf.e[0];
			pf.v[0] = pf.v[2] + pf.e[1];

			Vec3 temp = vert_data[pf.vert[0]].dir;
			temp = (temp*norm)*norm;
			temp = vert_data[pf.vert[0]].dir - temp;
			pf.vf[0].first  = temp*i;
			pf.vf[0].second = temp*j;

			temp = vert_data[pf.vert[1]].dir;
			temp = (temp*norm)*norm;
			temp = vert_data[pf.vert[1]].dir - temp;
			pf.vf[1].first  = temp*i;
			pf.vf[1].second = temp*j;

			temp = vert_data[pf.vert[2]].dir;
			temp = (temp*norm)*norm;
			temp = vert_data[pf.vert[2]].dir - temp;
			pf.vf[2].first  = temp*i;
			pf.vf[2].second = temp*j;

			pf.angle[0] = atan2(pf.vf[0].second, pf.vf[0].first);
			pf.angle[1] = atan2(pf.vf[1].second, pf.vf[1].first);
			pf.angle[2] = atan2(pf.vf[2].second, pf.vf[2].first);
			if(pf.angle[0] < 0) pf.angle[0] += k3d::pi_times_2();
			if(pf.angle[1] < 0) pf.angle[1] += k3d::pi_times_2();
			if(pf.angle[2] < 0) pf.angle[2] += k3d::pi_times_2();

		}
	}

	void PGP::curl_correction() 
	{
		const double test = 1.0;
		const double test0 = -1.0;
		gmm::row_matrix< gmm::wsvector<double> > G(2*mesh->num_faces, mesh->num_verts);
		std::vector<double> B(2*mesh->num_faces,0.0);
		std::vector<double> X(mesh->num_verts,0.0);
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			mesh_info::Face face = mesh->getFace(f);
			per_face &pf = face_data[f];
			
			double area = triangle_area(mesh->getVert(pf.vert[0]).pos(), 
										mesh->getVert(pf.vert[1]).pos(),
										mesh->getVert(pf.vert[2]).pos());

			area = 0.5*(1.0/sqrt(area));
			
			double angleDiff[3];
			angleDiff[0] = pf.angle[2] - pf.angle[1];
            angleDiff[1] = pf.angle[0] - pf.angle[2];
            angleDiff[2] = pf.angle[1] - pf.angle[2];			
			if(angleDiff[0] > k3d::pi())      angleDiff[0] -= k3d::pi_times_2();
			if(angleDiff[0] < -1.0*k3d::pi()) angleDiff[0] += k3d::pi_times_2();			
			if(angleDiff[1] > k3d::pi())      angleDiff[1] -= k3d::pi_times_2();
			if(angleDiff[1] < -1.0*k3d::pi()) angleDiff[1] += k3d::pi_times_2();			
			if(angleDiff[2] > k3d::pi())      angleDiff[2] -= k3d::pi_times_2();
			if(angleDiff[2] < -1.0*k3d::pi()) angleDiff[2] += k3d::pi_times_2();			

			G(2*f + 0, pf.vert[0]) = area*(pf.v[1].second - pf.v[2].second);
			G(2*f + 1, pf.vert[0]) = area*(pf.v[2].first - pf.v[1].first);

			G(2*f + 0, pf.vert[1]) = area*(pf.v[2].second - pf.v[0].second);
			G(2*f + 1, pf.vert[1]) = area*(pf.v[0].first - pf.v[2].first);

			G(2*f + 0, pf.vert[2]) = area*(pf.v[0].second - pf.v[1].second);
			G(2*f + 1, pf.vert[2]) = area*(pf.v[1].first - pf.v[0].first);

			B[2*f + 0] = -1.0*area*(  (pf.v[2].first - pf.v[1].first)*angleDiff[0] 
								    + (pf.v[0].first - pf.v[2].first)*angleDiff[1] 
								    + (pf.v[1].first - pf.v[0].first)*angleDiff[2]);

			B[2*f + 1] = area*(  (pf.v[1].second - pf.v[2].second)*angleDiff[0] 
			                   + (pf.v[2].second - pf.v[0].second)*angleDiff[1] 
							   + (pf.v[0].second - pf.v[1].second)*angleDiff[2]);
		}

		gmm::row_matrix< gmm::wsvector<double> > G2(mesh->num_verts, mesh->num_verts);
		std::vector<double> B2(mesh->num_verts,0.0);

		gmm::mult(gmm::transposed(G), B, B2);
		gmm::mult(gmm::transposed(G), G, G2);
		
		gmm::csr_matrix<double> Gc;
		gmm::clean(G2, 1E-8);
		gmm::copy(G2, Gc);

		gmm::diagonal_precond<gmm::csr_matrix<double> > PR(Gc);
		gmm::iteration iter(1E-10);
		iter.set_noisy(1);
		gmm::bicgstab(Gc, X, B2, PR, iter);
		
		double max = 0;
		for(vert_t v = 0; v < mesh->num_verts; ++v) {
			X[v] = exp(X[v]);
			if(X[v] > max) max = X[v];
		}
		//std::cout << " Max " << max;
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			mesh_info::Face face = mesh->getFace(f);
			per_face &pf = face_data[f];
			pf.vf[0] =	(X[pf.vert[0]]/max)*pf.vf[0];
			pf.vf[1] =	(X[pf.vert[1]]/max)*pf.vf[1];
			pf.vf[2] =	(X[pf.vert[2]]/max)*pf.vf[2];
		}
	}

	void PGP::setup(double omega) 
	{
		// Fill face data with edge and vert indices, and project vf into triangle
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			mesh_info::Face face = mesh->getFace(f);

			per_face &pf = face_data[f];

			std::vector<double> x(3);
			std::vector<double> b(3);
			b[0] = 1.0;
			b[1] = 1.0;
			b[2] = 0.0;

			gmm::dense_matrix<double> L(3,3);
			L(0,0) = pf.e[0].first*pf.e[0].first;
			L(0,1) = pf.e[1].first*pf.e[1].first;
			L(0,2) = pf.e[2].first*pf.e[2].first;
			L(1,0) = pf.e[0].second*pf.e[0].second;
			L(1,1) = pf.e[1].second*pf.e[1].second;
			L(1,2) = pf.e[2].second*pf.e[2].second;
			L(2,0) = 2.0*pf.e[0].first*pf.e[0].second;
			L(2,1) = 2.0*pf.e[1].first*pf.e[1].second;
			L(2,2) = 2.0*pf.e[2].first*pf.e[2].second;

			gmm::lu_solve(L, x, b);

			pf.lamda[0] = x[0];
			pf.lamda[1] = x[1];
			pf.lamda[2] = x[2];
			
			//pf.lamda[0] = 1;
			//pf.lamda[1] = 1;
			//pf.lamda[2] = 1;

			//std::cout << "t = " << f << " L = " << "(" << x[0] << ", " << x[1] << ", " << x[2] << ")\n";

			double max1 = pf.vf[0]*pf.vf[1]; 
			double max2 = pf.vf[0]*pf.vf[2]; 

			int max_arg1 = 0;
			int max_arg2 = 0;

			//for(int i = 2; i < 4; i+=2) {
			for(int i = 1; i < 4; i++) {
				double dot1 = pf.vf[0]*rotate90(pf.vf[1], i);
				double dot2 = pf.vf[0]*rotate90(pf.vf[2], i);

				if(dot1 > max1) {
					max_arg1 = i;
					max1 = dot1;
				}
				if(dot2 > max2) {
					max_arg2 = i;
					max2 = dot2;
				}
			}

			pf.rot[0] = 0;
			pf.rot[1] = max_arg1;
			pf.rot[2] = max_arg2;

			for(int i = 0; i < 3; ++i) {
				int v0 = (i+1)%3;
				int v1 = (i+2)%3;

				pf.delta[i] = omega*0.5*((rotate90(pf.vf[v0], pf.rot[v0]) + rotate90(pf.vf[v1], pf.rot[v1]))*pf.e[i]);
				pf.delta_p[i] = omega*0.5*((rotate90(pf.vf[v0], pf.rot[v0]+1) + rotate90(pf.vf[v1], pf.rot[v1]+1))*pf.e[i]);
			}
		}
	}

	void PGP::solve() 
	{
		gmm::dense_matrix<double> D(4,4);
		gmm::dense_matrix<double> Temp1(4,4);
		gmm::dense_matrix<double> Temp2(4,4);
		double c_a = cos(k3d::pi()/180.0);
		double s_a = sin(k3d::pi()/180.0);
		M[0].resize(4,4);
		M[1].resize(4,4);
		M[2].resize(4,4);

		//M[0](0,2) = -1;
		//M[0](1,3) = 1;

		M[0](0,2) = 1;
		M[0](1,3) = -1;

		M[0](2,0) = 1;
		M[0](3,1) = 1;

		gmm::mult(M[0], M[0], M[1]);
		gmm::mult(M[0], M[1], M[2]);
		
		//std::cout << D << std::endl;			
		//std::cout << M[0] << std::endl;			
		//std::cout << M[1] << std::endl;			
		//std::cout << M[2] << std::endl;			
		
		std::vector<int> mapping(mesh->num_verts, 0);
		

		for(size_t p = 0; p < mesh->mesh->polyhedra->first_faces->size(); p++) {
			face_t f = mesh->mesh->polyhedra->first_faces->at(p);
			mapping[face_data[f].vert[0]] = -face_data[f].vert[0];
		}
		
		int curr = 0;
		for(vert_t v = 0; v < mapping.size(); ++v) {
			if(mapping[v] == 0) {
				mapping[v] = curr++;
			}
		}
		gmm::row_matrix< gmm::wsvector<double> > A(4*curr, 4*curr);
		std::vector<double> X(4*curr, 0.0);
		std::vector<double> B(4*curr, 0.0);

		std::vector<double> sum_lamba(mesh->num_verts, 0.0);
		
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			for(int e = 0; e < 3; ++e) {
				vert_t i = face_data[f].vert[(e+1)%3];
				vert_t j = face_data[f].vert[(e+2)%3];
				sum_lamba[i] += face_data[f].lamda[e];
				sum_lamba[j] += face_data[f].lamda[e];
				vec2 f; 
			}								
		}
		
		for(vert_t v = 0; v < mapping.size(); ++v) {
			if(mapping[v] >= 0) {
				vert_t vm = 4*mapping[v];
				//std::cout << v << "->" << vm << " = " << sum_lamba[v] << std::endl;
				A(vm + 0, vm + 0) = sum_lamba[v];
				A(vm + 1, vm + 1) = sum_lamba[v];
				A(vm + 2, vm + 2) = sum_lamba[v];
				A(vm + 3, vm + 3) = sum_lamba[v];
			}
		}
					
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			for(int e = 0; e < 3; ++e) {
				int v0 = (e+1)%3;
				int v1 = (e+2)%3;
				int i = 4*mapping[face_data[f].vert[v0]];
				int j = 4*mapping[face_data[f].vert[v1]];
				bool constrain0 = (mapping[face_data[f].vert[v0]] < 0);
				bool constrain1 = (mapping[face_data[f].vert[v1]] < 0);
				
				// Taken care of in seperate loop
				//if(!constrain0) {
				//	A(i + 0, i + 0) += face_data[f].lamda[e];
				//	A(i + 1, i + 1) += face_data[f].lamda[e];
				//	A(i + 2, i + 2) += face_data[f].lamda[e];
				//	A(i + 3, i + 3) += face_data[f].lamda[e];
				//}

				//if(!constrain1) {
				//	A(j + 0, j + 0) += face_data[f].lamda[e];
				//	A(j + 1, j + 1) += face_data[f].lamda[e];
				//	A(j + 2, j + 2) += face_data[f].lamda[e];
				//	A(j + 3, j + 3) += face_data[f].lamda[e];
				//}

				double cos_d = face_data[f].lamda[e]*std::cos(face_data[f].delta[e]);
				double sin_d = face_data[f].lamda[e]*std::sin(face_data[f].delta[e]);
				double cos_dt = face_data[f].lamda[e]*std::cos(face_data[f].delta_p[e]);
				double sin_dt = face_data[f].lamda[e]*std::sin(face_data[f].delta_p[e]);
				if(f == 0) {

//					std::cout << cos_d << " " << cos_dt << " " << sin_d << " "<< sin_dt << " " << std::endl;
//					std::cout << D << std::endl;
				}
				gmm::clear(D);
//				if(f == 0) std::cout << "0 " << D << std::endl;
				D(0,0) = -cos_d;
//				if(f == 0) std::cout << "1 " << D << std::endl;
				D(1,1) = -cos_d;
//				if(f == 0) std::cout << "2 " << D << std::endl;
				D(0,1) = sin_d;
//				if(f == 0) std::cout << "3 " << D << std::endl;
				D(1,0) = -sin_d;
//				if(f == 0) std::cout << "4 " << D << std::endl;

				D(2,2) = -cos_dt;
//				if(f == 0) std::cout << "5 " << D << std::endl;
				D(3,3) = -cos_dt;
//				if(f == 0) std::cout << "6 " << D << std::endl;
				D(2,3) = sin_dt;
//				if(f == 0) std::cout << "7 " << D << std::endl;
				D(3,2) = -sin_dt;
//				if(f == 0) std::cout << "8 " << D << std::endl;
//					std::cout << D << std::endl;
				if(f == 0) {
//					std::cout << "8 " << D << std::endl;
				}
				
				int r0 = face_data[f].rot[v0];
				int r1 = face_data[f].rot[v1];
				if((r1 % 2) == 1) r1 = (r1+2)%4;
				//k3d::log() << f << ": " << r0 << " " << r1 << std::endl;
				if(r0 == 0 && r1 == 0) {
					gmm::copy(D, Temp2);
				} else if (r0 == 0 && r1 != 0) {
					//k3d::log() << "mult 1" <<std::endl;
					gmm::mult(M[r1-1], D, Temp2);
				} else if (r0 != 0 && r1 == 0) {
					//k3d::log() << "mult 2" <<std::endl;
					gmm::mult(D, M[r0-1], Temp2);
				} else {
					//k3d::log() << "mult 3" <<std::endl;
					gmm::mult(D, M[r0-1], Temp1);
					//k3d::log() << "mult 4" <<std::endl;
					gmm::mult(M[r1-1], Temp1, Temp2);
				}
				//if(f == 0) {
				//	std::cout << Temp2 << std::endl;
				//}

				if(constrain0 && !constrain1) {
					//std::cout << "con0 |";
					double den = 1.0/sum_lamba[face_data[f].vert[v0]];
					//std::cout << "|";

					//B[j + 0] -= (Temp2(0,0) + Temp2(0,2))*den;
					//B[j + 1] -= (Temp2(1,0) + Temp2(1,2))*den;
					//B[j + 2] -= (Temp2(2,0) + Temp2(2,2))*den;
					//B[j + 3] -= (Temp2(3,0) + Temp2(3,2))*den;
					B[j + 0] -= (c_a*Temp2(0,0) + s_a*Temp2(0,1) + c_a*Temp2(0,2) + s_a*Temp2(0,3))*den;
					B[j + 1] -= (c_a*Temp2(1,0) + s_a*Temp2(1,1) + c_a*Temp2(1,2) + s_a*Temp2(1,3))*den;
					B[j + 2] -= (c_a*Temp2(2,0) + s_a*Temp2(2,1) + c_a*Temp2(2,2) + s_a*Temp2(2,3))*den;
					B[j + 3] -= (c_a*Temp2(3,0) + s_a*Temp2(3,1) + c_a*Temp2(3,2) + s_a*Temp2(3,3))*den;
					//std::cout << "|" <<std::endl;
					
				} else if(!constrain0 && constrain1) {
					double den = 1.0/sum_lamba[face_data[f].vert[v1]];

					B[i + 0] -= (c_a*Temp2(0,0) + s_a*Temp2(1,0) + c_a*Temp2(2,0) + s_a*Temp2(3,0))*den;
					B[i + 1] -= (c_a*Temp2(0,1) + s_a*Temp2(1,1) + c_a*Temp2(2,1) + s_a*Temp2(3,1))*den;
					B[i + 2] -= (c_a*Temp2(0,2) + s_a*Temp2(1,2) + c_a*Temp2(2,2) + s_a*Temp2(3,2))*den;
					B[i + 3] -= (c_a*Temp2(0,3) + s_a*Temp2(1,3) + c_a*Temp2(2,3) + s_a*Temp2(3,3))*den;

				} else if(!constrain0 && !constrain1) {
					//std::cout << "- | " << i << " " << j << std::endl;
					A(j + 0, i + 0) += Temp2(0, 0);
					A(j + 0, i + 1) += Temp2(0, 1);
					A(j + 0, i + 2) += Temp2(0, 2);
					A(j + 0, i + 3) += Temp2(0, 3);

					A(j + 1, i + 0) += Temp2(1, 0);
					A(j + 1, i + 1) += Temp2(1, 1);
					A(j + 1, i + 2) += Temp2(1, 2);
					A(j + 1, i + 3) += Temp2(1, 3);

					A(j + 2, i + 0) += Temp2(2, 0);
					A(j + 2, i + 1) += Temp2(2, 1);
					A(j + 2, i + 2) += Temp2(2, 2);
					A(j + 2, i + 3) += Temp2(2, 3);

					A(j + 3, i + 0) += Temp2(3, 0);
					A(j + 3, i + 1) += Temp2(3, 1);
					A(j + 3, i + 2) += Temp2(3, 2);
					A(j + 3, i + 3) += Temp2(3, 3);

					///// 

					A(i + 0, j + 0) += Temp2(0, 0);
					A(i + 0, j + 1) += Temp2(1, 0);
					A(i + 0, j + 2) += Temp2(2, 0);
					A(i + 0, j + 3) += Temp2(3, 0);

					A(i + 1, j + 0) += Temp2(0, 1);
					A(i + 1, j + 1) += Temp2(1, 1);
					A(i + 1, j + 2) += Temp2(2, 1);
					A(i + 1, j + 3) += Temp2(3, 1);

					A(i + 2, j + 0) += Temp2(0, 2);
					A(i + 2, j + 1) += Temp2(1, 2);
					A(i + 2, j + 2) += Temp2(2, 2);
					A(i + 2, j + 3) += Temp2(3, 2);

					A(i + 3, j + 0) += Temp2(0, 3);
					A(i + 3, j + 1) += Temp2(1, 3);
					A(i + 3, j + 2) += Temp2(2, 3);
					A(i + 3, j + 3) += Temp2(3, 3);
					//std::cout << "-" << std::endl;
				}
			}
		}

		gmm::clean(A, 1E-10);
		gmm::csr_matrix<double> S(4*curr, 4*curr);
		gmm::copy(A, S);
		
		gmm::diagonal_precond<gmm::csr_matrix<double> > PR(S);
		gmm::iteration iter(1E-10);
		iter.set_noisy(0);
		gmm::cg(S, X, B, PR, iter);
		
		for(vert_t v = 0; v < mapping.size(); ++v) {
			if(mapping[v] < 0) {
				vert_data[v].theta = k3d::pi()/180.0;
				vert_data[v].phi   = k3d::pi()/180.0;
				vert_data[v].U.first  = c_a;
				vert_data[v].U.second = s_a;
				vert_data[v].V.first  = c_a;
				vert_data[v].V.second = s_a;

			} else {
				vert_t vm = 4*mapping[v];
				vert_data[v].theta = atan2(X[vm + 1], X[vm + 0]);
				vert_data[v].phi   = atan2(X[vm + 3], X[vm + 2]);
				if(vert_data[v].theta < 0) vert_data[v].theta += k3d::pi_times_2();
				if(vert_data[v].phi < 0) vert_data[v].phi += k3d::pi_times_2();
				vert_data[v].U.first  = X[vm + 0];
				vert_data[v].U.second = X[vm + 1];
				vert_data[v].V.first  = X[vm + 2];
				vert_data[v].V.second = X[vm + 3];
			}

		}
		

	}


	void PGP::optimize(double omega) {
		std::vector<double> UV(4);
		std::vector<double> M_UV(4);
		for(face_t f = 0; f < mesh->num_faces; ++f) {
			per_face &pf = face_data[f];

			pf.theta[0] = vert_data[pf.vert[0]].theta;
			pf.theta[1] = vert_data[pf.vert[1]].theta;
			pf.theta[2] = vert_data[pf.vert[2]].theta;

			pf.phi[0] = vert_data[pf.vert[0]].phi;
			pf.phi[1] = vert_data[pf.vert[1]].phi;
			pf.phi[2] = vert_data[pf.vert[2]].phi;

			for(int v = 0; v < 2; v++) {
				// propagate from v to (v+1)%3 over edge (v+2)%3
				int i = v%3;
				int j = (v+1)%3;
				int e = (v+2)%3;

				int r,s,t;

				vec2 n = pf.e[e];
				n = n*(1.0/sqrt(n*n));

				double max_dot = pf.vf[i]*pf.vf[j]; 
				r = 0;

				for(int k = 1; k < 4; k++) {
					double dot = pf.vf[i]*rotate90(pf.vf[j], k);

					if(dot > max_dot) {
						max_dot = dot;
						r = k;
					}
				}

				pf.vf[j] = rotate90(pf.vf[j], r);
				
				vec2 param = vec2(pf.theta[j], pf.phi[j]);
				param = rotate90(param, r);
				pf.theta[j] = param.first;
				pf.phi[j] = param.second;

				s = (int)floor(-1*(pf.theta[i] - (k3d::pi()/omega)*(n*(pf.vf[i] + pf.vf[j])) - pf.theta[j])/k3d::pi_times_2() + 0.5);
				t = (int)floor(-1*(pf.phi[i] - (k3d::pi()/omega)*(n*(rotate90(pf.vf[i], 1) + rotate90(pf.vf[j], 1))) - pf.phi[j])/k3d::pi_times_2() + 0.5);
				pf.theta[j] -= k3d::pi_times_2()*s;
				pf.phi[j] -= k3d::pi_times_2()*t;
			}
		}
	}
	
	void PGP::validate(std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes) {
		for(edge_t e = 0; e < edges.size(); e++) {
			return_if_fail(edges[e].vert != 0xffffffff);			
			return_if_fail(edges[e].face != 0xffffffff);			
			return_if_fail(edges[e].next != 0xffffffff);
			return_if_fail(edges[e].comp  >= -1);
			return_if_fail(edges[e].index >= 0);
		}
		for(face_t f = 0; f < faces.size(); f++) {
			return_if_fail(faces[f].edge != 0xffffffff);
			return_if_fail(faces[f].index >= 0);
			edge_t e = faces[f].edge;
			int c = 0;
			do {
				c++;
				e = edges[e].next;
				return_if_fail(c <= edges.size());
			} while(e != faces[f].edge);
		}
		for(vert_t v = 0; v < verts.size(); v++) {
			return_if_fail(verts[v].edge != 0xffffffff);
			return_if_fail(verts[v].index >= 0);
		}
	}

	void PGP::print(std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes) {
		for(edge_t e = 0; e < edges.size(); e++) {
			std::cout << "e " << e << '(' << edges[e].index << ", " << edges[e].next << ", " << edges[e].comp << ", " << edges[e].face << ", " << edges[e].vert <<')' << std::endl;
		}
		for(face_t f = 0; f < faces.size(); f++) {
			std::cout << "f " << f << '(' << faces[f].index << ", " << faces[f].edge << ')' << std::endl;
		}
		for(vert_t v = 0; v < verts.size(); v++) {
			std::cout << "v " << v << '(' << verts[v].index << ", " << verts[v].edge << ')' << std::endl;
		}
		for(edge_t e = 0; e < fakes.size(); e++) {
			std::cout << "fake " << e << '(' << fakes[e].index << ", " << fakes[e].next << ", " << fakes[e].comp << ", " << fakes[e].edge << ')' << std::endl;		
		}
	}

	int PGP::split(edge_t edge_start, int i, double X, std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes){
		new_face2 nf(faces.size(), 0);
		edge_t edge = edge_start;
		new_face2& f = faces[edges[edge].face];
		bool found = false;
		edge_t edge_stop = edge_start;
		edge_t ne_1 = edges.size();
		edge_t ne_2 = edges.size()+1;
		int ret = -1;
//		std::cout << std::endl << std::endl << i << " - " << X << std::endl;
		//do {
		//	edges[edge].operator <<(std::cout);
		//	edge = edges[edge].next;
		//} while( edge != edge_start );		
		edge = edges[edge].next;

		do {
			new_edge2& e = edges[edge];
			new_edge2& en = edges[e.next];
			if(!found) {
//				std::cout << ' ' << edge << ' ' <<std::flush;
				
				double x0 = get(e.param, i);
				double x1 = get(en.param, i);
				if(x0 == X) { // Unlikely case, but should check for it
					//std::cout << "!!! ";
					return -1;
				}
				else if((x0 < X && X < x1) || (x1 < X && X < x0) ) {
					found = true;
					double alpha = (X - x0)/(x1 - x0);
					new_vert2 _v(verts.size(), ne_1);
					
					vec2 local = e.local + alpha*(en.local - e.local);
					vec2 param = e.param + alpha*(en.param - e.param);
					get(param, i) = X;

					_v.world = verts[en.vert].world;
					_v.world -= verts[e.vert].world;
					_v.world *= alpha;
					_v.world += verts[e.vert].world;

					verts.push_back(_v);

					if(e.comp >= 0) {
//						std::cout << 'a' <<std::flush;
						new_edge2& ec = edges[e.comp];
						new_edge2 _e(ne_2+1), _en(ne_1), _enc(ne_2), _ec(ne_2+2);

						_e.next = e.next;
						_e.comp = ec.index;
						_e.face = nf.index;
						_e.vert = _v.index;
						_e.local = local;
						_e.param = param;
						nf.edge = _e.index;

						e.next = _en.index;
						e.comp = _ec.index;
						e.face = f.index;
						f.edge = e.index;
						
						_ec.next = ec.next;
						_ec.comp = e.index;
						_ec.face = ec.face;
						_ec.vert = _v.index;
						_ec.local = local;
						_ec.param = param;
						
						ret = _ec.index;

						ec.next = _ec.index;
						ec.comp = _e.index;

						_en.next = edge_start;
						_en.comp = _enc.index;
						_en.face = f.index;
						_en.vert = _v.index;
						_en.local = local;
						_en.param = param;

						_enc.comp = _en.index;
						_enc.next = _e.index;
						_enc.vert = edges[edge_start].vert;
						_enc.face = nf.index;
						_enc.param = edges[edge_start].param;
						_enc.local = edges[edge_start].local;
						
						//std::cout << "\ne"; e.operator <<(std::cout);
						//std::cout << "en"; en.operator <<(std::cout);
						//std::cout << "_e"; _e.operator <<(std::cout);
						//std::cout << "_en"; _en.operator <<(std::cout);
						//std::cout << "_enc"; _enc.operator <<(std::cout);
						//std::cout << "_ec"; _ec.operator <<(std::cout);


						edges.push_back(_en);
						edges.push_back(_enc);
						edges.push_back(_e);
						edges.push_back(_ec);
						
						//std::cout << "---------" << std::endl;
						//edge_t e1 =_en.index;
						//do {
						//	edges[e1].operator <<(std::cout);
						//	e1 = edges[e1].next;
						//} while( e1 != _en.index );		
						//std::cout << "---------" << std::endl;
						//e1 =_enc.index;
						//do {
						//	edges[e1].operator <<(std::cout);
						//	e1 = edges[e1].next;
						//} while( e1 != _enc.index );		
						//std::cout << "---------" << std::endl;

						edge = _e.index;

					} else {
//						std::cout << 'b' <<std::flush;
						fake_edge& fe = fakes[e.fake];
						new_edge2 _en(ne_1), _enc(ne_2), _e(ne_2+1);
						fake_edge _fe(fakes.size(), fe.next, _e.index, fe.edge);
						fe.next = _fe.index;
						_e.next = e.next;
						_e.comp = -1;
						_e.fake = _fe.index;
						_e.face = nf.index;
						_e.vert = _v.index;
						_e.local = local;
						_e.param = param;
						nf.edge = _e.index;

						e.next = _en.index;
						e.face = f.index;
						f.edge = e.index;

						_en.next = edge_start;
						_en.comp = _enc.index;
						_en.face = f.index;
						_en.vert = _v.index;
						_en.local = local;
						_en.param = param;

						_enc.comp = _en.index;
						_enc.next = _e.index;
						_enc.vert = edges[edge_start].vert;
						_enc.face = nf.index;
						_enc.param = edges[edge_start].param;
						_enc.local = edges[edge_start].local;
						
						//std::cout << "\ne"; e.operator <<(std::cout);
						//std::cout << "en"; en.operator <<(std::cout);
						//std::cout << "_e"; _e.operator <<(std::cout);
						//std::cout << "_en"; _en.operator <<(std::cout);
						//std::cout << "_enc"; _enc.operator <<(std::cout);

						fakes.push_back(_fe);
						edges.push_back(_en);
						edges.push_back(_enc);
						edges.push_back(_e);

/*						std::cout << "---------" << std::endl;
						edge_t e1 =_en.index;
						do {
							edges[e1].operator <<(std::cout);
							e1 = edges[e1].next;
						} while( e1 != _en.index );		
						std::cout << "---------" << std::endl;
						e1 =_enc.index;
						do {
							edges[e1].operator <<(std::cout);
							e1 = edges[e1].next;
						} while( e1 != _enc.index );		
						std::cout << "---------" << std::endl;
	*/					
						edge = _e.index;
					}
				} else {
//					std::cout << "\\" << std::flush;
					e.face = f.index;
					f.edge = e.index;
					edge = edges[edge].next;
				}
				

			} else {
//				std::cout << '-' <<std::flush;

				e.face = nf.index;
				nf.edge = e.index;
				if(en.index == edge_start) {
					e.next = ne_2;
					break;
				}
				edge = en.index;				

			}
		} while(edge != edge_start);

		if(found) {
			face_t f_i = f.index;
			faces.push_back(nf);
			//std::cout << "new f: " << nf.index << " " << nf.edge << std::endl;
			//edge = nf.edge;
			//do {
			//	edges[edge].operator <<(std::cout);
			//	edge = edges[edge].next;
			//} while( edge != nf.edge );		

			//std::cout << "_old f: " << &f << " " << faces[f_i].index << " " << faces[f_i].edge << std::endl;
			//edge = faces[f_i].edge;
			//do {
			//	edges[edge].operator <<(std::cout);
			//	edge = edges[edge].next;
			//} while( edge != faces[f_i].edge );		
		}

		return ret;
	}

	void PGP::split_tri(face_t f, int i, double X, std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes){
		edge_t fe = mesh->face_edge[f];
		do {
			new_edge2& e = edges[fakes[fe].comp];
			new_edge2& en = edges[fakes[fakes[fe].next].comp];
			double x0 = get(e.param, i);
			double x1 = get(en.param, i);
			if(x0 == X) { // Unlikely case, but shoud check for it
				//std::cout << "!!! ";
					return;
			}
			else if((x0 < X && X < x1) || (x1 < X && X < x0)) {
				double alpha = (X - x0)/(x1 - x0);

				fake_edge nfe(fakes.size());
				new_vert2 nv(verts.size());
				new_edge2 ne(edges.size(), e.next, -1, e.face, nv.index, nfe.index);

				e.next = ne.index;
				nv.edge = ne.index;
				
				nfe.comp = ne.index;
				nfe.next = fakes[fe].next;
				nfe.edge = fakes[fe].edge;
				fakes[fe].next = nfe.index;

				ne.local = e.local + alpha*(en.local - e.local);
				ne.param = e.param + alpha*(en.param - e.param);
				get(ne.param, i) = X;

				nv.world = verts[en.vert].world;
				nv.world -= verts[e.vert].world;
				nv.world *= alpha;
				nv.world += verts[e.vert].world;

				fakes.push_back(nfe);
				verts.push_back(nv);
				edges.push_back(ne);

				int s = ne.index;
				while(s >= 0) {
					s = split(s, i, X, edges, faces, verts, fakes);
				}

				return;
			}
			fe = fakes[fe].next;
		
		} while( fe != mesh->face_edge[f] );
		
		return;
	}
	void PGP::clean_edges(std::vector<new_edge2>& edges) {
		edge_t e_size = 0;
		for(edge_t e = 0; e < edges.size(); ++e) {
			if(edges[e].index >= 0) {
				edges[e].index = e_size++;
			}
		}

		for(edge_t e = 0; e < edges.size(); ++e) {
			if(edges[e].index >= 0) {
				edges[e].next = edges[edges[e].next].index;
				if(edges[e].comp >= 0 ) edges[e].comp = edges[edges[e].comp].index;

			}
		}

		for(edge_t e = 0; e < edges.size(); ++e) {
			if(edges[e].index >= 0) {
				edges[edges[e].index] = edges[e];
			}
		}
		edges.resize(e_size);
	}

	void PGP::clean_verts(std::vector<new_edge2>& edges, std::vector<new_vert2>& verts) {
		for(edge_t e = 0; e < edges.size(); ++e) {
			if(verts[edges[e].vert].index < 0) {
				verts[edges[e].vert].index = edges[e].vert;
			}
			edges[e].vert = verts[edges[e].vert].index;
			
		}
		vert_t v_size=0;

		for(vert_t v = 0; v < verts.size(); ++v) {
			if(v != verts[v].index || v < mesh->num_verts) {
				verts[v].index = -1;
			} else {
				verts[v].index = v_size++;
			}
		}

		for(edge_t e = 0; e < edges.size(); ++e) {
			edges[e].vert = verts[edges[e].vert].index;
		}

		for(vert_t v = 0; v < verts.size(); ++v) {
			if(verts[v].index >= 0)
				verts[verts[v].index] = verts[v];
		}

		verts.resize(v_size);
	}

	void PGP::zip(edge_t edge, std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes)
	{
		std::vector<edge_t> stack;
		edge_t fe = edge;
		while(fe != mesh->edge_ccw[edge]) {
			stack.push_back(fe);
			fe = fakes[fe].next;
		}

		fe = mesh->edge_comp[edge];
		while(fe != mesh->edge_ccw[ mesh->edge_comp[edge]]) {
			fe = fakes[fe].next;
		}

		fe = mesh->edge_comp[edge];
		while(fe != mesh->edge_ccw[ mesh->edge_comp[edge]]) {
			edge_t fe_c = stack.back();
			stack.pop_back();

			edge_t e = fakes[fe].comp;
			edge_t ec = fakes[fe_c].comp;

			edges[e].comp = edges[ec].index; // ec
			edges[ec].comp = edges[e].index; // e

			vert_t v;
			if(edges[edges[e].next].vert != edges[ec].vert) {
				v = std::min(edges[ec].vert, edges[edges[e].next].vert);
				verts[edges[edges[e].next].vert].index = verts[edges[ec].vert].index = v;
			}
			if(edges[edges[ec].next].vert != edges[e].vert) {
				v = std::min(edges[e].vert, edges[edges[ec].next].vert);
				verts[edges[edges[ec].next].vert].index = verts[edges[e].vert].index = v;
			}

			fe = fakes[fe].next;
		}
	}
	void PGP::unzip(edge_t e, std::vector<new_edge2>& edges, std::vector<new_face2>& faces, std::vector<new_vert2>& verts, std::vector<fake_edge>& fakes)
	{
		edge_t fe;
		fe = e;

		mesh_info::Edge mesh_edge = mesh->getEdge(e);
		Vec3 start = mesh_edge.start();
		Vec3 end = mesh_edge.end();
		
		std::vector<edge_t> stack;
		fe = e;

		while(fe != mesh->edge_ccw[e]) {
			stack.push_back(fe);
			fe = fakes[fe].next;
		}

		fe = mesh->edge_comp[e];
		while(fe != mesh->edge_ccw[ mesh->edge_comp[e]]) {


			edge_t p_edge = fakes[stack.back()].comp;
			Vec3 p = verts[edges[p_edge].vert].world;
			p -= end;
			double p_len = p.Length();
			
			edge_t q_edge = fakes[fe].comp;
			Vec3 q = verts[edges[fakes[fakes[fe].next].comp].vert].world;
			q -= end;
			double q_len = q.Length();
			if(fakes[fe].next == mesh->edge_ccw[ mesh->edge_comp[e]] && p_edge == e) {
				edges[p_edge].comp = edges[q_edge].index; // ec
				edges[q_edge].comp = edges[p_edge].index; // e
				break;
			} 
			if(q_len < p_len) {
				new_edge2 p_new(edges.size(), 
								edges[p_edge].next, 
								q_edge, 
								edges[p_edge].face, 
								edges[fakes[fakes[fe].next].comp].vert, 
								fakes.size());

				fake_edge fe_new(p_new.fake, fakes[stack.back()].next, p_new.index, fakes[stack.back()].edge);

				edges[q_edge].comp = p_new.index;
				edges[p_edge].next = p_new.index;
				fakes[stack.back()].next = fe_new.index;

				edges.push_back(p_new);
				fakes.push_back(fe_new);

				fe = fakes[fe].next;
			} else if(p_len < q_len) {
				new_edge2 q_new(edges.size(),
								edges[q_edge].next,
								-1,
								edges[q_edge].face,
								edges[p_edge].vert,
								fakes.size());

				fake_edge fe_new(q_new.fake, fakes[fe].next, q_new.index, fakes[fe].edge);

				edges[p_edge].comp = q_edge;
				edges[q_edge].comp = p_edge;
				edges[q_edge].next = q_new.index;
				fakes[fe].next = fe_new.index;

				edges.push_back(q_new);
				fakes.push_back(fe_new);
				
				fe = q_new.fake;
				stack.pop_back();
			} else {
				stack.pop_back();
				fe = fakes[fe].next;
				//std::cout << "!!!!!" << std::endl;
				edges[p_edge].comp = edges[q_edge].index; // ec
				edges[q_edge].comp = edges[p_edge].index; // e

				vert_t v;
				if(edges[edges[p_edge].next].vert != edges[q_edge].vert) {
					v = std::min(edges[q_edge].vert, edges[edges[p_edge].next].vert);
					verts[edges[edges[p_edge].next].vert].index = verts[edges[q_edge].vert].index = v;
				}
				if(edges[edges[q_edge].next].vert != edges[p_edge].vert) {
					v = std::min(edges[p_edge].vert, edges[edges[q_edge].next].vert);
					verts[edges[edges[q_edge].next].vert].index = verts[edges[p_edge].vert].index = v;
				}
			}
		}
		

	}

	void PGP::extract2(int divisions, k3d::mesh& OutputMesh) 
	{
		std::vector<new_edge2> edges(mesh->num_edges);
		std::vector<fake_edge> fakes(mesh->num_edges);
		std::vector<new_face2> faces(mesh->num_faces);
		std::vector<new_vert2> verts(mesh->num_verts);

		for(edge_t e = 0; e < mesh->num_edges; ++e) {
			edges[e] = new_edge2(e, mesh->edge_ccw[e], -1, mesh->edge_face[e], mesh->edge_vert[e], e);
			fakes[e] = fake_edge(e, mesh->edge_ccw[e], e, e);
		}

		for(face_t f = 0; f < mesh->num_faces; ++f) {
			faces[f] = new_face2(f, mesh->face_edge[f]);
			edges[face_data[f].edge[0]].param = vec2(face_data[f].theta[1], face_data[f].phi[1]);
			edges[face_data[f].edge[1]].param = vec2(face_data[f].theta[2], face_data[f].phi[2]);
			edges[face_data[f].edge[2]].param = vec2(face_data[f].theta[0], face_data[f].phi[0]);
			edges[face_data[f].edge[0]].local = face_data[f].v[1];
			edges[face_data[f].edge[1]].local = face_data[f].v[2];
			edges[face_data[f].edge[2]].local = face_data[f].v[0];
		}

		for(vert_t v = 0; v < mesh->num_verts; ++v) {
			verts[v] = new_vert2(v, mesh->vert_edge[v]);
			verts[v].world = mesh->getVert(v).pos();
		}

		double trans = k3d::pi_times_2()/(double)divisions;

		for(face_t f = 0; f < mesh->num_faces; ++f) {
			per_face &pf = face_data[f];
			double max_t = std::max(pf.theta[0], std::max(pf.theta[1], pf.theta[2]));
			double max_p = std::max(pf.phi[0],   std::max(pf.phi[1], pf.phi[2]));
			double min_t = std::min(pf.theta[0], std::min(pf.theta[1], pf.theta[2]));
			double min_p = std::min(pf.phi[0],   std::min(pf.phi[1], pf.phi[2]));
			int s0 = (int)(min_t/trans);// - 1;
			int s1 = (int)(max_t/trans);// + 1;
			int t0 = (int)(min_p/trans);// - 1;
			int t1 = (int)(max_p/trans);// + 1;

			for(int s = s0; s <= s1; ++s) {
				split_tri(f, 0, trans*((double)s), edges, faces, verts, fakes);
			}
			for(int t = t0; t <= t1; ++t) {
				split_tri(f, 1, trans*((double)t), edges, faces, verts, fakes);
			}
		}

		for(edge_t e = 0; e < mesh->num_edges; ++e) {
			
			if(e < mesh->edge_comp[e]) {
				vec2 a = edges[e].param-edges[mesh->edge_ccw[e]].param;
				edge_t ec = mesh->edge_comp[e];
				vec2 b = edges[ec].param-edges[mesh->edge_ccw[ec]].param;
				
				if(std::abs(a*a - b*b) > 1.0e-8) {
					unzip(e, edges, faces, verts, fakes);
				} else {
					int c1 = 0, c2 = 0;
					edge_t fe = e;
					while(fakes[fe].edge == fakes[e].edge) {
						c1++;
						fe = fakes[fe].next;
					}
					fe = mesh->edge_comp[e];
					while(fakes[fe].edge == fakes[mesh->edge_comp[e]].edge) {
						c2++;
						fe = fakes[fe].next;
					}
					if(c1 == c2 && c1 == 1 ) {
						edges[e].comp = mesh->edge_comp[e];
						edges[mesh->edge_comp[e]].comp = e;
					} else if(c1 == c2) {
						zip(e, edges, faces, verts, fakes);
					} else {
						unzip(e, edges, faces, verts, fakes);
					}
				}
			}
		}

		std::vector<edge_t> prev(edges.size());
		for(edge_t e = 0; e < edges.size(); e++) {
			prev[edges[e].next] = e;
		}

		for(edge_t fe = 0; fe < fakes.size(); ++fe) {
			int e = fakes[fe].comp;
			
			int ec = edges[e].comp;

			if(e < ec) {
				assert_error(edges[prev[e]].next == e);
				assert_error(edges[prev[ec]].next == ec);

				edges[prev[e]].next = edges[ec].next;
				edges[prev[ec]].next = edges[e].next;

				prev[edges[ec].next] = prev[e];
				prev[edges[e].next] = prev[ec];
				
				edges[e].index = -1;
				edges[ec].index = -1;
			}
		}

		clean_edges(edges);
		

		for(edge_t e = 0; e < edges.size(); e++) {
			prev[edges[e].next] = e;
		}

		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].comp < 0 || edges[edges[e].next].comp < 0) {
				continue;
			}
			if(edges[e].comp == edges[edges[edges[e].next].comp].next && edges[e].comp != edges[e].next) {
				edges[edges[e].next].index = -4;
				verts[edges[edges[e].next].vert].index = -10;
				edges[edges[e].comp].index = -5;
			}
		}
		
		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].index >= 0 && edges[e].comp >= 0) {
				edge_t en = edges[e].next;
				edge_t ec = edges[e].comp;
				while(edges[en].index < 0) {
					ec = edges[en].comp;
					en = edges[en].next;
				}

				edges[e].next = en;
				edges[e].comp = ec;
			}
		}
		
		clean_edges(edges);
		for(edge_t e = 0; e < edges.size(); e++) {
			prev[edges[e].next] = e;
		}
		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].index < 0) 
				continue;

			edge_t en = edges[e].next;
			
			if(edges[e].index >= 0 && edges[en].index >= 0 && edges[en].next == e) {
				edges[e].index = -1;
				edges[en].index = -1;
				edges[edges[e].comp].comp = edges[en].comp;
				edges[edges[en].comp].comp = edges[e].comp;
			}
		}
		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].index < 0)
				continue;
			edge_t en = edges[e].next;
			edge_t enn = edges[en].next;
			edge_t ennn = edges[enn].next;
	
			if(edges[en].index >= 0 && edges[en].vert == edges[ennn].vert) {
				edges[e].next = ennn;
				verts[edges[enn].vert].index = -1;
				edges[en].index = -1;
				edges[enn].index = -1;
			}
		}
		clean_edges(edges);
		for(edge_t e = 0; e < edges.size(); e++) {
			prev[edges[e].next] = e;
		}

		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].comp < 0 || edges[edges[e].next].comp < 0) {
				continue;
			}
			if(edges[e].comp == edges[edges[edges[e].next].comp].next && edges[e].comp != edges[e].next) {
				edges[edges[e].next].index = -4;
				verts[edges[edges[e].next].vert].index = -10;
				edges[edges[e].comp].index = -5;
			}
		}
		
		for(edge_t e = 0; e < edges.size(); e++) {
			if(edges[e].index >= 0 && edges[e].comp >= 0) {
				edge_t en = edges[e].next;
				edge_t ec = edges[e].comp;
				while(edges[en].index < 0) {
					ec = edges[en].comp;
					en = edges[en].next;
				}

				edges[e].next = en;
				edges[e].comp = ec;
			}
		}
		
		clean_edges(edges);
	
		clean_verts(edges, verts);
		for(edge_t e = 0; e < edges.size(); ++e) {
			edges[e].face = -1;
		}

		faces.clear();
		for(edge_t e = 0; e < edges.size(); ++e) {
			//bool p = false;
			int c = 0;
			edge_t temp = e;
			if(edges[e].face < 0) {	
				faces.push_back(new_face2(faces.size()));
				faces.back().index = faces.size()-1;

				
				while(edges[temp].face < 0) {
					edges[temp].face = faces.back().index;
					faces.back().edge = temp;
					temp = edges[temp].next;
					c++;
					if(edges[temp].comp == edges[temp].next) std::cout << '!';
					//p = p || edges[temp].comp < 0;
				} 
				//p = p || temp != e;// || c < 3 || c > 4;
			}
			//if(p) {
			//	do {
			//		edges[e].operator <<(std::cout);
			//		e = edges[e].next;
			//	} while(e != temp);				
			//	std::cout << std::endl <<  std::endl;
			//}
		}

		k3d::mesh::polyhedra_t* poly = new k3d::mesh::polyhedra_t();
		k3d::mesh::selection_t* points_sel = new k3d::mesh::selection_t();
		k3d::mesh::points_t* points = new k3d::mesh::points_t();
		k3d::mesh::indices_t* clockwise_edges = new k3d::mesh::indices_t();
		k3d::mesh::indices_t* edge_points = new k3d::mesh::indices_t();
		k3d::mesh::selection_t* edge_selection = new k3d::mesh::selection_t();
		k3d::mesh::counts_t* face_counts = new k3d::mesh::counts_t();
		k3d::mesh::indices_t* face_first_loops = new k3d::mesh::indices_t();
		k3d::mesh::counts_t* face_loop_counts = new k3d::mesh::counts_t();
		k3d::mesh::materials_t* face_materials = new k3d::mesh::materials_t();
		k3d::mesh::selection_t* face_selection = new k3d::mesh::selection_t();
		k3d::mesh::indices_t* first_faces = new k3d::mesh::indices_t();
		k3d::mesh::indices_t* loop_first_edges = new k3d::mesh::indices_t();
		k3d::mesh::polyhedra_t::types_t* types = new k3d::mesh::polyhedra_t::types_t();
		
		points->resize(verts.size());
		points_sel->resize(verts.size(), 0.0);

		for(size_t i = 0; i < points->size(); ++i) {
			points->at(i)[0] = verts[i].world[0];
			points->at(i)[1] = verts[i].world[1];
			points->at(i)[2] = verts[i].world[2];
		}

		edge_selection->resize(edges.size(), 0.0);

		face_counts->push_back(faces.size());
		face_first_loops->resize(faces.size());
		face_loop_counts->resize(faces.size(), 1);
		face_materials->resize(faces.size(), 0);
		face_selection->resize(faces.size(), 0.0);
		first_faces->push_back(0);
		loop_first_edges->resize(edges.size());

		types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

		edge_points->resize(edges.size());
		for(size_t i = 0; i < edge_points->size(); ++i) {
			edge_points->at(i) = edges[edges[i].next].vert;
			//edge_points->at(i) = edges[i].vert;
			
			////////////////
			face_first_loops->at(edges[i].face) = i;
			loop_first_edges->at(i) = i;
		}

		clockwise_edges->resize(edges.size());
		for(size_t i = 0; i < clockwise_edges->size(); ++i) {
			clockwise_edges->at(edges[i].next) = i;
			//clockwise_edges->at(i) = edges[i].next;
		}

		
		OutputMesh.points = boost::shared_ptr<k3d::mesh::points_t>(points);
		OutputMesh.polyhedra = boost::shared_ptr<k3d::mesh::polyhedra_t>(poly);
		OutputMesh.point_selection = boost::shared_ptr<k3d::mesh::selection_t>(points_sel);

		poly->clockwise_edges = boost::shared_ptr<k3d::mesh::indices_t>(clockwise_edges);
		poly->edge_points = boost::shared_ptr<k3d::mesh::indices_t>(edge_points);
		poly->edge_selection = boost::shared_ptr<k3d::mesh::selection_t>(edge_selection);
		poly->face_counts = boost::shared_ptr<k3d::mesh::counts_t>(face_counts);
		poly->face_first_loops = boost::shared_ptr<k3d::mesh::indices_t>(face_first_loops);
		poly->face_loop_counts = boost::shared_ptr<k3d::mesh::counts_t>(face_loop_counts);
		poly->face_materials = boost::shared_ptr<k3d::mesh::materials_t>(face_materials);
		poly->face_selection = boost::shared_ptr<k3d::mesh::selection_t>(face_selection);
		poly->first_faces = boost::shared_ptr<k3d::mesh::indices_t>(first_faces);
		poly->loop_first_edges = boost::shared_ptr<k3d::mesh::indices_t>(loop_first_edges);
		poly->types = boost::shared_ptr<k3d::mesh::polyhedra_t::types_t>(types);
	}

};

}; // namespace pgp_module
