#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		for(int i = 1; i < argc; ++i)
		{
			std::cerr << "boost::math::isnormal(" << argv[i] << "): ";
			std::cerr << boost::math::isnormal(boost::lexical_cast<double>(argv[i])) << std::endl;
		}

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return 1;
	}
}

