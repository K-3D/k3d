#include <k3dsdk/uuid.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#define test_expression(expression) \
	if(!(expression)) \
	{ \
		std::ostringstream buffer; \
		buffer << #expression << " failed at " << __FILE__ << ": " << __LINE__; \
		throw std::runtime_error(buffer.str()); \
	} \

static void test_serialization(const k3d::uuid& Value)
{
	std::stringstream buffer;
	buffer << Value;

	k3d::uuid value;
	buffer >> value;

	if(value != Value)
		throw std::runtime_error("serialized values differ");
}

int main(int argc, char* argv[])
{
	try
	{
		test_expression(k3d::uuid::null().is_nil());
		test_expression(k3d::uuid::null() == k3d::uuid(0, 0, 0, 0));
		test_expression(k3d::uuid(1, 2, 3, 3) != k3d::uuid(1, 2, 3, 4));
		test_expression(k3d::uuid(1, 2, 3, 3) < k3d::uuid(1, 2, 3, 4));

		test_serialization(k3d::uuid::null());
		test_serialization(k3d::uuid(1, 2, 3, 4));
		test_serialization(k3d::uuid::random());
	}
	catch(std::exception& e)
	{
		std::cerr << "uncaught exception: " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "unknown exception" << std::endl;
		return 1;
	}

	return 0;
}

