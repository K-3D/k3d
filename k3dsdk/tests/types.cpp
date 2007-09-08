#include <k3dsdk/types.h>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace k3d
{

typedef bool bool_t;
typedef float float_t;
typedef double double_t;
typedef boost::int8_t int8_t;
typedef boost::int16_t int16_t;
typedef boost::int32_t int32_t;
typedef boost::int64_t int64_t;
typedef boost::uint8_t uint8_t;
typedef boost::uint16_t uint16_t;
typedef boost::uint32_t uint32_t;
typedef boost::uint64_t uint64_t;

} // namespace k3d

boost::format format("%1% %|20t|%2% %|45t|%3% %|55t|%4%");

void print_headers()
{
	std::cout << format % "type name" % "typeid name" % "expected" % "actual" << "\n\n";
}

template<typename T>
void test_type(const std::string& TypeName, const size_t ExpectedSize, bool& Error)
{
	std::ostringstream typeid_name;
	if(TypeName != k3d::demangle(typeid(T)))
		typeid_name << "* ";
	typeid_name << k3d::demangle(typeid(T));

	std::ostringstream actual_size;
	if(ExpectedSize != sizeof(T))
		actual_size << "* ";
	actual_size << sizeof(T);

	std::cout << format % TypeName % typeid_name.str() % ExpectedSize % actual_size.str();

	if(ExpectedSize != sizeof(T))
		Error = true;

	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	print_headers();

	try
	{
		bool error = false;

		test_type<k3d::bool_t>("k3d::bool_t", 1, error);
		test_type<k3d::float_t>("k3d::float_t", 4, error);
		test_type<k3d::double_t>("k3d::double_t", 8, error);

		test_type<k3d::int8_t>("k3d::int8_t", 1, error);
		test_type<k3d::int16_t>("k3d::int16_t", 2, error);
		test_type<k3d::int32_t>("k3d::int32_t", 4, error);
		test_type<k3d::int64_t>("k3d::int64_t", 8, error);
		test_type<k3d::uint8_t>("k3d::uint8_t", 1, error);
		test_type<k3d::uint16_t>("k3d::uint16_t", 2, error);
		test_type<k3d::uint32_t>("k3d::uint32_t", 4, error);
		test_type<k3d::uint64_t>("k3d::uint64_t", 8, error);

		if(error)
			throw std::runtime_error("type mismatch");

		return 0;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}

