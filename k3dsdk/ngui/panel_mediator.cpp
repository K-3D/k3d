// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/ngui/panel_mediator.h>
#include <k3dsdk/signal_accumulators.h>
#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace ngui
{

namespace panel
{

/////////////////////////////////////////////////////////////////////////////
// mediator::implementation

class mediator::implementation
{
public:
	static implementation& instance(idocument& Document)
	{
		typedef std::map<idocument*, implementation*> cache_t;
		static cache_t cache;
		cache_t::iterator result = cache.find(&Document);
		if(result == cache.end())
			result = cache.insert(std::make_pair(&Document, new implementation(Document))).first;

		return *result->second;
	}

	idocument& document;

	sigc::signal1<bool_t, inode*, k3d::signal::consumable> node_focus_signal;

private:
	implementation(idocument& Document) :
		document(Document)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// mediator

mediator::mediator(idocument& Document) :
	internal(implementation::instance(Document))
{
}

void mediator::set_focus(inode& Node)
{
	internal.node_focus_signal.emit(&Node);
}

sigc::connection mediator::connect_focus_node_signal(const sigc::slot<bool_t, inode*>& Slot)
{
	return internal.node_focus_signal.connect(Slot);
}

} // namespace panel

} // namespace ngui

} // namespace k3d

