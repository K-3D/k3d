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

#include <k3d-i18n-config.h>
#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/fstream.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/socket.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/system.h>
#include <k3dsdk/utility.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/textview.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <sstream>
#include <vector>

namespace detail
{

typedef std::vector<std::string> string_array;

bool g_show_timestamps = true;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = false;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////
// main_window

class main_window :
	public Gtk::Window
{
public:
	main_window() :
		m_cancel(Gtk::Stock::CANCEL),
		m_send(_("Send"))
	{
		set_title(_("K-3D Bug Buddy"));
		set_border_width(6);

		Gtk::Label* const title = new Gtk::Label();
		title->set_markup(_("<span size=\"xx-large\" weight=\"heavy\">Welcome to K-3D Bug Buddy</span>"));

		Gtk::Label* const warning = new Gtk::Label(
			_(
			"K-3D Bug Buddy will help you generate a bug report - please complete the following fields,\n"
			"and provide an e-mail address where you can be reached."));

		Gtk::Label* const summary = new Gtk::Label();
		summary->set_markup(_("<span weight=\"bold\">Required</span> - enter a one-line summary of what happened:"));
		summary->set_alignment(Gtk::ALIGN_LEFT);

		Gtk::Label* const detail = new Gtk::Label();
		detail->set_markup(_("<span weight=\"bold\">Required</span> - describe what happened in detail:"));
		detail->set_alignment(Gtk::ALIGN_LEFT);

		Gtk::ScrolledWindow* const detail_window = new Gtk::ScrolledWindow();
		detail_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		detail_window->add(m_detail);

		Gtk::Label* const send_label = new Gtk::Label();
		send_label->set_markup(_("<span weight=\"bold\">Required</span> - enter your contact <span weight=\"bold\">e-mail</span> address:"));
		send_label->set_alignment(Gtk::ALIGN_LEFT);

		Gtk::HButtonBox* const send_buttons = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		send_buttons->pack_start(m_cancel, Gtk::PACK_SHRINK);
		send_buttons->pack_start(m_send, Gtk::PACK_SHRINK);

		Gtk::VBox* const vbox = new Gtk::VBox(false, 10);
		vbox->pack_start(*Gtk::manage(title), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(warning), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(summary), Gtk::PACK_SHRINK);
		vbox->pack_start(m_summary, Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(detail), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(detail_window), Gtk::PACK_EXPAND_WIDGET);

		vbox->pack_start(*Gtk::manage(send_label), Gtk::PACK_SHRINK);
		vbox->pack_start(m_contact, Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(send_buttons), Gtk::PACK_SHRINK);

		add(*Gtk::manage(vbox));

		m_detail.get_buffer()->set_text("Description of problem:\n\nSteps to reproduce problem:\n\n1. \n2. \n3. \n\nExpected results:\n\nActual results:\n\nHow often does this happen?\n\nAdditional information:\n");

		m_cancel.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_cancel));
		m_send.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_send_http));

		set_position(Gtk::WIN_POS_CENTER);
		show_all();
	}

private:
	void on_cancel()
	{
		Gtk::MessageDialog nag(*this, _("Are you sure?  The only way to get a bug fixed is to report it ..."), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
		nag.set_default_response(Gtk::RESPONSE_NO);

		if(Gtk::RESPONSE_YES == nag.run())
			hide();
	}

	bool ready_to_go()
	{
		if(!m_summary.get_text().size())
		{
			Gtk::MessageDialog nag(*this, _("Please include a summary for this bug report"), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
			nag.run();

			return false;
		}

		if(!m_contact.get_text().size())
		{
			Gtk::MessageDialog nag(*this, _("Please include a contact email address"), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
			nag.run();

			return false;
		}

		return true;
	}

	void on_send_http()
	{
		if(!ready_to_go())
			return;

		const std::string boundary = "0x6995453520214d4798bd1aa51991ae73";

		std::stringstream content;
		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"group_id\"\r\n";
		content << "\r\n";
		content << "11113\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"atid\"\r\n";
		content << "\r\n";
		content << "111113\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"func\"\r\n";
		content << "\r\n";
		content << "postadd\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"category_id\"\r\n";
		content << "\r\n";
		content << "608711\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"artifact_group_id\"\r\n";
		content << "\r\n";
		content << "630800\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"summary\"\r\n";
		content << "\r\n";
		content << m_summary.get_text() << "\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"details\"\r\n";
		content << "\r\n";
		content << "Contact: " << m_contact.get_text() << "\r\n";
		content << "Summary: " << m_summary.get_text() << "\r\n";
		content << "Report Generated: " << boost::posix_time::second_clock::universal_time() << " UTC\r\n";
		content << "Package: " << K3D_PACKAGE << "\r\n";
		content << "Version: " << K3D_VERSION << "\r\n";
		content << "Platform: " << K3D_HOST << "\r\n";
		content << "Compiler: " << K3D_COMPILER_NAME << "\r\n";
		content << "Build Time: " << __DATE__ << " " << __TIME__ << " local\r\n";
		content << m_detail.get_buffer()->get_text() << "\r\n";

		content << "--" << boundary << "\r\n";
		content << "Content-Disposition: form-data; name=\"submit\"\r\n";
		content << "\r\n";
		content << "SUBMIT\r\n";

		std::stringstream header;
		header << "POST /tracker/?func=add&group_id=11113&atid=111113 HTTP/1.1\r\n";
		header << "Host: sourceforge.net\r\n";
		header << "User-Agent: k3d-bug-buddy\r\n";
		header << "Connection: close\r\n";
		header << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";
		header << "Content-Length: " << content.str().size() << "\r\n";
		header << "\r\n";

		std::string results;
		try
		{
			k3d::socket::endpoint connection = k3d::socket::connect("sourceforge.net", 80);
			connection.write(header.str());
			connection.write(content.str());

			std::string buffer;
			while(true)
			{
				connection.read(buffer);
				results += buffer;
			}
		}
		catch(k3d::socket::closed& e)
		{
		}
		catch(std::exception& e)
		{
			transmission_error(e.what());
			return;
		}

		boost::match_results<std::string::iterator> match;
		boost::regex created_exp("Item Successfully Created.*atid=111113&amp;aid=([0-9]*)&amp;group_id=11113");
		if(!boost::regex_search(results.begin(), results.end(), match, created_exp))
		{
			transmission_error("Unknown server error");
			return;
		}
		const std::string aid = match[1].str();

		Gtk::MessageDialog nag(*this, k3d::string_cast(boost::format(_("Thanks!  Your bug report has been submitted.  You can track your report at:\n\nhttp://sourceforge.net/tracker/index.php?func=detail&aid=%1%&group_id=11113&atid=111113")) % aid), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
		nag.run();

		hide();
	}

	void transmission_error(const std::string& Message)
	{
		k3d::log() << error << Message << std::endl;

		Gtk::MessageDialog nag(*this, _("There was an error sending your report - please visit\n\nhttp://sourceforge.net/tracker/?group_id=11113&atid=111113\n\nand file a report manually instead."), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
		nag.run();
	}

	Gtk::Entry m_summary;
	Gtk::TextView m_detail;
	Gtk::Entry m_contact;
	Gtk::Button m_cancel;
	Gtk::Button m_send;
};

/////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(const std::string& Name, std::ostream& Stream)
{
	Stream << "usage: " << Name << " [options]" << std::endl;
	Stream << std::endl;
	Stream << "  -h, --help               prints this help information and exits" << std::endl;
	Stream << "      --version            prints program version information and exits" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// version

/// Prints version info
void print_version(std::ostream& Stream)
{
	Stream << "K-3D Bug Buddy Version " << K3D_VERSION << std::endl;
	Stream << K3D_COPYRIGHT << "  See the AUTHORS file for contributors." << std::endl;
	Stream << "Licensed by the GNU General Public License.  See the COPYING file for details." << std::endl;
	Stream << "K-3D Home Page: http://www.k-3d.org" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// setup_logging

/// Sets-up options for logging our output
void setup_logging(const std::string& ProcessName)
{
	k3d::log_show_timestamps(g_show_timestamps);
	k3d::log_set_tag(g_show_process ? "[" + ProcessName + "]" : std::string());
	k3d::log_color_level(g_color_level);
	k3d::log_show_level(true);
	k3d::log_syslog(g_syslog);
	k3d::log_minimum_level(g_minimum_log_level);
}

} // namespace detail

/// Program main
int main(int argc, char* argv[])
{
    try
    {
        const std::string program_name = k3d::filesystem::native_path(k3d::ustring::from_utf8(argv[0])).leaf().raw();

#ifdef K3D_HAVE_NLS
        setlocale(LC_ALL, "");
        bindtextdomain(PACKAGE, K3D_LOCALE);
        bind_textdomain_codeset(PACKAGE, "UTF-8");
        textdomain(PACKAGE);
#endif

        Gtk::Main main(&argc, &argv);

        // Put our command-line arguments in a more useable form ...
        detail::string_array options(&argv[1], &argv[argc]);

        // Print a "help" message ...
        if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
        {
            detail::usage(program_name, std::cout);
            return 0;
        }

        // Print version data ...
        if(std::count(options.begin(), options.end(), "--version"))
        {
            detail::print_version(std::cout);
            return 0;
        }

        // Setup logging right away ...
        detail::setup_logging(program_name);

        detail::main_window window;
        main.run(window);

        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unknown exception" << std::endl;
    }

    return 1;
}

