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
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>
#include <iostream>
#include <map>
#include "mesh_info.h"
#include "diff_geom.h"
#include "pgp.h"

namespace libk3dquadremesh
{
namespace detail
{

	struct do_triangulate
	{
		bool operator()(k3d::legacy::face* Face)
		{
			k3d::legacy::split_edge* edge = Face->first_edge;

			int edges = 0;
			do
			{
				++edges;
				edge = edge->face_clockwise;
			}
			while(edge != Face->first_edge);

			return edges > 4;
		}
	};

} // namespace detail

	class pgp_remesh :
		public k3d::mesh_modifier<k3d::persistent<k3d::node> > 
	{
		typedef  k3d::mesh_modifier<k3d::persistent<k3d::node> >  base_t;
	public:
		pgp_remesh(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
			base_t(Factory, Document),
			m_smooth(init_owner(*this) + init_name("use_smooth") + init_label(_("Smooth Curvature")) + init_description(_("Smooth Curvature")) + init_value(true)),
			//m_symmetry(init_owner(*this) + init_name("smooth_4") + init_label(_("Smooth as 4-symmetry")) + init_description(_("Smooth as 4-symmetry")) + init_value(false)),
			m_steps(init_owner(*this) + init_name("steps") + init_label(_("Smoothing steps")) + init_description(_("Smoothing steps")) + init_value(3) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
			m_h(init_owner(*this) + init_name("h") + init_label(_("Smoothing timestep")) + init_description(_("Smoothing timesteps")) + init_value(1500.0) + init_constraint(constraint::minimum<double>(0.0001))),
			m_omega(init_owner(*this) + init_name("omega") + init_label(_("Omega parameter")) + init_description(_("Omega parameter")) + init_value(1.0) + init_constraint(constraint::minimum<double>(0.1))),
			m_divides(init_owner(*this) + init_name("div") + init_label(_("Iso line divisions")) + init_description(_("Iso line divisions")) + init_value(2) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
			m_triang(init_owner(*this) + init_name("triangulate") + init_label(_("Triangulate Non-Quads")) + init_description(_("Triangulates faces with more than 4 edges")) + init_value(true)),
			prev_steps(0),
			smoothed(false)
		{
			//m_smooth.changed_signal().connect(sigc::mem_fun(*this, &pgp_remesh::smooth));
			//m_steps.changed_signal().connect(sigc::mem_fun(*this, &pgp_remesh::smooth));
			//m_h.changed_signal().connect(sigc::mem_fun(*this, &pgp_remesh::smooth));
			m_smooth.changed_signal().connect(make_reset_mesh_slot());
			m_steps.changed_signal().connect(make_reset_mesh_slot());
			m_h.changed_signal().connect(make_reset_mesh_slot());
			//m_symmetry.changed_signal().connect(make_reset_mesh_slot());
			m_omega.changed_signal().connect(make_reset_mesh_slot());
			m_divides.changed_signal().connect(make_reset_mesh_slot());
			std::cout << "end \n";
		}

		~pgp_remesh()
		{

		}

		void smooth(k3d::iunknown* const Hint)
		{
		}	

		void on_create_mesh(const k3d::mesh& InputMesh, k3d::mesh& OutputMesh) 
		{
			//k3d::log() << debug << "PGP: create mesh: " << std::endl;
			//detail::mesh_info m(InputMesh); 
			//k3d::log() << debug << "PGP: create mesh: diff geom" << std::endl;
			//detail::diff_geom diff(m);
			OutputMesh = InputMesh;
			//k3d::typed_array<k3d::vector3>* curv_p = new k3d::typed_array<k3d::vector3>;
			//k3d::log() << debug << "PGP: create mesh: fill diff geom" << std::endl;
			//diff.fill_diff_geom(OutputMesh);
			//// Will do this more efficiently later
		}
		void on_update_mesh(const k3d::mesh& InputMesh, k3d::mesh& OutputMesh)		  
		{
			if(!k3d::is_solid(InputMesh))
			{
				OutputMesh = k3d::mesh();
				k3d::log() << error << name() << " requires a closed mesh (no holes)" << std::endl;
				return;
			}

			// If our input isn't triangles, bail-out ...
			if(!k3d::is_triangles(InputMesh))
			{
				OutputMesh = k3d::mesh();
				k3d::log() << error << name() << " requires triangles as input" << std::endl;
				return;
			}

			base_t::document().pipeline_profiler().start_execution(*this, "Process Mesh");
			info = detail::mesh_info(&InputMesh);
			base_t::document().pipeline_profiler().finish_execution(*this, "Process Mesh");

			base_t::document().pipeline_profiler().start_execution(*this, "Calc Diff Geom");
			geom = detail::diff_geom(&info);
			geom.initialize();
			base_t::document().pipeline_profiler().finish_execution(*this, "Calc Diff Geom");

			if(m_smooth.pipeline_value()) {
				base_t::document().pipeline_profiler().start_execution(*this, "Smooth");
				geom.smooth(m_h.pipeline_value(), m_steps.pipeline_value(), false); //m_symmetry.pipeline_value());
				base_t::document().pipeline_profiler().finish_execution(*this, "Smooth");
			}

			pgp = detail::PGP(&info, &geom);

			base_t::document().pipeline_profiler().start_execution(*this, "PGP Setup VF");
			pgp.setup_vf(true);
			base_t::document().pipeline_profiler().finish_execution(*this, "PGP Setup VF");
			
			//base_t::document().pipeline_profiler().start_execution(*this, "PGP Curl Correction");
			//pgp.curl_correction();
			//base_t::document().pipeline_profiler().finish_execution(*this, "PGP Curl Correction");


			base_t::document().pipeline_profiler().start_execution(*this, "PGP Setup");
			pgp.setup(m_omega.pipeline_value());
			base_t::document().pipeline_profiler().finish_execution(*this, "PGP Setup");

			base_t::document().pipeline_profiler().start_execution(*this, "PGP solve");
			pgp.solve();
			base_t::document().pipeline_profiler().finish_execution(*this, "PGP solve");

			base_t::document().pipeline_profiler().start_execution(*this, "PGP optimize");
			pgp.optimize(m_omega.pipeline_value());
			base_t::document().pipeline_profiler().finish_execution(*this, "PGP omptimize");

			base_t::document().pipeline_profiler().start_execution(*this, "PGP extract");
			pgp.extract2(m_divides.pipeline_value(), OutputMesh);
			base_t::document().pipeline_profiler().finish_execution(*this, "PGP extract");

			//base_t::document().pipeline_profiler().start_execution(*this, "PGP extract");
			//pgp.extract(m_divides.pipeline_value());
			//base_t::document().pipeline_profiler().finish_execution(*this, "PGP extract");
			//
			//base_t::document().pipeline_profiler().start_execution(*this, "PGP remesh");
			//pgp.remesh(OutputMesh);
			//base_t::document().pipeline_profiler().finish_execution(*this, "PGP remesh");

			// If our output isn't valid, bail-out ...
			if(!k3d::validate(OutputMesh))
			{
				OutputMesh = k3d::mesh();
				k3d::log() << error << "internal error: produced invalid mesh" << std::endl;
				return;
			}
			
			if(m_triang.pipeline_value()) {
				k3d::legacy::mesh LegMesh;
				LegMesh = OutputMesh;
				k3d::legacy::polyhedron& polyhedron = *LegMesh.polyhedra[0];

				// Triangulate faces that are not quads
				k3d::legacy::polyhedron::faces_t selected_faces;
				k3d::copy_if(polyhedron.faces.begin(), polyhedron.faces.end(), std::inserter(selected_faces, selected_faces.end()), detail::do_triangulate());

				// For each face ...
				for(k3d::legacy::polyhedron::faces_t::iterator face = selected_faces.begin(); face != selected_faces.end(); ++face)
				{
					k3d::legacy::polyhedron::faces_t faces;
					faces.push_back(*face);

					k3d::legacy::triangulate(faces, polyhedron.faces, LegMesh.points);

					polyhedron.faces.erase(std::remove(polyhedron.faces.begin(), polyhedron.faces.end(), *face), polyhedron.faces.end());
					delete *face;
				}

				// Set companions
				k3d::legacy::set_companions(polyhedron);
				OutputMesh = LegMesh;
			}
		}

		static k3d::iplugin_factory& get_factory()
		{
		static k3d::document_plugin_factory<pgp_remesh,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
			  k3d::uuid(0xc97aa4ce, 0x412c1ed2, 0x055044a4, 0xa151f085),
			  "PGPRemesh",
			  _("Quad remeshing using the PGP algorithm"),
			  "PGP",
			  k3d::iplugin_factory::EXPERIMENTAL);
			
			return factory;

		}
	private:
		k3d_data(bool,   immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_smooth;
		//k3d_data(bool,   immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_symmetry;
		k3d_data(k3d::int32_t,    immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_steps;
		k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_h;
		k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_omega;
		k3d_data(k3d::int32_t,    immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_divides;
		k3d_data(bool,   immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_triang;
		int prev_steps;
		bool smoothed;
		detail::mesh_info info;
		detail::diff_geom geom;
		detail::PGP pgp;
		k3d::mesh m;
	};


	k3d::iplugin_factory& pgp_remesh_factory()
	{
		return pgp_remesh::get_factory();
	}
} // namespace pgp_module
