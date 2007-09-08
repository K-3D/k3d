#include <k3dsdk/types.h>

#include <boost/format.hpp>

#include <iomanip>
#include <iostream>
#include <stdexcept>

void print_headers()
{
	boost::format format("%1% %|20t|%2% %|40t|%3% %|60t|%4%\n\n");
	std::cout << format % "expected name" % "typeid name" % "expected size" % "actual size";
}

template<typename T>
void test_type(const std::string& ExpectedName, const size_t ExpectedSize, bool& Error)
{
	boost::format format("%1% %|20t|%2% %|40t|%3% %|60t|%4%");
	std::cout << format % ExpectedName % k3d::demangle(typeid(T)) % ExpectedSize % sizeof(T);

	if(ExpectedName != k3d::demangle(typeid(T)) || ExpectedSize != sizeof(T))
	{
		std::cout << "********";
		Error = true;
	}

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

