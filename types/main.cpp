#include <k3dsdk/types.h>

#include <boost/format.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#if defined(__GNUC__) &&  __GNUC__ >= 3

	#define K3D_HAVE_GCC_DEMANGLE

	// http://lists.debian.org/debian-gcc/2003/09/msg00055.html notes
	// that, in cxxabi.h of gcc-3.x for x < 4, this type is used before it
	// is declared.

	#if __GNUC__ == 3 && __GNUC_MINOR__ < 4
		class __class_type_info;
	#endif

	#include <cxxabi.h>
#endif

const std::string demangle(const std::string& Type)
{
	std::string result = Type;

#ifdef K3D_HAVE_GCC_DEMANGLE

	int status = 0;
	char* const temp = ::abi::__cxa_demangle(Type.c_str(), 0, 0, &status);
	if(temp && (status == 0))
		result = temp;
	if(temp)
		::free(temp);

#endif // K3D_HAVE_GCC_DEMANGLE

	return result;
}

boost::format format("%1% %|30t|%2% %|55t|%3%");

void print_headers()
{
	std::cout << format % "type" % "actual type" % "size" << "\n";
}

void print_divider()
{
	std::cout << format % "----" % "-----------" % "----" << "\n";
}

template<typename T>
void print_type(const std::string& Type)
{
	std::ostringstream actual_type;
	actual_type << demangle(typeid(T).name());

	std::ostringstream actual_size;
	actual_size << sizeof(T);

	std::cout << format % Type % actual_type.str() % actual_size.str() << "\n";
}

int main(int argc, char* argv[])
{
	print_headers();
	print_divider();

	print_type<bool>("bool");
	print_type<half>("half");
	print_type<float>("float");
	print_type<double>("double");
	print_divider();

	print_type<char>("char");
	print_type<unsigned char>("unsigned char");
	print_type<int>("int");
	print_type<unsigned int>("unsigned int");
	print_type<long>("long");
	print_type<unsigned long>("unsigned long");
	print_divider();

	print_type<size_t>("size_t");
	print_type<std::vector<int>::size_type>("std::vector<int>::size_type");
	print_divider();

	print_type<k3d::bool_t>("k3d::bool_t");
	print_type<k3d::half_t>("k3d::half_t");
	print_type<k3d::float_t>("k3d::float_t");
	print_type<k3d::double_t>("k3d::double_t");
	print_divider();

	print_type<k3d::int8_t>("k3d::int8_t");
	print_type<k3d::int16_t>("k3d::int16_t");
	print_type<k3d::int32_t>("k3d::int32_t");
	print_type<k3d::int64_t>("k3d::int64_t");
	print_type<k3d::uint8_t>("k3d::uint8_t");
	print_type<k3d::uint16_t>("k3d::uint16_t");
	print_type<k3d::uint32_t>("k3d::uint32_t");
	print_type<k3d::uint64_t>("k3d::uint64_t");

	return 0;
}

