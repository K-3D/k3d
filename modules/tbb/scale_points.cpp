// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

#include <pthread.h>

namespace module
{

namespace tbb
{

/////////////////////////////////////////////////////////////////////////////
// scale_points

class scale_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	scale_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z")) + init_description(_("Z scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_thread_count(init_owner(*this) + init_name("thread_count") + init_label(_("Thread Count")) + init_description(_("Number of threads to use for computation.")) + init_value(1) + init_constraint(constraint::minimum(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_grain_size(init_owner(*this) + init_name("grain_size") + init_label(_("Grain Size")) + init_description(_("Minimum number of computations to assign to a thread.")) + init_value(1) + init_constraint(constraint::minimum(0)) + init_step_increment(1000) + init_units(typeid(k3d::measurement::scalar))),
		m_scheduler(::tbb::task_scheduler_init::deferred)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_x.changed_signal().connect(make_update_mesh_slot());
		m_y.changed_signal().connect(make_update_mesh_slot());
		m_z.changed_signal().connect(make_update_mesh_slot());
		m_thread_count.changed_signal().connect(make_update_mesh_slot());
		m_grain_size.changed_signal().connect(make_update_mesh_slot());
	}

	class worker
	{
	public:
		worker(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints, const k3d::matrix4& Matrix) :
			input_points(InputPoints),
			point_selection(PointSelection),
			output_points(OutputPoints),
			matrix(Matrix)
		{
		}

		void operator()(const ::tbb::blocked_range<k3d::uint_t>& range) const
		{
k3d::log() << debug << __PRETTY_FUNCTION__ << " [" << range.begin() << ", " << range.end() << ") thread: " << pthread_self() << std::endl;

			const k3d::uint_t point_begin = range.begin();
			const k3d::uint_t point_end = range.end();
			for(k3d::uint_t point = point_begin; point != point_end; ++point)
				output_points[point] = k3d::mix(input_points[point], matrix * input_points[point], point_selection[point]);
		}

	private:
		const k3d::mesh::points_t& input_points;
		const k3d::mesh::selection_t& point_selection;
		k3d::mesh::points_t& output_points;
		const k3d::matrix4& matrix;
	};

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;

		const k3d::matrix4 matrix = k3d::scaling3D(k3d::point3(m_x.pipeline_value(), m_y.pipeline_value(), m_z.pipeline_value()));
		const k3d::int32_t thread_count = m_thread_count.pipeline_value();
		const k3d::int32_t grain_size = m_grain_size.pipeline_value();

		::tbb::tick_count t0 = ::tbb::tick_count::now();

		m_scheduler.initialize(thread_count);

		::tbb::parallel_for(
			::tbb::blocked_range<k3d::uint_t>(0, OutputPoints.size(), grain_size),
			worker(InputPoints, PointSelection, OutputPoints, matrix));

		m_scheduler.terminate();

		::tbb::tick_count t1 = ::tbb::tick_count::now();

k3d::log() << debug << (t1 - t0).seconds() << " seconds" << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scale_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x38ffd3c1, 0xc048e25f, 0x360f4a9b, 0xdf2d3ce3),
				"TBBScalePoints",
				_("Scales mesh points in parallel"),
				"Deformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;

	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_thread_count;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_grain_size;

	::tbb::task_scheduler_init m_scheduler;
};

/////////////////////////////////////////////////////////////////////////////
// scale_points_factory

k3d::iplugin_factory& scale_points_factory()
{
	return scale_points::get_factory();
}

} // namespace tbb

} // namespace module

