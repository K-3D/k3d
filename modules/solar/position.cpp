// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
*/

#include <solpos.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>
#include <k3dsdk/vector3.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

namespace module
{

namespace solar
{

/////////////////////////////////////////////////////////////////////////////
// position

class position :
	public k3d::node
{
	typedef k3d::node base;

public:
	position(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_latitude(init_owner(*this) + init_name("latitude") + init_label(_("Latitude")) + init_description(_("Observer latitude (decimal degrees).")) + init_value(k3d::radians(35.110703)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_longitude(init_owner(*this) + init_name("longitude") + init_label(_("Longitude")) + init_description(_("Observer longitude (decimal degrees).")) + init_value(k3d::radians(-106.609991)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Time since 1970-1-1, in seconds.")) + init_value(0.0) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::time))),
		m_time_zone_offset(init_owner(*this) + init_name("time_zone_offset") + init_label(_("Time Zone Offset")) + init_description(_("Time zone offset in minutes, [-720, +720].  Note that you must adjust the timezone manually to account for daylight savings time.")) + init_value(-7 * 60)),
		m_north(init_owner(*this) + init_name("north") + init_label(_("North")) + init_description(_("Specifies which direction will be treated as 'North'.")) + init_value(k3d::vector3(0, -1, 0))),
		m_east(init_owner(*this) + init_name("east") + init_label(_("East")) + init_description(_("Specifies which direction will be treated as 'East'.")) + init_value(k3d::vector3(1, 0, 0))),
		m_up(init_owner(*this) + init_name("up") + init_label(_("Up")) + init_description(_("Specifies which direction will be treated as 'up'.")) + init_value(k3d::vector3(0, 0, 1))),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Solar Position")) + init_description(_("Solar position.")) + init_value(k3d::vector3(0, 0, 1)))
	{
		// Note: all input change events pass through our own on_input_changed() method,
		// instead of routing directly to the position property.  This gives us a chance
		// to reset any cached data, and reduces the number of hint mappings if we provide
		// additional outputs in the future.
		m_latitude.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_longitude.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_time.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_time_zone_offset.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_north.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_east.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));
		m_up.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_slot()));

		m_position.set_update_slot(sigc::mem_fun(*this, &position::compute_position));
	}

	sigc::slot<void, k3d::ihint*> make_slot()
	{
		return sigc::mem_fun(*this, &position::on_input_changed);
	}

	void on_input_changed(k3d::ihint*)
	{
		m_computed_data.reset();
		m_position.update();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<position> factory(
				k3d::uuid(0xc96348f9, 0xc842a921, 0x9389f091, 0xa0c4c1dd),
				"SolarPosition",
				_("Calculates the position of the Sun based on observer position, date, and time."),
				"Metrics",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_latitude;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_longitude;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_time;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_time_zone_offset;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_north;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_east;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_up;
	k3d_data(k3d::vector3, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_position;

	boost::scoped_ptr<posdata> m_computed_data;

	posdata& computed_data()
	{
		// Compute solar position using SOLPOS 2.0, developed by the National Renewable Energy Laboratory, http://www.nrel.gov
		// See http://rredc.nrel.gov/solar/codes_algs/solpos/ for details.

		if(!m_computed_data)
		{
			m_computed_data.reset(new posdata);
			S_init(m_computed_data.get());

			m_computed_data->function &= ~S_DOY;

			m_computed_data->latitude = k3d::degrees(m_latitude.pipeline_value());
			m_computed_data->longitude = k3d::degrees(m_longitude.pipeline_value());

			const double_t input_seconds = m_time.pipeline_value();
			const double_t input_whole_seconds = std::floor(input_seconds);
			const double_t input_fractional_seconds = input_seconds - input_whole_seconds;

			boost::posix_time::ptime input_time = boost::posix_time::from_time_t(static_cast<time_t>(input_whole_seconds));
			input_time += boost::posix_time::time_duration(0, 0, 0, input_fractional_seconds * boost::posix_time::time_duration::ticks_per_second());
			input_time -= boost::posix_time::time_duration(0, m_time_zone_offset.pipeline_value(), 0, 0);

			m_computed_data->timezone = 0;
			m_computed_data->year = input_time.date().year();
			m_computed_data->month = input_time.date().month();
			m_computed_data->day = input_time.date().day();
			m_computed_data->hour = input_time.time_of_day().hours();
			m_computed_data->minute = input_time.time_of_day().minutes();
			m_computed_data->second = input_time.time_of_day().seconds();

			long result = S_solpos(m_computed_data.get());
			S_decode(result, m_computed_data.get());
		}

		return *m_computed_data;
	}

	void compute_position(const std::vector<k3d::ihint*>& Hints, k3d::vector3& Position)
	{
		posdata& data = computed_data();

		const k3d::double_t cos_azimuth = std::cos(k3d::radians(data.azim));
		const k3d::double_t sin_azimuth = std::sin(k3d::radians(data.azim));

		const k3d::double_t cos_elevation = std::cos(k3d::radians(data.elevetr));
		const k3d::double_t sin_elevation = std::sin(k3d::radians(data.elevetr));

		Position = k3d::vector3(0, 0, 0);
		Position += cos_elevation * cos_azimuth * k3d::normalize(m_north.pipeline_value());
		Position += cos_elevation * sin_azimuth * k3d::normalize(m_east.pipeline_value());
		Position += sin_elevation * k3d::normalize(m_up.pipeline_value());
	}
};

/////////////////////////////////////////////////////////////////////////////
// position

k3d::iplugin_factory& position_factory()
{
	return position::get_factory();
}

} // namespace solar

} // namespace module

