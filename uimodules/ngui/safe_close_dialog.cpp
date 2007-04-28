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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "safe_close_dialog.h"
#include "unsaved_document.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>

namespace libk3dngui
{

namespace safe_close_dialog
{

namespace detail
{

class unsaved_document_columns :
	public Gtk::TreeModelColumnRecord
{
public:
	unsaved_document_columns()
	{
		add(label);
		add(save);
	}

	Gtk::TreeModelColumn<Glib::ustring> label;
	Gtk::TreeModelColumn<bool> save;
};

} // namespace detail

int run(Gtk::Window& Parent, const std::string& Title)
{
	Gtk::MessageDialog dialog(Parent, "", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);

	dialog.set_message(k3d::string_cast(boost::format(_("Save the changes to document \"%1%\" before closing?")) % Title));
	dialog.set_secondary_text( _("If you don't save, changes will be permanently lost (no undo)."));

    Gtk::Button* const close_button = new Gtk::Button(_("Cl_ose without Saving"), true);
    close_button->show();

	dialog.add_action_widget(*Gtk::manage(close_button), Gtk::RESPONSE_CLOSE);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	dialog.set_default_response(Gtk::RESPONSE_OK);

    const int result = dialog.run();

	return result;
}

int run(Gtk::Window& Parent, entries_t& UnsavedDocuments)
{
	return_val_if_fail(UnsavedDocuments.size(), Gtk::RESPONSE_NONE);

	if(1 == UnsavedDocuments.size())
		return run(Parent, UnsavedDocuments.front().document->unsaved_document_title());

	Gtk::MessageDialog dialog(Parent, "", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);

	dialog.set_message(k3d::string_cast(boost::format(_("There are %1% documents with unsaved changes. Save changes before closing?")) % UnsavedDocuments.size()));
	dialog.set_secondary_text(_("Select the documents you want to save:"));

	detail::unsaved_document_columns columns;
	Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(columns);
	for(entries_t::iterator unsaved_document = UnsavedDocuments.begin(); unsaved_document != UnsavedDocuments.end(); ++unsaved_document)
	{
		Gtk::TreeRow row = *model->append();
		row[columns.label] = unsaved_document->document->unsaved_document_title();
		row[columns.save] = true;
	}

	Gtk::TreeView view(model);
	view.set_headers_visible(false);
	view.append_column_editable("Save", columns.save);
	view.append_column("Document", columns.label);

	Gtk::Frame frame;
	frame.set_shadow_type(Gtk::SHADOW_IN);
	frame.add(view);

	Gtk::Label label3(_("If you don't save, all your changes will be permanently lost (no undo)."), Gtk::ALIGN_LEFT);
	label3.set_selectable(true);

	std::vector<Gtk::Widget*> vbox_children = dialog.get_vbox()->get_children();
	return_val_if_fail(vbox_children.size(), Gtk::RESPONSE_NONE);

	Gtk::HBox* const hbox = dynamic_cast<Gtk::HBox*>(vbox_children.front());
	return_val_if_fail(hbox, Gtk::RESPONSE_NONE);

	std::vector<Gtk::Widget*> hbox_children = hbox->get_children();
	return_val_if_fail(hbox_children.size(), Gtk::RESPONSE_NONE);

	Gtk::VBox* const vbox = dynamic_cast<Gtk::VBox*>(hbox_children[1]);
	return_val_if_fail(vbox, Gtk::RESPONSE_NONE);

	vbox->pack_start(frame, Gtk::PACK_EXPAND_WIDGET);
	vbox->pack_start(label3, Gtk::PACK_SHRINK);

	dialog.get_vbox()->show_all();

    Gtk::Button* const close_button = new Gtk::Button(_("Cl_ose without Saving"), true);
    close_button->show();

	dialog.add_action_widget(*Gtk::manage(close_button), Gtk::RESPONSE_CLOSE);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	dialog.set_default_response(Gtk::RESPONSE_OK);

	const int result = dialog.run();

	return result;
}

} // namespace safe_close_dialog

} // namespace libk3dngui
