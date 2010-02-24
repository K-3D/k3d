#include <k3dsdk/string_cast.h>
#include <iostream>

/// Tests to ensure that floating-point numbers are converted to strings without loss of precision
int main(int argc, char* argv[])
{
	for(int i = 1; i < argc; ++i)
	{
		const std::string original = argv[i];
		const double value = k3d::from_string<double>(original, 0.0);

		std::ostringstream buffer;
		buffer << k3d::string_cast(value);

		if(original != buffer.str())
		{
			std::cerr << "string conversion failure, original: " << original << " result: " << buffer.str() << std::endl;
			return 1;
		}
	}

	return 0;
}

