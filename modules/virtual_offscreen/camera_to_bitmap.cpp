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

#include <k3d-i18n-config.h>
#include <k3d-module-config.h>
#include <k3dsdk/ibitmap_source.h>
#include <k3dsdk/log.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/virtual_document_plugin_factory.h>

#if defined K3D_BUILD_GLX_MODULE
	static const std::string plugin_name = "GLXCameraToBitmap";
#elif defined K3D_BUILD_WGL_MODULE
	static const std::string plugin_name = "WGLCameraToBitmap";
#endif

namespace module
{

namespace virtual_offscreen
{

class virtual_camera_to_bitmap_factory:
	public k3d::virtual_document_plugin_factory<k3d::interface_list<k3d::ibitmap_source> >
{
	typedef k3d::virtual_document_plugin_factory<k3d::interface_list<k3d::ibitmap_source> > base;

public:
	virtual_camera_to_bitmap_factory() :
		base(k3d::uuid(0xc9a3d742, 0x4646dd3a, 0x95c49480, 0x428e7641),
			"VirtualCameraToBitmap",
			_("Renders the scene to a bitmap using OpenGL"),
			"Bitmap Test",
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
			const k3d::factories_t factories = k3d::plugins(plugin_name);
			if(1 == factories.size())
				delegate = dynamic_cast<k3d::idocument_plugin_factory*>(*factories.begin());

			k3d::log() << info << this->name() << " delegating to " << dynamic_cast<k3d::iplugin_factory*>(delegate)->name() << std::endl;
		}

		return_val_if_fail(delegate, 0);
		return delegate->create_plugin(*this, Document);
	}

private:
	k3d::idocument_plugin_factory* delegate;
};

k3d::iplugin_factory& camera_to_bitmap_factory()
{
	static virtual_camera_to_bitmap_factory instance;
	return instance;
}

} // namespace virtual_offscreen

} // namespace module

