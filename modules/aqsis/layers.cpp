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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iaqsis.h>
#include <k3dsdk/ishader_collection_ri.h>
#include <k3dsdk/idisplacement_shader_ri.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/isurface_shader_ri.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/shader_ri.h>

namespace module
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// layer

/// Abstract base class that provides most of the implementation for our shader objects
class layer :
	public k3d::ri::shader
{
	typedef k3d::ri::shader base;

public:
	layer(k3d::iplugin_factory& Factory, k3d::idocument& Document, const k3d::sl::shader::type_t ShaderType) :
		base(Factory, Document, ShaderType)
	{
	}

protected:
	template<typename LayerT, typename ShaderT>
	void setup_connected_layers(const k3d::ri::render_state& State, void (ShaderT::*SetupFunction)(const k3d::ri::render_state&))
	{
		const iproperty_collection::properties_t& properties = this->properties();
		for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			k3d::aqsis::ilayer_connection_property* const layer_property = dynamic_cast<k3d::aqsis::ilayer_connection_property*>(*property);
			if(!layer_property)
				continue;

			if((*property)->property_type() != typeid(k3d::inode*))
			{
				k3d::log() << warning << "layer connection property has incorrect type" << std::endl;
				continue;
			}

			ShaderT* shader = dynamic_cast<ShaderT*>(dynamic_cast<LayerT*>(boost::any_cast<k3d::inode*>((*property)->property_value())));
			if(!shader)
				continue;

			(shader->*SetupFunction)(State);
		}
	}

	template<typename LayerT>
	void setup_connections(const std::string& Type, const k3d::ri::render_state& State)
	{
		const iproperty_collection::properties_t& properties = this->properties();
		for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			k3d::aqsis::ilayer_connection_property* const layer_property = dynamic_cast<k3d::aqsis::ilayer_connection_property*>(*property);
			if(!layer_property)
				continue;

			if((*property)->property_type() != typeid(k3d::inode*))
			{
				k3d::log() << warning << "layer connection property has incorrect type" << std::endl;
				continue;
			}


			k3d::inode* const node = boost::any_cast<k3d::inode*>((*property)->property_value());
			if(!node)
				continue;

			if(!dynamic_cast<LayerT*>(node))
				continue;

			const std::string source_name = node->name();
			const std::string source_variable = layer_property->get_source_variable();
			const std::string target_name = name();
			const std::string target_variable = layer_property->get_target_variable();

			State.engine.RiConnectShaderLayers(Type, source_name, source_variable, target_name, target_variable);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// displacement_shader_layer

/// Implements an Aqsis displacement shader layer
class displacement_shader_layer :
	public layer,
	public k3d::ri::idisplacement_shader,
	public k3d::aqsis::idisplacement_layer
{
	typedef layer base;

public:
	displacement_shader_layer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::DISPLACEMENT)
	{
	}
	void setup_renderman_displacement_shader(const k3d::ri::render_state& State)
	{
		setup_connected_layers<k3d::aqsis::idisplacement_layer, k3d::ri::idisplacement_shader>(State, &k3d::ri::idisplacement_shader::setup_renderman_displacement_shader);
		State.shaders.use_shader(shader_path());
		State.engine.RiShaderLayerV("displacement", shader_path(), shader_name(), name(), shader_arguments(State));
		setup_connections<k3d::aqsis::idisplacement_layer>("displacement", State);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<displacement_shader_layer,
				k3d::interface_list<k3d::ri::idisplacement_shader,
				k3d::interface_list<k3d::aqsis::idisplacement_layer> > > factory(
			k3d::classes::AqsisDisplacementShaderLayer(),
			"AqsisDisplacementShaderLayer",
			"Encapsulates an Aqsis displacement shader layer",
			"Aqsis",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// surface_shader_layer

/// Implements an Aqsis surface shader layer
class surface_shader_layer :
	public layer,
	public k3d::ri::isurface_shader,
	public k3d::aqsis::isurface_layer
{
	typedef layer base;

public:
	surface_shader_layer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::SURFACE)
	{
	}
	void setup_renderman_surface_shader(const k3d::ri::render_state& State)
	{
		setup_connected_layers<k3d::aqsis::isurface_layer, k3d::ri::isurface_shader>(State, &k3d::ri::isurface_shader::setup_renderman_surface_shader);
		State.shaders.use_shader(shader_path());
		State.engine.RiShaderLayerV("surface", shader_path(), shader_name(), name(), shader_arguments(State));
		setup_connections<k3d::aqsis::isurface_layer>("surface", State);
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<surface_shader_layer,
				k3d::interface_list<k3d::ri::isurface_shader,
				k3d::interface_list<k3d::aqsis::isurface_layer> > > factory(
			k3d::classes::AqsisSurfaceShaderLayer(),
			"AqsisSurfaceShaderLayer",
			"Encapsulates an Aqsis surface shader layer",
			"Aqsis",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// layer factories

k3d::iplugin_factory& displacement_shader_layer_factory()
{
	return displacement_shader_layer::get_factory();
}

k3d::iplugin_factory& surface_shader_layer_factory()
{
	return surface_shader_layer::get_factory();
}

} // namespace aqsis

} // namespace module


