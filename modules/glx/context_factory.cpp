// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/gl/api.h>
#include <k3dsdk/gl/context_factory.h>
#include <k3dsdk/gl/context.h>
#include <k3dsdk/module.h>
#include <k3dsdk/result.h>

#include <boost/optional.hpp>

#include <iterator>
#include <stdexcept>

#include <GL/glx.h>
#include <X11/Xlib.h>

#include <dlfcn.h>

namespace module
{

namespace glx
{

class context :
	public k3d::gl::context
{
public:
	context(Display* const XDisplay, const GLXContext XContext, const GLXDrawable XDrawable, k3d::gl::api& API) :
		x_display(XDisplay),
		x_context(XContext),
		x_drawable(XDrawable),
		api(API)
	{
	}

	~context()
	{
		glXDestroyContext(x_display, x_context);
//		glXDestroyWindow(x_display, x_drawable);
	}

	const k3d::gl::api& begin()
	{
		try
		{
			if(!glXMakeCurrent(x_display, x_drawable, x_context))
				throw std::runtime_error("Error making GLX context current.");
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}
		return api;
	}

	void end()
	{
		glXSwapBuffers(x_display, x_drawable);
	}

	Display* const x_display;
	const GLXContext x_context;
	const GLXDrawable x_drawable;
	const k3d::gl::api& api;
};

struct api_loader
{
	api_loader(void* Module) :
		module(Module)
	{
	}

	template<typename FunctionT>
	void operator()(const char* Name, FunctionT& Function) const
	{
		if(void* function = dlsym(module, Name))
			Function = FunctionT(function);
	}

	void* const module;
};

class context_factory :
	public k3d::gl::context_factory
{
public:
	context_factory()
	{
	}

	k3d::gl::context* create(k3d::uint64_t Drawable)
	{
		try
		{
			if(!api)
			{
				void* const module = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
				if(!module)
					throw std::runtime_error(dlerror());

				api = k3d::gl::api();
				api.get().load(api_loader(module));
			}

			if(!api)
				throw std::runtime_error("Failed to initialize OpenGL library.");

			Display* const x_display = XOpenDisplay(0);
			if(!x_display)
				throw std::runtime_error("Error opening X display.");

			int dummy = 0;
			if(!glXQueryExtension(x_display, &dummy, &dummy))
				throw std::runtime_error("X display doesn't support GLX.");

			int x_config[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 1, GLX_DOUBLEBUFFER, None};
			XVisualInfo* const x_visual = glXChooseVisual(x_display, DefaultScreen(x_display), x_config);

			if(!x_visual)
				throw std::runtime_error("Error choosing X visual.");
		
			const GLXContext x_context = glXCreateContext(x_display, x_visual, 0, False);
			if(!x_context)
				throw std::runtime_error("Error creating X context.");

			return new context(x_display, x_context, GLXDrawable(Drawable), api.get());

/*
			const int x_attributes[] = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DOUBLEBUFFER, True, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, None };
			int x_config_count = 0;
			const GLXFBConfig* const x_configs = glXChooseFBConfig(x_display, DefaultScreen(x_display), x_attributes, &x_config_count);
			if(!x_configs)
				throw std::runtime_error("Error choosing GLXFBConfig.");
			if(!x_config_count)
				throw std::runtime_error("No compatible GLXFBConfig.");

			const GLXContext x_context = glXCreateNewContext(x_display, x_configs[0], GLX_RGBA_TYPE, NULL, True);
			if(!x_context)
				throw std::runtime_error("Error creating GLXContext.");

			const GLXDrawable x_drawable = glXCreateWindow(x_display, x_configs[0], XID(Drawable), NULL);
			if(!x_drawable)
				throw std::runtime_error("Error creating GLXDrawable.");

			return new context(x_display, x_context, x_drawable, api.get());
*/
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return 0;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<context_factory,
			k3d::interface_list<k3d::gl::context_factory> > factory(
				k3d::uuid(0x3a6869e9, 0x9a4ed128, 0x0f06b4bc, 0x672b53e1),
				"GLXContextFactory",
				_("Creates GLX graphics contexts."),
				"OpenGL",
				k3d::iplugin_factory::EXPERIMENTAL
				);

		return factory;
	}

private:
	static boost::optional<k3d::gl::api> api;
};

boost::optional<k3d::gl::api> context_factory::api;

/////////////////////////////////////////////////////////////////////////////
// context_factory_factory

k3d::iplugin_factory& context_factory_factory()
{
	return context_factory::get_factory();
}

} // namespace glx

} // namespace module

