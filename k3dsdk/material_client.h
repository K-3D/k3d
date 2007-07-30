#ifndef K3DSDK_MATERIAL_CLIENT_H
#define K3DSDK_MATERIAL_CLIENT_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "k3d-i18n-config.h"
#include "imaterial.h"
#include "imaterial_client.h"
#include "material.h"

#ifdef	interface
#undef	interface
#endif	// interface

namespace k3d
{

// Forward declarations
class idocument;

/// Adds a boilerplate implementation of k3d::imaterial_client to a base class, using the parameterized-inheritance idiom
template<typename base_t>
class material_client :
	public base_t,
	public imaterial_client
{
public:
	material_client(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_material(init_owner(*this) + init_name("material") + init_label(_("Surface Material")) + init_description(_("Surface material")) + init_value(k3d::default_material(Document)))
	{
	}

	imaterial* material()
	{
		return m_material.pipeline_value();
	}
	
	void set_material(imaterial* const Material)
	{
		m_material.set_value(Material);
	}

protected:
	k3d_data(imaterial*, data::immutable_name, data::change_signal, data::with_undo, data::node_storage, data::no_constraint, data::node_property, data::node_serialization) m_material;
};

} // namespace k3d

#endif // K3DSDK_MATERIAL_CLIENT_H

