#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/typed_array.h>
#include <k3dsdk/vectors.h>

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

template<typename T>
void test_shared_array(const T& Value)
{
	const std::string type_name = typeid(T).name();

	boost::shared_ptr<T> a(new T(Value));
	boost::shared_ptr<T> b = a;
	if(*a != *b)
		throw std::runtime_error(type_name + ": value not shared");
	if(a.get() != b.get())
		throw std::runtime_error(type_name + ": storage not shared");

	k3d::make_unique(b);
	if(*a != *b)
		throw std::runtime_error(type_name + ": value not copied");
	if(a.get() == b.get())
		throw std::runtime_error(type_name + ": storage not unique");
}

int main(int argc, char* argv[])
{
	try
	{
		test_shared_array<const k3d::typed_array<int> >(k3d::typed_array<int>(2, 3));
		test_shared_array<const k3d::typed_array<double> >(k3d::typed_array<double>(3, 4.3));
		test_shared_array<const k3d::typed_array<std::string> >(k3d::typed_array<std::string>(4, "foobar"));
		test_shared_array<const k3d::typed_array<k3d::point3> >(k3d::typed_array<k3d::point3>(5, k3d::point3(1, 2, 3)));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}

