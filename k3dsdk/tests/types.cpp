#include <k3dsdk/types.h>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

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

		test_type<bool>("bool", 1, error);
		test_type<char>("char", 1, error);
		test_type<unsigned char>("unsigned char", 1, error);
		test_type<int>("int", 4, error);
		test_type<unsigned int>("unsigned int", 4, error);
		test_type<long>("long", 4, error);
		test_type<unsigned long>("unsigned long", 4, error);
		test_type<size_t>("size_t", 4, error);
		test_type<float>("float", 4, error);
		test_type<double>("double", 8, error);

		test_type<boost::int8_t>("boost::int8_t", 1, error);
		test_type<boost::int16_t>("boost::int16_t", 2, error);
		test_type<boost::int32_t>("boost::int32_t", 4, error);
		test_type<boost::int64_t>("boost::int64_t", 8, error);
		test_type<boost::uint8_t>("boost::uint8_t", 1, error);
		test_type<boost::uint16_t>("boost::uint16_t", 2, error);
		test_type<boost::uint32_t>("boost::uint32_t", 4, error);
		test_type<boost::uint64_t>("boost::uint64_t", 8, error);

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

