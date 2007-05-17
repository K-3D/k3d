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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>

namespace libk3ddevelopment
{

GLhandleARB compile_shader(const GLenum ShaderType, const std::string& ShaderSource)
{
	GLhandleARB shader_object = glCreateShaderObjectARB(ShaderType);
	return_val_if_fail(shader_object, 0);

	const GLcharARB* source = ShaderSource.c_str();
	glShaderSourceARB(shader_object, 1, &source, 0);
	glCompileShaderARB(shader_object);

	int compile_status = 0;
	glGetObjectParameterivARB(shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &compile_status);
	k3d::log() << debug << "compile status: " << compile_status << std::endl;

	int log_length = 0;
	glGetObjectParameterivARB(shader_object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_length);
	std::string log(log_length, ' ');
	glGetInfoLogARB(shader_object, log_length, 0, const_cast<char*>(log.data()));

	k3d::log() << debug << "compile log: " << log << std::endl;

	return shader_object;
}

GLhandleARB link_shader(const GLhandleARB VertexObject, const GLhandleARB FragmentObject)
{
	GLhandleARB program_object = glCreateProgramObjectARB();
	return_val_if_fail(program_object, 0);

	glAttachObjectARB(program_object, VertexObject);
	glAttachObjectARB(program_object, FragmentObject);

	glLinkProgramARB(program_object);

	int link_status = 0;
	glGetObjectParameterivARB(program_object, GL_OBJECT_LINK_STATUS_ARB, &link_status);
	k3d::log() << debug << "link status: " << link_status << std::endl;

	int log_length = 0;
	glGetObjectParameterivARB(program_object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_length);
	std::string log(log_length, ' ');
	glGetInfoLogARB(program_object, log_length, 0, const_cast<char*>(log.data()));

	k3d::log() << debug << "link log: " << log << std::endl;

	return program_object;
}

/////////////////////////////////////////////////////////////////////////////
// glsl_painter

class glsl_painter :
	public k3d::persistent<k3d::node>,
	public k3d::gl::imesh_painter
{
	typedef k3d::persistent<k3d::node> base;

public:
	glsl_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_vertex_shader(init_owner(*this) + init_name("vertex_shader") + init_label(_("Vertex Shader")) + init_description(_("Vertex shader source code")) + init_value<std::string>("")),
		m_fragment_shader(init_owner(*this) + init_name("fragment_shader") + init_label(_("Fragment Shader")) + init_description(_("Fragment shader source code")) + init_value<std::string>("")),
		m_program_object(0)
	{
		m_vertex_shader.changed_signal().connect(sigc::mem_fun(*this, &glsl_painter::on_shader_source_changed));
		m_fragment_shader.changed_signal().connect(sigc::mem_fun(*this, &glsl_painter::on_shader_source_changed));
	}

	~glsl_painter()
	{
		if(m_program_object)
			glDeleteObjectARB(m_program_object);
	}

	void on_shader_source_changed(k3d::iunknown* Hint)
	{
		if(m_program_object)
		{
			glDeleteObjectARB(m_program_object);
			m_program_object = 0;
		}
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!m_program_object)
		{
			const GLhandleARB vertex_shader_object = compile_shader(GL_VERTEX_SHADER_ARB, m_vertex_shader.value());
			const GLhandleARB fragment_shader_object = compile_shader(GL_FRAGMENT_SHADER_ARB, m_fragment_shader.value());
			m_program_object = link_shader(vertex_shader_object, fragment_shader_object);

			if(fragment_shader_object)
				glDeleteObjectARB(fragment_shader_object);
			if(vertex_shader_object)
				glDeleteObjectARB(vertex_shader_object);
		}

		glUseProgramObjectARB(m_program_object);

		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->paint_mesh(Mesh, RenderState);
				}
			}
		}

		glUseProgramObjectARB(0);
	}
	
	void select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->select_mesh(Mesh, RenderState, SelectionState);
				}
			}
		}
	}
	
	void mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->mesh_changed(Mesh, Hint);
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<glsl_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x24d4d987, 0x62f849ea, 0xad1907fe, 0xe2a0a728),
			"GLSLPainter",
			_("Applies an OpenGL shader to downstream painters"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, script_property, with_serialization) m_vertex_shader;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, script_property, with_serialization) m_fragment_shader;
	GLhandleARB m_program_object;
};

/////////////////////////////////////////////////////////////////////////////
// glsl_painter_factory

k3d::iplugin_factory& glsl_painter_factory()
{
	return glsl_painter::get_factory();
}

} // namespace libk3ddevelopment

