// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief An object that converts a scalar to a string using strftime()
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3d-platform-config.h>

#include <boost/format.hpp>

#include <time.h>

namespace libk3dcore
{

class format_time :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	format_time(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input") + init_label(_("Input")) + init_description(_("Timestamp value, or 0 to use current system time")) + init_value(0.0)),
		m_format(init_owner(*this) + init_name("format") + init_label(_("Format")) + init_description(_("strftime() format string")) + init_value(std::string("%H:%M:%S"))),
		m_output(init_owner(*this) + init_name("output") + init_label(_("Output string")) + init_description(_("strftime() output string (read only)")) + init_slot(sigc::mem_fun(*this, &format_time::get_value)))
	{
		m_input.changed_signal().connect(m_output.make_reset_slot());
		m_format.changed_signal().connect(m_output.make_reset_slot());
	}

	std::string get_value()
	{
		const time_t time = static_cast<time_t>(m_input.pipeline_value());
		tm time_value;

#if defined K3D_API_WIN32
		time_value = *gmtime(&time);
#else
		gmtime_r(&time, &time_value);
#endif

		std::string buffer(128, '\0');
		while(buffer.size() < 2048)
		{
			const int buffer_size = strftime(const_cast<char*>(buffer.data()), buffer.size()-1, m_format.pipeline_value().c_str(), &time_value);
			if(buffer_size)
			{
				buffer.resize(buffer_size);
				return buffer;
			}
			buffer.resize(buffer.size() * 2);
		}

		k3d::log() << error << k3d_file_reference << " buffer overrun" << std::endl;
		return std::string();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<format_time > factory(
			k3d::uuid(0x40d699fb, 0xeb12444f, 0xa84df477, 0x5cca00f9),
			"FormatTime",
			_("Converts a scalar to a string using strftime()-style formatting"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_format;
	k3d_data(std::string, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

k3d::iplugin_factory& format_time_factory()
{
	return format_time::get_factory();
}

} //namespace libk3dcore

