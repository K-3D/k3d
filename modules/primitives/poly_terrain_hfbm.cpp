// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Creates an fBM-based fractal landscape
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/noise.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dprimitives
{

/// A fractal terrain algorithm that creates heterogeneous terrains
/// based on fBM (from Perlin, "Texturing and Modeling, A Procedural Approach")

double get_elevation(const unsigned long XIndex, const unsigned long YIndex, const unsigned long Size, const std::vector<double>& ExponentArray, const double Frequency, const double NoiseOffset, const double Offset, const double Lacunarity, const double Octaves)
{
	// Sanity checks ...
	assert_warning(XIndex < Size);
	assert_warning(YIndex < Size);

	// Convert grid coordinates to the range [0, 1], [0, 1] ...
	double x = double(XIndex) / double(Size-1);
	double y = double(YIndex) / double(Size-1);

	// First, unscaled octave ...
	k3d::point3 point(x * Frequency, y * Frequency, NoiseOffset);
	double value = Offset + k3d::noise(point);
	point *= Lacunarity;

	// Spectral construction inner loop ...
	for(int i = 1; i < Octaves; i++)
	{
		// Obtain displaced noise value ...
		double increment = Offset + k3d::noise(point);

		// Scale amplitude for this frequency ...
		increment *= ExponentArray[i];

		// Scale increment by current "altitude" ....
		increment *= value;

		// Add increment to value ...
		value += increment;

		// Raise spatial frequency ...
		point *= Lacunarity;
	}

	// Handle remainder in octaves ...
	double remainder = Octaves - int(Octaves);
	if(remainder)
	{
		double increment = Offset + k3d::noise(point) * ExponentArray.back();
		value += remainder * increment * value;
	}

	return value;
}

bool create_hfbm_triangle(k3d::legacy::polyhedron& Polyhedron, k3d::legacy::point* Point1, k3d::legacy::point* Point2, k3d::legacy::point* Point3, k3d::imaterial* const Material)
{
	assert_warning(Point1);
	assert_warning(Point2);
	assert_warning(Point3);

	k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(Point1);
	k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(Point2);
	k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(Point3);

	edge1->face_clockwise = edge3;
	edge3->face_clockwise = edge2;
	edge2->face_clockwise = edge1;

	k3d::legacy::face* const face = new k3d::legacy::face(edge1, Material);
	return_val_if_fail(face, false);
	Polyhedron.faces.push_back(face);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// poly_terrain_hfbm_implementation

class poly_terrain_hfbm_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_terrain_hfbm_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_iterations(init_owner(*this) + init_name("iterations") + init_label(_("Iterations")) + init_description(_("Iterations")) + init_value(4) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_fractal_dimension(init_owner(*this) + init_name("dimension") + init_label(_("Dimension")) + init_description(_("Fractal dimension")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_lacunarity(init_owner(*this) + init_name("lacunarity") + init_label(_("Lacunarity")) + init_description(_("Lacunarity")) + init_value(0.6) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_octaves(init_owner(*this) + init_name("octaves") + init_label(_("Octaves")) + init_description(_("Octaves")) + init_value(4.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offset")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_frequency(init_owner(*this) + init_name("frequency") + init_label(_("Frequency")) + init_description(_("Frequency")) + init_value(6.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_noise_offset(init_owner(*this) + init_name("noiseoffset") + init_label(_("Noise offset")) + init_description(_("Noise offset")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values()))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_iterations.changed_signal().connect(make_reset_mesh_slot());
		m_fractal_dimension.changed_signal().connect(make_reset_mesh_slot());
		m_lacunarity.changed_signal().connect(make_reset_mesh_slot());
		m_octaves.changed_signal().connect(make_reset_mesh_slot());
		m_offset.changed_signal().connect(make_reset_mesh_slot());
		m_frequency.changed_signal().connect(make_reset_mesh_slot());
		m_noise_offset.changed_signal().connect(make_reset_mesh_slot());
		m_orientation.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Calculate standard terrain parameters ...
		const unsigned long iterations = m_iterations.pipeline_value();
		const unsigned long points = static_cast<unsigned long>(pow(2.0, static_cast<double>(iterations)));
		const unsigned long segments = points - 1;
		const double terrain_width = 20.0;
		k3d::imaterial* const material = m_material.pipeline_value();

		std::auto_ptr<k3d::legacy::mesh> mesh(new k3d::legacy::mesh());

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		// Create points ...
		const double terrain_step = terrain_width / static_cast<double>(segments);
		double terrain_z = -terrain_width / 2;
		for(unsigned long z = 0; z < points; z++)
		{
			double terrain_x = -terrain_width / 2;
			for(unsigned long x = 0; x < points; x++)
			{
				Mesh.points.push_back(new k3d::legacy::point(terrain_x, 0, terrain_z));
				terrain_x += terrain_step;
			}

			terrain_z += terrain_step;
		}

		// Create triangle grid ...
		for(unsigned long z = 0; z < segments; z++)
			for(unsigned long x = 0; x < segments; x++)
			{
				create_hfbm_triangle(polyhedron, Mesh.points[z*points + x], Mesh.points[z*points + x+1], Mesh.points[(z+1)*points + x+1], material);
				create_hfbm_triangle(polyhedron, Mesh.points[z*points + x], Mesh.points[(z+1)*points + x+1], Mesh.points[(z+1)*points + x], material);
			}

		// Calculate surface elevations ...
		std::vector<double> exponent_array;

		double frequency = 1.0;
		for(unsigned long i = 0; i <= m_octaves.pipeline_value(); i++)
		{
			exponent_array.push_back(pow(frequency, -m_fractal_dimension.pipeline_value()));
			frequency *= m_lacunarity.pipeline_value();
		}

		k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin();
		for(unsigned long z = 0; z < points; z++)
		{
			for(unsigned long x = 0; x < points; x++)
			{
				// 5 is an arbitrary constant to keep same y-scale ratio as other terrain algorithms
				const double elevation = get_elevation(x, z, points, exponent_array, m_frequency.pipeline_value(), m_noise_offset.pipeline_value(), m_offset.pipeline_value(), m_lacunarity.pipeline_value(), m_octaves.pipeline_value());

				(*point)->position.n[1] = elevation;
				point++;
			}
		}

		// Set orientation
		k3d::signed_axis orientation = m_orientation.pipeline_value();
		for(k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
		{
			// Update orientation
			k3d::point3& position = (*point)->position;
			const double x = position[0];
			const double y = position[1];
			const double z = position[2];
			switch(orientation)
			{
				case k3d::PX:
					position = k3d::point3(y, -x, z);
					break;
				case k3d::NX:
					position = k3d::point3(-y, -x, -z);
					break;
				case k3d::PY:
					position = k3d::point3(x, y, z);
					break;
				case k3d::NY:
					position = k3d::point3(x, -y, -z);
					break;
				case k3d::PZ:
					position = k3d::point3(x, -z, y);
					break;
				case k3d::NZ:
					position = k3d::point3(x, z, -y);
					break;
			}
		}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_terrain_hfbm_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xff22f8f8, 0xa8b540f6, 0xb612a012, 0x8d4e9adb),
			"PolyTerrainHfBM",
			_("Generates an HfBM-based fractal terrain"),
			"Polygons",
			k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_iterations;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_fractal_dimension;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_lacunarity;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_octaves;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_offset;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_frequency;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_noise_offset;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
};

/////////////////////////////////////////////////////////////////////////////
// poly_terrain_hfbm_factory

k3d::iplugin_factory& poly_terrain_hfbm_factory()
{
	return poly_terrain_hfbm_implementation::get_factory();
}

} // namespace libk3dprimitives

