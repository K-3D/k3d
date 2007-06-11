// K-3D
// Copyright (c) 2003-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Creates an FFT-based fractal landscape
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <complex>

namespace libk3dprimitives
{

/// Creates terrains out of a brownian movement using Fast Fourrier Transform
// (actually a 2-variable discrete inverse FFT is used)

class terrain_seed
{
public:
	/// Store four random numbers
	k3d::point4 m_Seeds;

	terrain_seed() {}
	terrain_seed(k3d::point4 seed) { m_Seeds = seed; }

	// A random variable
	double drand()
	{
		double	d = (double)rand()/(double)RAND_MAX;
		if(d < 1e-6) return 1e-6;
		return d;
	}

	// A normal random variable
	double normaldrand(int k, int l, double d)
	{
		double dk = (double)k, dl = (double)l;
		double sigma = 1/exp((4-d)/2*log(dk*dk + dl*dl));
		return sigma*sqrt(-2*log(m_Seeds.n[0]))*cos(2*k3d::pi()*m_Seeds.n[1]);
	}

	// A normal random complex variable
	std::complex<double> cplxnormaldrand(int k, int l, double d)
	{
		double dk = (double)k, dl = (double)l;
		double sigma = 1/exp((4-d)/2*log(dk*dk + dl*dl));
		return sigma*std::complex<double>(sqrt(-2*log(m_Seeds.n[0]))*cos(2*k3d::pi()*m_Seeds.n[1]), sqrt(-2*log(m_Seeds.n[2]))*cos(2*k3d::pi()*m_Seeds.n[3]));
	}

	void Randomize()
	{
		for(unsigned long i = 0; i < 4; i++)
			m_Seeds.n[i] = drand();
	}
};

class FFT_vector
{
public:
	FFT_vector(int Size) :
		m_Size(Size)
	{
		P.resize(m_Size);
		Roots.resize(m_Size);
		Temp.resize(m_Size);

		Roots[0] = std::complex<double>(1.0);
		std::complex<double> Wt = std::polar<double>(1.0, k3d::pi_times_2() / static_cast<double>(m_Size));
		std::complex<double> Wj = Wt;
		for(int i = m_Size-1; i > 0; i--)
		{
			Roots[i] = Wt;
			Wt *= Wj;
		}
	}

	void InvFFT()
	{
		RealFFT(m_Size, 0);
	}

	void RealFFT(unsigned long d, unsigned long k)
	{
		if(d < 2)
			return;

		unsigned long i, j;

		for(i = 0; i < d/2; i++)
		{
			j = k + 2*i;
			Temp[i] = P[j];
			Temp[i+d/2] = P[j+1];
		}

		for(i = 0; i < d; i++)
			P[k+i] = Temp[i];

		RealFFT(d/2, k);
		RealFFT(d/2, k+d/2);

		j = m_Size / d;
		for(i = 0; i < d/2; i++)
		{
			std::complex<double> aux = Roots[i*j] * P[k+d/2+i];
			Temp[i] = P[k+i] + aux;
			Temp[i+d/2] = P[k+i] - aux;
		}

		for(i = 0; i < d; i++)
			P[k+i] = Temp[i];
	}

	std::vector< std::complex<double> > P;

protected:
	int m_Size;

	std::vector< std::complex<double> > Roots;
	std::vector< std::complex<double> > Temp;
};

typedef FFT_vector* FFT_map;


// Generate a fractal terrain using preset random values with 2D FFT
typedef std::vector< std::vector<terrain_seed> > terrain_seeds_t;
FFT_map* seeds_to_terrain(unsigned long N, double d, terrain_seeds_t& TerrainSeeds)
{
	// Allocate terrain ...
	FFT_map* terrain = new FFT_map[N];
	for(unsigned long n = 0; n < N; n++)
		terrain[n] = new FFT_vector(N);

	// Set all values using a normal random variable (NRV),
	// Array[0][0] = 0, Array[0][N/2] = NRV, Array[N/2][0] = NRV,
	// Array[N/2][N/2]; fill up the array a normal random complex
	// variable (CRNV) following the rule : CRNVi = Conjugate(CRNVj)
	// when CRNVi and CRNVj are symmetrically placed around one
	// of the three real values :

	//  0  C  C  C R1  C  C  C  (N = 8)
	//  C  C  C C1  C  C  C  C
	//  C  C  C  C  C  C  C  C
	// C3  C  C  C  C  C  C  C
	// R2  C  C  C R3  C  C  C  C3 = Conjugate(C4), C1 = Conjugate(C2)
	// C4  C  C  C  C  C  C  C
	//  C  C  C  C  C  C  C  C
	//  C  C  C  C  C C2  C  C


	// 0, R1, R2, R3

	terrain[0]->P[0] = std::complex<double>(0);
	terrain[0]->P[N/2] = std::complex<double>(TerrainSeeds[0][N/2].normaldrand(0, N/2, d));
	terrain[N/2]->P[0] = std::complex<double>(TerrainSeeds[N/2][0].normaldrand(N/2, 0, d));
	terrain[N/2]->P[N/2] = std::complex<double>(TerrainSeeds[N/2][N/2].normaldrand(N/2, N/2, d));

	// Row[0] and Row[N/2]

	std::complex<double> Tmp;
	unsigned long k;
	for(k = 1; k < N/2; k++)
	{
		Tmp = TerrainSeeds[0][k].cplxnormaldrand(0, k, d);
		terrain[k]->P[0] = Tmp;
		terrain[N-k]->P[0] = std::conj(Tmp);

		Tmp = TerrainSeeds[N/2][k].cplxnormaldrand(N/2, k, d);
		terrain[k]->P[N/2] = Tmp;
		terrain[N-k]->P[N/2] = std::conj(Tmp);
	}

	// Column[0] and Column[N/2]

	for(k = 1; k < N/2; k++)
	{
		Tmp = TerrainSeeds[k][0].cplxnormaldrand(k, 0, d);
		terrain[0]->P[k] = Tmp;
		terrain[0]->P[N-k] = std::conj(Tmp);

		Tmp = TerrainSeeds[k][N/2].cplxnormaldrand(k, N/2, d);
		terrain[N/2]->P[k] = Tmp;
		terrain[N/2]->P[N-k] = std::conj(Tmp);
	}

	// Fill up
	unsigned long l;
	for(k = 1; k < N/2; k++)
		for(l = 1; l < N/2; l++)
		{
			Tmp = TerrainSeeds[k][l].cplxnormaldrand(k, l, d);
			// The square inside 0 R1 R2 R3 and symmtrical values
			terrain[k]->P[l] = Tmp;
			terrain[N-k]->P[N-l] = std::conj(Tmp);

			Tmp = TerrainSeeds[N-k][l].cplxnormaldrand(k, l, d);
			// Bottom left inside square and symmetrical values
			terrain[N-k]->P[l] = Tmp;
			terrain[k]->P[N-l] = std::conj(Tmp);
		}


	// 2D inverse FFT

	// Apply InvFFT on each row
	for(k = 0; k < N; k++)
		terrain[k]->InvFFT();

	// Apply InvFFT on each column
	FFT_vector TmpFFT(N);
	for(l = 0; l < N; l++)
	{
		for(k = 0; k < N; k++) TmpFFT.P[k] = terrain[k]->P[l];
		TmpFFT.InvFFT();
		for(k = 0; k < N; k++) terrain[k]->P[l] = TmpFFT.P[k];
	}

	return terrain;
}

bool create_fft_triangle(k3d::legacy::polyhedron& Polyhedron, k3d::legacy::point* Point1, k3d::legacy::point* Point2, k3d::legacy::point* Point3, k3d::imaterial* const Material)
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
// poly_terrain_fft_implementation

class poly_terrain_fft_implementation :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_terrain_fft_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_iterations(init_owner(*this) + init_name("iterations") + init_label(_("Iterations")) + init_description(_("Subdivision iterations")) + init_value(4) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_fractal_dimension(init_owner(*this) + init_name("dimension") + init_label(_("Dimension")) + init_description(_("Fractal dimension")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_random_seed(init_owner(*this) + init_name("randomseed") + init_label(_("Random seed")) + init_description(_("Random seed")) + init_value(123) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values()))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_iterations.changed_signal().connect(make_reset_mesh_slot());
		m_fractal_dimension.changed_signal().connect(make_reset_mesh_slot());
		m_random_seed.changed_signal().connect(make_reset_mesh_slot());
		m_orientation.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Calculate standard terrain parameters ...
		const unsigned long iterations = m_iterations.value();
		const unsigned long points = static_cast<unsigned long>(pow(2, iterations));
		const unsigned long segments = points - 1;
		const double terrain_width = 20.0;
		k3d::imaterial* const material = m_material.value();

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
				create_fft_triangle(polyhedron, Mesh.points[z*points + x], Mesh.points[z*points + x+1], Mesh.points[(z+1)*points + x+1], material);
				create_fft_triangle(polyhedron, Mesh.points[z*points + x], Mesh.points[(z+1)*points + x+1], Mesh.points[(z+1)*points + x], material);
			}

		// Random seeds for a fractal terrain (built using rules)
		terrain_seeds_t terrain_seeds;

		srand(m_random_seed.value());

		terrain_seeds.resize(points);
		for(unsigned long i = 0; i < points; i++)
		{
			terrain_seeds[i].resize(points);
			for(unsigned long j = 0; j < points; j++)
				terrain_seeds[i][j].Randomize();
		}

		// Calculate surface elevations ...
		FFT_map* terrain = seeds_to_terrain(points, m_fractal_dimension.value(), terrain_seeds);

		k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin();
		for(unsigned long z = 0; z < points; z++)
		{
			for(unsigned long x = 0; x < points; x++)
			{
				// 5 is an arbitrary constant to keep same y-scale ratio as other terrain algorithms
				const double elevation = terrain[z]->P[x].real() / 5;

				//const double y = m_sea.value() && elevation < m_sea_level.value() ? m_sea_level.value() : elevation;
				(*point)->position.n[1] = elevation;
				point++;
			}
		}

		// Set orientation
		k3d::signed_axis orientation = m_orientation.value();
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
		static k3d::document_plugin_factory<poly_terrain_fft_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x7646f5a1, 0x3f3640d6, 0x8d4c70af, 0x91bcb418),
			"PolyTerrainFFT",
			_("Generates an FFT-based fractal terrain"),
			"Polygons",
			k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_iterations;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_fractal_dimension;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_random_seed;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
};

/////////////////////////////////////////////////////////////////////////////
// poly_terrain_fft_factory

k3d::iplugin_factory& poly_terrain_fft_factory()
{
	return poly_terrain_fft_implementation::get_factory();
}

} // namespace libk3dprimitives

