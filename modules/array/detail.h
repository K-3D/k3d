#ifndef DETAIL_H
#define DETAIL_H

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

#include <k3dsdk/data.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/itransform_array_1d.h>
#include <k3dsdk/itransform_array_2d.h>
#include <k3dsdk/itransform_array_3d.h>
#include <k3dsdk/node_change_signal.h>
#include <k3dsdk/signal_slots.h>

namespace libk3darray
{

/////////////////////////////////////////////////////////////////////////////
// transform_array_1d

/// Boilerplate implementation of k3d::itransform_array_1d
template<typename base_t>
class transform_array_1d :
	public k3d::node_change_signal<base_t>,
	public k3d::itransform_array_1d
{
public:
	transform_array_1d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::node_change_signal<base_t>(Factory, Document),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_1d*>(0)),
		m_processing(false)
	{
		m_layout.changed_signal().connect(this->make_node_change_slot());
	}

	const k3d::matrix4 get_element(unsigned long Index1, unsigned long Count1)
	{
		k3d::matrix4 result = on_get_element(Index1, Count1);

		if(m_processing)
		{
			if(!Index1)
				k3d::log() << warning << "breaking array_1d infinite loop" << std::endl;
			return result;
		}

		m_processing = true;
		if(k3d::itransform_array_1d* const layout = m_layout.value())
			result = result * layout->get_element(Index1, Count1);
		m_processing = false;

		return result;
	}

private:
	virtual const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1) = 0;

	k3d_data(k3d::itransform_array_1d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;

	bool m_processing;
};

/////////////////////////////////////////////////////////////////////////////
// transform_array_2d

/// Boilerplate implementation of k3d::itransform_array_1d
template<typename base_t>
class transform_array_2d :
	public k3d::node_change_signal<base_t>,
	public k3d::itransform_array_2d
{
public:
	transform_array_2d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::node_change_signal<base_t>(Factory, Document),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_2d*>(0)),
		m_processing(false)
	{
		m_layout.changed_signal().connect(this->make_node_change_slot());
	}

	const k3d::matrix4 get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2)
	{
		k3d::matrix4 result = on_get_element(Index1, Count1, Index2, Count2);

		if(m_processing)
		{
			if(!Index1 && !Index2)
				k3d::log() << warning << "breaking array_2d infinite loop" << std::endl;
			return result;
		}

		m_processing = true;
		if(k3d::itransform_array_2d* const layout = m_layout.value())
			result = result * layout->get_element(Index1, Count1, Index2, Count2);
		m_processing = false;

		return result;
	}

private:
	virtual const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2) = 0;

	k3d_data(k3d::itransform_array_2d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;

	bool m_processing;
};

/////////////////////////////////////////////////////////////////////////////
// transform_array_3d

/// Boilerplate implementation of k3d::itransform_array_1d
template<typename base_t>
class transform_array_3d :
	public k3d::node_change_signal<base_t>,
	public k3d::itransform_array_3d
{
public:
	transform_array_3d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::node_change_signal<base_t>(Factory, Document),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_3d*>(0)),
		m_processing(false)
	{
		m_layout.changed_signal().connect(this->make_node_change_slot());
	}

	const k3d::matrix4 get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2, unsigned long Index3, unsigned long Count3)
	{
		k3d::matrix4 result = on_get_element(Index1, Count1, Index2, Count2, Index3, Count3);

		if(m_processing)
		{
			if(!Index1 && !Index2 && !Index3)
				k3d::log() << warning << "breaking array_3d infinite loop" << std::endl;
			return k3d::identity3D();
		}

		m_processing = true;
		if(k3d::itransform_array_3d* const layout = m_layout.value())
			result = result * layout->get_element(Index1, Count1, Index2, Count2, Index3, Count3);
		m_processing = false;

		return result;
	}

private:
	virtual const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2, unsigned long Index3, unsigned long Count3) = 0;

	k3d_data(k3d::itransform_array_3d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;

	bool m_processing;
};

} // namespace libk3darray

#endif // !DETAIL_H

