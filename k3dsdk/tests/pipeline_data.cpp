#include <k3dsdk/pipeline_data.h>
#include <k3dsdk/typed_array.h>

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

int main(int argc, char* argv[])
{
	try
	{
		// Validate the initially-empty state ...
		k3d::pipeline_data<k3d::typed_array<k3d::int32_t> > source;
		test_expression(!source.get());
		test_expression(!source);
		test_expression(source.use_count() == 0);

		// Validate the state after creating an array ...
		k3d::typed_array<k3d::int32_t>& source_array = source.create();
		test_expression(source.get());
		test_expression(source);
		test_expression(source.use_count() == 1);

		// Put some data into the array ...
		source_array.push_back(1);
		source_array.push_back(2);
		source_array.push_back(4);

		// Confirm that read-write access to the array doesn't cause reallocation ...
		const k3d::typed_array<k3d::int32_t>* before = source.get();
		k3d::typed_array<k3d::int32_t>& mutable_source = source.writable();
		const k3d::typed_array<k3d::int32_t>* after = source.get();
		test_expression(before == after);
		test_expression(source.use_count() == 1);
	
		// Validate the state after creating a shared instance ...
		k3d::pipeline_data<k3d::typed_array<k3d::int32_t> > modifier(source);
		test_expression(modifier.get());
		test_expression(modifier);
		test_expression(source.use_count() == 2);
		test_expression(modifier.use_count() == 2);
		test_expression(source.get() == modifier.get());

		// Validate the state after creating a second shared instance ...
		k3d::pipeline_data<k3d::typed_array<k3d::int32_t> > sink(modifier);
		test_expression(sink.get());
		test_expression(sink);
		test_expression(source.use_count() == 3);
		test_expression(modifier.use_count() == 3);
		test_expression(sink.use_count() == 3);
		test_expression(source.get() == sink.get());
		test_expression(modifier.get() == sink.get());

		// Validate that read-write access from a shared instance reallocates ...
		k3d::typed_array<k3d::int32_t>& mutable_modifier = modifier.writable();
		test_expression(modifier.get());
		test_expression(modifier);
		test_expression(source.use_count() == 2);
		test_expression(modifier.use_count() == 1);
		test_expression(sink.use_count() == 2);
		test_expression(source.get() != modifier.get());
		test_expression(modifier.get() != sink.get());
		test_expression(source.get() == sink.get());
		test_expression(*source == *modifier);

		// Validate that assignment re-synchronizes the second and third shared instances ...
		sink = modifier;
		test_expression(sink.get());
		test_expression(sink);
		test_expression(source.use_count() == 1);
		test_expression(modifier.use_count() == 2);
		test_expression(sink.use_count() == 2);
		test_expression(source.get() != modifier.get());
		test_expression(modifier.get() == sink.get());
		test_expression(source.get() != sink.get());
		test_expression(*modifier == *sink);

		std::vector<k3d::pipeline_data<k3d::int32_t> > a;
		a.push_back(k3d::pipeline_data<k3d::int32_t>());
		a[0].create();
		a[0].writable() = 5;
		test_expression(*a[0] == 5);

		const std::vector<k3d::pipeline_data<k3d::int32_t> >& b = a;
		test_expression(*b[0] == 5);
	}
	catch(std::exception& e)
	{
		std::cerr << "uncaught exception: " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "unknown exception" << std::endl;
	}
	
	return 0;
}

