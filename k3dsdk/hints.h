#ifndef K3DSDK_HINTS_H
#define K3DSDK_HINTS_H

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

#include "algebra.h"
#include "ihint.h"
#include "mesh.h"
#include "signal_system.h"

#include <boost/any.hpp>

#include <iosfwd>

namespace k3d
{

/// Namespace reserved for "hints" that pass metadata about an upstream change to downstream nodes
namespace hint
{

//////////////////////////////////////////////////////////////////////////////
// slot_t

/// iostream-compatible manipulator object that serializes information about a hint object
class print
{
public:
	print(ihint* Hint);

	ihint* const hint;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const print& RHS);

//////////////////////////////////////////////////////////////////////////////
// slot_t

/// Defines a standard slot that receives a hint
typedef sigc::slot<void, ihint*> slot_t;

//////////////////////////////////////////////////////////////////////////////
// bitmap_dimensions_changed

/// Hint passed to downstream nodes to indicate that the pixel dimensions of a bitmap have changed (and thus its internal memory).
class bitmap_dimensions_changed :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static bitmap_dimensions_changed* instance();
};

//////////////////////////////////////////////////////////////////////////////
// bitmap_pixels_changed

/// Hint passed to downstream nodes to indicate that the pixel values of a bitmap have changed (i.e. one-or-more pixels have changed color).
class bitmap_pixels_changed :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static bitmap_pixels_changed* instance();
};

//////////////////////////////////////////////////////////////////////////////
// selection_changed

/// Hint object that indicates that an object's selection state has changed
class selection_changed :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static selection_changed* instance();
};

//////////////////////////////////////////////////////////////////////////////
// mesh_geometry_changed

/// Hint object that indicates that a mesh's geometry (the locations of its points) has changed
class mesh_geometry_changed :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static mesh_geometry_changed* instance();

	/// Indices of the points affected by the change
	k3d::mesh::indices_t changed_points;
	/// Transformation matrix used for the change
	k3d::matrix4 transformation_matrix;
};

//////////////////////////////////////////////////////////////////////////////
// mesh_topology_changed

/// Hint object that indicates that a mesh's topology has changed
class mesh_topology_changed :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static mesh_topology_changed* instance();
};

//////////////////////////////////////////////////////////////////////////////
// mesh_deleted

/// Hint object that indicates a mesh was deleted
class mesh_deleted :
	public ihint
{
public:
	ihint* clone();
	void print(std::ostream& Stream);

	static mesh_deleted* instance();
};

//////////////////////////////////////////////////////////////////////////////
// any

/// Used when creating a hint-mapping that matches any incoming hint type.
class any
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// none

/// Used when creating a hint-mapping that maps to a NULL ("none") hint.
class none
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// unchanged

/// Used when creating a hint-mapping that does not convert hints.
class unchanged
{
public:
};

//////////////////////////////////////////////////////////////////////////////
// hint_traits

/// Hint traits template responsible for matching and converting hints.
template<typename HintT>
class hint_traits
{
public:
};

template<>
class hint_traits<bitmap_dimensions_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<bitmap_dimensions_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static bitmap_dimensions_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<bitmap_pixels_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<bitmap_pixels_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static bitmap_pixels_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<selection_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<selection_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static selection_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<mesh_topology_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<mesh_topology_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static mesh_topology_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<mesh_geometry_changed>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return dynamic_cast<mesh_geometry_changed*>(Hint);
	}

	static ihint* convert(ihint*)
	{
		static mesh_geometry_changed hint;
		return &hint;
	}
};

template<>
class hint_traits<any>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return true;
	}
};

template<>
class hint_traits<none>
{
public:
	static const bool_t match(ihint* Hint)
	{
		return false;
	}

	static ihint* convert(ihint*)
	{
		return 0;
	}
};

template<>
class hint_traits<unchanged>
{
public:
	static ihint* convert(ihint* Hint)
	{
		return Hint;
	}
};

//////////////////////////////////////////////////////////////////////////////
// last_conversion

class last_conversion
{
};

//////////////////////////////////////////////////////////////////////////////
// convert

template<typename SourceT, typename TargetT, typename NextT = last_conversion>
struct convert
{
	typedef SourceT Source;
	typedef TargetT Target;
	typedef NextT Next;
};

namespace detail
{

template<typename ListT>
void execute(ihint* const Hint, const slot_t& Slot)
{
	if(hint_traits<typename ListT::Source>::match(Hint))
	{
		Slot(hint_traits<typename ListT::Target>::convert(Hint));
	}
	else
	{
		execute<typename ListT::Next>(Hint, Slot);
	}
}

template<>
inline void execute<last_conversion>(ihint* const Hint, const slot_t& Slot)
{
	std::cerr << "unhandled hint: " << print(Hint) << std::endl;
}

template<typename ListT>
class converter
{
public:
	converter(const slot_t& Slot) :
		slot(Slot)
	{
	}

	void operator()(ihint* const Hint)
	{
		execute<ListT>(Hint, slot);
	}

private:
	slot_t slot;
};

} // namespace detail

//////////////////////////////////////////////////////////////////////////////
// converter

/// Factory function for creating hint-converter objects.  You can pass the
/// result from converter() to the connect() method of a signal, to establish
/// a hint-mapping.
template<typename ListT>
detail::converter<ListT> converter(const sigc::slot<void, ihint*>& Slot)
{
	return detail::converter<ListT>(Slot);
}

} // namespace hint

} // namespace k3d

#endif // !K3DSDK_HINTS_H

