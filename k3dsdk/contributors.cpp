
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

#include "contributors.h"

namespace k3d
{

contributor::contributor(const ustring& Name, const std::string& Description) :
	name(Name),
	description(Description)
{
}

const contributors_t& contributors()
{
	static contributors_t storage;
	if(storage.empty())
	{
		storage.push_back(contributor(ustring::from_utf8("Timothy M. Shead"), "K-3D Founder"));
		storage.push_back(contributor(ustring::from_utf8("Romain Behar"), "K-3D Co-Founder"));
		
		storage.push_back(contributor(ustring::from_utf8("Anders Dahnielson"), "Documentation, script engines, web design")); 
		storage.push_back(contributor(ustring::from_utf8("Andy Gill"), "Too many plugins and tools to count")); 
		storage.push_back(contributor(ustring::from_utf8("Bart Janssens"), "Subdivision surfaces, OpenGL hardware support, animation")); 
		storage.push_back(contributor(ustring::from_utf8("Daniel S. Matthews"), "Win32 build, icons, documentation, samples"));
		storage.push_back(contributor(ustring::from_utf8("David Mart\303\255nez Moreno"), "Debian build, portability enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Joaqu\303\255n Duo"), "User interface enhancements, documentation, web design")); 
		storage.push_back(contributor(ustring::from_utf8("Joe Crawford"), "User interface design")); 
		storage.push_back(contributor(ustring::from_utf8("Jotham Ritorze"), "Logo design"));
		storage.push_back(contributor(ustring::from_utf8("Kirstyn Amanda \"GeekGirl\" Fox"), "Win32 installer")); 
		storage.push_back(contributor(ustring::from_utf8("Paul Gregory"), "Win32 build, subdivision surfaces, bug fixes"));
		storage.push_back(contributor(ustring::from_utf8("Rene Jensen"), "Icon design")); 
		
		storage.push_back(contributor(ustring::from_utf8("Adam Hupp"), "Python script engine enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Adam Sakareassen"), ".X file format writer")); 
		storage.push_back(contributor(ustring::from_utf8("Alexander Leidinger"), "BSD build")); 
		storage.push_back(contributor(ustring::from_utf8("Alper Ersoy"), "Web design"));
		storage.push_back(contributor(ustring::from_utf8("Anders Stenberg"), "Win32/MSVC build"));
		storage.push_back(contributor(ustring::from_utf8("Andreas Haferburg"), "Win32 build")); 
		storage.push_back(contributor(ustring::from_utf8("Anonymous"), "Bugfixes")); 
		storage.push_back(contributor(ustring::from_utf8("Anonymous"), "Mandrake 9 RPMs"));
		storage.push_back(contributor(ustring::from_utf8("Ben Campbell"), "3DS file format reader")); 
		storage.push_back(contributor(ustring::from_utf8("Brett W. McCoy"), "Configuration, ImageMagick plugins")); 
		storage.push_back(contributor(ustring::from_utf8("Chris Funke"), "Suse 8.0 build")); 
		storage.push_back(contributor(ustring::from_utf8("Damien Pollet"), "MacOSX build")); 
		storage.push_back(contributor(ustring::from_utf8("Dan Erikson"), "User interface enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Daniel MacDonald"), "PPC build")); 
		storage.push_back(contributor(ustring::from_utf8("Dave Crane"), "Configuration and JavaScript enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Dave Wallace"), "GCC 3.0 fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Denis Leroy"), "GCC 4.1, Python 2.5 fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Diego Petteno"), "NLS fixes, enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Ed Millard"), "User interface fixes, prototype pipeline viewer")); 
		storage.push_back(contributor(ustring::from_utf8("Francisco Andrades Grassi"), "Startup script improvements")); 
		storage.push_back(contributor(ustring::from_utf8("Fred Labrosse"), "RedHat RPM packages")); 
		storage.push_back(contributor(ustring::from_utf8("German Gomez Garcia"), "Shader cache enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Gilles J. Seguin"), "Bugfixes")); 
		storage.push_back(contributor(ustring::from_utf8("Giuseppe Zompatori"), "RenderMan enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Gregory Junker"), "Shader compilation fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Ilya Volynets"), "SGI IRIX build, GCC 3.1 fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Joal Heagney"), "Gallery contributions")); 
		storage.push_back(contributor(ustring::from_utf8("Joshua Moline"), "Configuration and portability enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Kevin McBride"), "Configure/install fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Louis-Dominique Dubeau"), "Bugfixes and enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Lukas Schroeder"), "Configuration")); 
		storage.push_back(contributor(ustring::from_utf8("Manuel Bastioni"), "Authored the first K-3D fan page")); 
		storage.push_back(contributor(ustring::from_utf8("Marco Amato from Udine"), "Wrote first Italian tutorial")); 
		storage.push_back(contributor(ustring::from_utf8("Patrick Mauritz"), "Plugin build enhancements")); 
		storage.push_back(contributor(ustring::from_utf8("Peter Balon"), "GCC 3.3 fixes")); 
		storage.push_back(contributor(ustring::from_utf8("Tom Browder"), "GNU Triangulated Surface support")); 
		storage.push_back(contributor(ustring::from_utf8("Wladyslaw Strugala"), "Win32 build and support")); 
	}

	return storage;
}

} // namespace k3d

