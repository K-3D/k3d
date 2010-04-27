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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/options.h>
#include <k3dsdk/qtui/file_dialog.h>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// file_dialog

const k3d::filesystem::path file_dialog::get_open_filename(QWidget* parent, const QString& caption, const QString& type, const QString& filter, QString* selectedFilter, QFileDialog::Options options)
{
	const k3d::filesystem::path default_path = k3d::options::get_path(type.toStdString());
	const k3d::filesystem::path result = k3d::filesystem::native_path(k3d::ustring::from_utf8(QFileDialog::getOpenFileName(parent, caption, default_path.native_filesystem_string().c_str(), filter, selectedFilter, options).toStdString()));
	if(!result.empty())
		k3d::options::set_path(type.toStdString(), result);

	return result;
}

const k3d::filesystem::path file_dialog::get_save_filename(QWidget* parent, const QString& caption, const QString& type, const QString& filter, QString* selectedFilter, QFileDialog::Options options)
{
	const k3d::filesystem::path default_path = k3d::options::get_path(type.toStdString());
	const k3d::filesystem::path result = k3d::filesystem::native_path(k3d::ustring::from_utf8(QFileDialog::getSaveFileName(parent, caption, default_path.native_filesystem_string().c_str(), filter, selectedFilter, options).toStdString()));
	if(!result.empty())
		k3d::options::set_path(type.toStdString(), result);

	return result;
}

} // namespace qtui

} // namespace k3d

