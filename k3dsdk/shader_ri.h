#ifndef K3DSDK_SHADER_RI_H
#define K3DSDK_SHADER_RI_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "node.h"
#include "node_change_signal.h"
#include "persistent.h"
#include "sl.h"
#include "types_ri.h"
#include "user_property_changed_signal.h"

namespace k3d
{

namespace ri
{

class render_state;

/////////////////////////////////////////////////////////////////////////////
// shader

/// Abstract base class that provides most of the boilerplate for a RenderMan shader instance
class shader :
	public persistent<node>,
	public node_change_signal<shader>
{
	typedef persistent<node> base;

public:
	void load(xml::element& Element, const ipersistent::load_context& Context);

protected:
	shader(iplugin_factory& Factory, idocument& Document, const sl::shader::type_t ShaderType);
	virtual ~shader();

	const path shader_path();
	const std::string shader_name();
	parameter_list shader_arguments(const render_state& State);

private:
	void on_shader_changed(iunknown*);
	void delete_arguments();
	void load_metafile();
	void create_arguments();

	const sl::shader::type_t m_shader_type;
	k3d_data(filesystem::path, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_shader_path;
	k3d_data(sl::shader, no_name, no_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) m_shader;

	sigc::connection m_shader_connection;

	user_property_changed_signal m_user_property_changed_signal;
};

} // namespace ri

} // namespace k3d

#endif // K3DSDK_SHADER_RI_H

