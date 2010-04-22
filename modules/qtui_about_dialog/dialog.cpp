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

#include <ui_dialog.h>

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/contributors.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/qtui/application_dialog.h>

#include <boost/assign/list_of.hpp>
#include <sstream>

namespace module
{

namespace qtui
{

namespace about
{

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Creates an "about" box that displays program version and copyright
class dialog :
	public k3d::qtui::application_dialog,
	public k3d::iunknown
{
public:
	dialog()
	{
		ui.setupUi(this);

		ui.version->setText("K-3D Version " K3D_VERSION);
		ui.copyright->setText(K3D_COPYRIGHT);

		std::ostringstream buffer;
		buffer << "<html>";
		buffer << "<body>";
		buffer << "<br/><br/><br/>";
		const k3d::contributors_t& contributors = k3d::contributors();
		for(k3d::contributors_t::const_iterator contributor = contributors.begin(); contributor != contributors.end(); ++contributor)
		{
			if(contributor->category.size())
			{
				buffer << "<br/><div align='center' style='color: #5555ff; font-size: 18px;'>" << contributor->category << "</div><br/><br/>";
			}
			else
			{
				buffer << "<div align='center' style='color: #000000; font-size: 16px; font-weight: bold;'>" << contributor->name.raw() << "</div>";
				buffer << "<div align='center' style='color: #555555; font-size: 10px;'>" << contributor->description << "</div><br/>";
			}
		}
		buffer << "</body>";
		buffer << "</html>";
		ui.contributors->setHtml(QString::fromUtf8(buffer.str().c_str()));

		this->setAttribute(Qt::WA_DeleteOnClose);
	}

	~dialog()
	{
k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0x9914fe3d, 0xf8402979, 0x22b7769e, 0xbeea9df1),
			"QTUIAboutDialog",
			_("Displays the current K-3D version and credits contributors"),
			"QTUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("qtui:component-type", "dialog"));

		return factory;
	}

	Ui::QTUIAboutDialog ui;
};

} // namespace about

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::about::dialog::get_factory());
K3D_MODULE_END

