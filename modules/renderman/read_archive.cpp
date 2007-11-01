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
		\brief Implements the read_archive K-3D object, which allows geometry to be rendered with boolean effects
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/renderable_ri.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////////////////////////
// read_archive

class read_archive :
	public k3d::persistent<k3d::node> ,
	public k3d::ri::irenderable
{
	typedef k3d::persistent<k3d::node>  base;

public:
	read_archive(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("file")) + init_description(_("RIB archive to be inserted in output")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("rib_archive"))
	{
	}

	void renderman_render(const k3d::ri::render_state& State)
	{
		// We only generate RIB on the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		const k3d::filesystem::path file = m_file.pipeline_value();
		if(!k3d::filesystem::exists(file))
			return;

		State.stream.RiReadArchive(file);
	}

	void renderman_render_complete(const k3d::ri::render_state& State)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<read_archive >factory(
			k3d::uuid(0x5dc6fac5, 0xd9a54c79, 0x929f1612, 0x6f83a8b8),
			"RenderManReadArchive",
			_("Inserts a RenderMan ReadArchive call into rendered output"),
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& read_archive_factory()
{
	return read_archive::get_factory();
}

} // namespace libk3drenderman

