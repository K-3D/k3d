#ifndef K3DSDK_MESH_SELECTION_H
#define K3DSDK_MESH_SELECTION_H

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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "mesh.h"
#include "selection.h"
#include "serialization_xml.h"
#include "types.h"
#include "xml.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <map>

namespace k3d
{

namespace legacy { class mesh; }

/////////////////////////////////////////////////////////////////////////////
// mesh_selection

/// Stores a collection of selected mesh components
class mesh_selection
{
public:
	/** \name Deprecated for generic mesh primitives */
	//@{
	/// Stores selection data that will apply to a range of components
	struct record
	{
		uint_t begin;
		uint_t end;
		double weight;

		record(const uint_t Begin, const double Weight) :
			begin(Begin),
			end(Begin + 1),
			weight(Weight)
		{
		}
		
		record(const uint_t Begin, const uint_t End, const double Weight) :
			begin(Begin),
			end(End),
			weight(Weight)
		{
		}

		bool operator<(const record& RHS) const
		{
		    return begin < RHS.begin;
		}

		bool operator==(const record& RHS) const
		{
			return begin == RHS.begin && end == RHS.end && weight == RHS.weight;
		}

		bool operator!=(const record& RHS) const
		{
			return !(operator==(RHS));
		}
	};

	/// Stores a set of selection ranges, where the map key is the beginning of the range, and the map value contains the end
	typedef std::vector<record> records_t;
	static const records_t component_select_all();
	static const records_t component_deselect_all();

	records_t points;
	records_t edges;
	records_t faces;
	records_t nurbs_patches;
	//@}


	/// Constructs a mesh selection that explicitly selects everything
	static const mesh_selection select_all();
	/// Constructs a mesh selection that explicitly deselects everything
	static const mesh_selection deselect_all();
	/// Constructs a mesh selection that explicitly doesn't alter selection at all
	static const mesh_selection select_null();

	/// Copies the selection state of a mesh into a mesh_selection.
	static void store(const mesh& Mesh, mesh_selection& Selection);
	/// Copies the selection state of a mesh into a mesh_selection
	static void store(const legacy::mesh& Mesh, mesh_selection& MeshSelection);

	/// Merges a mesh_selection with the current selection state in the given mesh.
	static void merge(const mesh_selection& MeshSelection, mesh& Mesh);
	/// Merges a set of mesh_selection records with the current selection state in the given array.
	static void merge(const mesh_selection::records_t& Records, mesh::selection_t& Selection);
	/// Copies mesh_selection state over any previous selection state in the given mesh
	static void merge(const mesh_selection& MeshSelection, legacy::mesh& Mesh);

	struct component
	{
		component();
		component(const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const selection::type Type);
		component(const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const selection::type Type, const uint_t IndexBegin, const uint_t IndexEnd, const double_t Weight);

		void add_range(const uint_t IndexBegin, const uint_t IndexEnd, const double_t Weight);
		void clear();
		const bool_t empty() const;

		uint_t primitive_begin;
		uint_t primitive_end;
		selection::type type;

		std::vector<uint_t> index_begin;
		std::vector<uint_t> index_end;
		std::vector<double_t> weight;

		bool operator==(const component& RHS) const;
	};

	/// Adds a new component to the selection.
	void add_component(const component& Component);
	/// Clears the selection.
	void clear();
	/// Returns true if the selection is "empty", i.e. none of the components are selected
	bool empty() const;
	/// Equivalence
	bool operator==(const mesh_selection& RHS) const;
	/// Non-equivalence
	bool operator!=(const mesh_selection& RHS) const;

	typedef std::vector<component> components_t;
	components_t components;
};

std::ostream& operator<<(std::ostream&, const mesh_selection::records_t&);
std::ostream& operator<<(std::ostream&, const mesh_selection&);
std::ostream& operator<<(std::ostream&, const mesh_selection::component&);

/////////////////////////////////////////////////////////////////////////////
// mesh_selection_serialization

/// k3d::data serialization policy for use with mesh selection data
template<typename value_t, class property_policy_t>
class mesh_selection_serialization :
	public property_policy_t,
	public ipersistent
{
	// This policy only works for data stored by-value
	BOOST_STATIC_ASSERT((!boost::is_pointer<value_t>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		xml::element& xml_property = Element.append(xml::element("property", xml::attribute("name", property_policy_t::name())));
		xml::save(property_policy_t::internal_value(), xml_property, Context);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		xml::load(property_policy_t::internal_value(), Element, Context);
	}

protected:
	template<typename init_t>
	mesh_selection_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.owner().enable_serialization(Init.name(), *this);
	}
};

} // namespace k3d

#endif // !K3DSDK_MESH_SELECTION_H

