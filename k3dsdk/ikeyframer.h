#ifndef IKEYFRAMER_H_
#define IKEYFRAMER_H_

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
		\brief Abstract interface for objects that set keyframes
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "iproperty.h"

namespace k3d
{

/// Abstract interface for objects that set keyframes.
class ikeyframer
{
public:
	
	/// List of the available keys
	typedef std::list<iproperty*> keys_t;

	/// Set a keyframe. Inputs are implementation-dependent, and may come from properties.
	virtual void keyframe() = 0;
	
	/// Delete the keyframe placed at the given time property
	virtual void delete_key(iproperty* TimeProperty) = 0;
	
	/// Get the property that inputs key values
	virtual iproperty& input_property() = 0;
	
	/// List with all the properties containing keytimes
	virtual keys_t get_keys() = 0;
	
	virtual ~ikeyframer() {}
};

}

#endif /*IKEYFRAMER_H_*/
