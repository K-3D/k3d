#include <k3dsdk/path.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

int main(int argc, char* argv[])
{
	try
	{
		if(argc != 4)
			throw std::runtime_error("Usage: relative_path [ABSOLUTE PATH] [REFERENCE PATH] [EXPECTED RESULT]");

		const k3d::filesystem::path absolute_path = k3d::filesystem::native_path(k3d::ustring::from_utf8(argv[1]));
		const k3d::filesystem::path reference_path = k3d::filesystem::native_path(k3d::ustring::from_utf8(argv[2]));
		const k3d::filesystem::path expected_result = k3d::filesystem::generic_path(k3d::ustring::from_utf8(argv[3]));

		const k3d::filesystem::path result =  k3d::filesystem::make_relative_path(absolute_path, reference_path);
		if(result != expected_result)
			throw std::runtime_error("expected [" + expected_result.native_console_string() + "] got [" + result.native_console_string() + "]");
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

