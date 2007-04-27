#include <k3dsdk/path.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

class quoted_string
{
public:
	quoted_string(const Glib::ustring& String) :
		string(String)
	{
	}

	friend std::ostream& operator<<(std::ostream& Stream, const quoted_string& String)
	{
		Stream << "[" << String.string.raw() << "]";
		return Stream;
	}

private:
	const Glib::ustring string;
};

void test_native_to_generic(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));
	std::cout << "native to generic: " << Native << " -> " << path.generic_utf8_string().raw() << std::endl;

	if(path.generic_utf8_string() != k3d::ustring::from_utf8(Generic))
		throw std::runtime_error("results don't match");
}

void test_iteration(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));

	std::ostringstream buffer;
	for(k3d::filesystem::path::iterator p = path.begin(); p != path.end(); ++p)
		buffer << (*p).raw() << ",";
//	std::copy(path.begin(), path.end(), std::ostream_iterator<Glib::ustring>(buffer, ","));

	std::cout << "path iteration: " << Native << " -> " << buffer.str() << std::endl;

	if(buffer.str() != Generic)
		throw std::runtime_error("results don't match");
}

void test_root_name(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));
	std::cout << "root name: " << Native << " -> " << path.root_name().raw() << std::endl;

	if(path.root_name() != k3d::ustring::from_utf8(Generic))
		throw std::runtime_error("results don't match");
}

void test_root_directory(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));
	std::cout << "root directory: " << Native << " -> " << path.root_directory().raw() << std::endl;

	if(path.root_directory() != k3d::ustring::from_utf8(Generic))
		throw std::runtime_error("results don't match");
}

void test_branch_path(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));
	std::cout << "branch path: " << Native << " -> " << path.branch_path().generic_utf8_string().raw() << std::endl;

	if(path.branch_path().generic_utf8_string() != k3d::ustring::from_utf8(Generic))
		throw std::runtime_error("results don't match");
}

void test_leaf(const std::string& Native, const std::string& Generic)
{
	k3d::filesystem::path path = k3d::filesystem::native_path(k3d::ustring::from_utf8(Native));
	std::cout << "path leaf: " << Native << " -> " << path.leaf().raw() << std::endl;

	if(path.leaf() != k3d::ustring::from_utf8(Generic))
		throw std::runtime_error("results don't match");
}

int main(int argc, char* argv[])
{
	try
	{
		test_native_to_generic("/", "/");
		test_native_to_generic("foo", "foo");
		test_native_to_generic("/foo", "/foo");
		test_native_to_generic("foo/bar", "foo/bar");
		test_native_to_generic("/foo/bar", "/foo/bar");
		test_native_to_generic(".", ".");
		test_native_to_generic("..", "..");
		test_native_to_generic("../foo", "../foo");
		test_native_to_generic("c:", "c:");
		test_native_to_generic("c:\\", "c:/");
		test_native_to_generic("c:..", "c:..");
		test_native_to_generic("c:foo", "c:foo");
		test_native_to_generic("c:\\foo", "c:/foo");
		test_native_to_generic("\\\\shr", "//shr");
		test_native_to_generic("\\\\shr\\", "//shr/");
		test_native_to_generic("\\\\shr\\foo", "//shr/foo");
		test_native_to_generic("prn:", "prn:");

		test_iteration("/", "/,");
		test_iteration("foo", "foo,");
		test_iteration("/foo", "/,foo,");
		test_iteration("foo/bar", "foo,bar,");
		test_iteration("/foo/bar", "/,foo,bar,");
		test_iteration("/foo/bar/baz", "/,foo,bar,baz,");
		test_iteration(".", ".,");
		test_iteration("..", "..,");
		test_iteration("../foo", "..,foo,");
		test_iteration("c:", "c:,");
		test_iteration("c:\\", "c:,/,");
		test_iteration("c:..", "c:,..,");
		test_iteration("c:foo", "c:,foo,");
		test_iteration("c:\\foo", "c:,/,foo,");
		test_iteration("c:\\foo\\bar", "c:,/,foo,bar,");
		test_iteration("\\\\shr", "//shr,");
		test_iteration("\\\\shr\\", "//shr,/,");
		test_iteration("\\\\shr\\foo", "//shr,/,foo,");
		test_iteration("\\\\shr\\foo\\bar", "//shr,/,foo,bar,");
		test_iteration("prn:", "prn:,");

		test_root_name("/", "");
		test_root_name("foo", "");
		test_root_name("/foo", "");
		test_root_name("foo/bar", "");
		test_root_name("/foo/bar", "");
		test_root_name(".", "");
		test_root_name("..", "");
		test_root_name("../foo", "");
		test_root_name("c:", "c:");
		test_root_name("c:\\", "c:");
		test_root_name("c:..", "c:");
		test_root_name("c:foo", "c:");
		test_root_name("c:\\foo", "c:");
		test_root_name("\\\\shr", "//shr");
		test_root_name("\\\\shr\\", "//shr");
		test_root_name("\\\\shr\\foo", "//shr");
		test_root_name("prn:", "prn:");

		test_root_directory("/", "/");
		test_root_directory("foo", "");
		test_root_directory("/foo", "/");
		test_root_directory("foo/bar", "");
		test_root_directory("/foo/bar", "/");
		test_root_directory(".", "");
		test_root_directory("..", "");
		test_root_directory("../foo", "");
		test_root_directory("c:", "");
		test_root_directory("c:\\", "/");
		test_root_directory("c:..", "");
		test_root_directory("c:foo", "");
		test_root_directory("c:\\foo", "/");
		test_root_directory("\\\\shr", "");
		test_root_directory("\\\\shr\\", "/");
		test_root_directory("\\\\shr\\foo", "/");
		test_root_directory("prn:", "");

		test_branch_path("/", "");
		test_branch_path("foo", "");
		test_branch_path("/foo", "/");
		test_branch_path("foo/bar", "foo");
		test_branch_path("/foo/bar", "/foo");
		test_branch_path(".", "");
		test_branch_path("..", "");
		test_branch_path("../foo", "..");
		test_branch_path("c:", "");
		test_branch_path("c:\\", "c:");
		test_branch_path("c:..", "c:");
		test_branch_path("c:foo", "c:");
		test_branch_path("c:\\foo", "c:/");
		test_branch_path("\\\\shr", "");
		test_branch_path("\\\\shr\\", "//shr");
		test_branch_path("\\\\shr\\foo", "//shr/");
		test_branch_path("prn:", "");

		test_leaf("/", "/");
		test_leaf("foo", "foo");
		test_leaf("/foo", "foo");
		test_leaf("foo/bar", "bar");
		test_leaf("/foo/bar", "bar");
		test_leaf(".", ".");
		test_leaf("..", "..");
		test_leaf("../foo", "foo");
		test_leaf("c:", "c:");
		test_leaf("c:\\", "/");
		test_leaf("c:..", "..");
		test_leaf("c:foo", "foo");
		test_leaf("c:\\foo", "foo");
		test_leaf("\\\\shr", "//shr");
		test_leaf("\\\\shr\\", "/");
		test_leaf("\\\\shr\\foo", "foo");
		test_leaf("prn:", "prn:");

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return 1;
	}
}
