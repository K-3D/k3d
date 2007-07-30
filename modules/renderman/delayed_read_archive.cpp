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
		\brief Implements the RenderManScript K-3D object, which can insert scripted data into RenderMan output
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/bounding_box.h>
#include <k3dsdk/drawable_gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/transformable.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// delayed_read_archive

class delayed_read_archive :
	public k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > >
{
	typedef k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > base;

public:
	delayed_read_archive(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("file")) + init_description(_("RIB archive to be inserted in output")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("rib_archives")),
		m_bounding_box(init_owner(*this) + init_name("bounding_box") + init_label(_("bounding_box")) + init_description(_("Visibility bounding box")) + init_value(k3d::bounding_box3(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5)))
	{
		m_bounding_box.changed_signal().connect(make_async_redraw_slot());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		const k3d::bounding_box3 bounding_box = m_bounding_box.pipeline_value();

		glDisable(GL_LIGHTING);
		glColor3d(1, 1, 0);

		k3d::gl::draw_bounding_box(bounding_box);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		// We only generate RIB on the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		const k3d::filesystem::path file = m_file.pipeline_value();
		if(!k3d::filesystem::exists(file))
			return;

		const k3d::bounding_box3 bounding_box = m_bounding_box.pipeline_value();

		State.engine.RiProcDelayedReadArchive(file, bounding_box);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<delayed_read_archive,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
			k3d::uuid(0x65fd1c9e, 0xa18b403a, 0xbccafc19, 0xdf504c75),
			"RenderManDelayedReadArchive",
			_("Inserts a RenderMan DelayedReadArchive call into rendered output"),
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	k3d_data(k3d::bounding_box3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bounding_box;
};

k3d::iplugin_factory& delayed_read_archive_factory()
{
	return delayed_read_archive::get_factory();
}

} // namespace libk3drenderman


