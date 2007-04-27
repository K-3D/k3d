// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3dsdk/extension_gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/log.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/virtual_document_plugin_factory.h>

namespace libk3dvirtualglpainters
{

class virtual_sds_face_painter_factory:
	public k3d::virtual_document_plugin_factory<k3d::interface_list<k3d::gl::imesh_painter> >
{
	typedef k3d::virtual_document_plugin_factory<k3d::interface_list<k3d::gl::imesh_painter> > base;

public:
	virtual_sds_face_painter_factory() :
		base(k3d::uuid(0x9d4043c8, 0x864fe71b, 0x38706d85, 0x8b60af0c),
			"GLSDSFacePainter",
			_("Renders mesh subdivision surface faces"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL),
		delegate(0)
	{
		// Note ... this ctor may be executed before k3d::application is initialized, so
		// we can't do very much here
	}

	k3d::inode* create_plugin(k3d::iplugin_factory& Factory, k3d::idocument& Document)
	{
		if(!delegate)
		{
			if(k3d::gl::extension::query_vbo())
			{
				const k3d::factories_t factories = k3d::plugins("SDSFacePainter");
				if(1 == factories.size())
					delegate = dynamic_cast<k3d::idocument_plugin_factory*>(*factories.begin());
			}
		}

		return_val_if_fail(delegate, 0);
		return delegate->create_plugin(*this, Document);
	}

private:
	k3d::idocument_plugin_factory* delegate;
};

k3d::iplugin_factory& sds_face_painter_factory()
{
	static virtual_sds_face_painter_factory instance;
	return instance;
}

} // namespace libk3dvirtualglpainters

