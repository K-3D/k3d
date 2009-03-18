// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements the k3d-sl2xml command-line application, which converts Pixar RenderMan Shading Language to XML on stdout
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-version-config.h>

#include <k3dsdk/sl.h>

#include <iostream>
#include <fstream>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// main

int main(int argc, char* argv[])
{
	// Keep track of command line arguments that aren't options ...
	std::vector<std::string> files;

	// Look for command-line options ...
	for(int i = 1; i < argc; ++i)
	{
		const std::string argument = argv[i];

		if(argument == "--help" || argument == "-h")
		{
			std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
			std::cout << std::endl;
			std::cout << "  -h, --help         prints this help information and exits" << std::endl;
			std::cout << "      --version      prints program version information and exits" << std::endl;
			std::cout << std::endl;
			std::cout << "Note: input on stdin MUST be PREPROCESSED Shading Language source code." << std::endl;
			std::cout << "Recommend you run this command with:" << std::endl;
			std::cout << std::endl;
			std::cout << "     gcc -E -P -x c <file> | sl2xml" << std::endl;
			std::cout << std::endl;
			return 0;
		}
		else if(argument == "--version")
		{
			std::cout << argv[0] << " version " << K3D_VERSION << std::endl;
			std::cout << K3D_COPYRIGHT << "  See the AUTHORS file for contributors." << std::endl;
			std::cout << "Licensed under the GNU General Public License.  See the COPYING file for details." << std::endl;
			std::cout << "K-3D Home Page: http://www.k-3d.com" << std::endl;
			return 0;
		}
	}

	const k3d::sl::shaders_t shaders = k3d::sl::parse_source(std::cin, k3d::filesystem::generic_path("stdin"));
	k3d::sl::generate_metafile(std::cout, shaders);

	return 0;
}


