#include <k3dsdk/mesh_selection.h>
#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		k3d::mesh_selection::records_t records;

		size_t begin;
		size_t end;
		double weight;

		const std::string expected_output = argv[1];

		for(int i = 0; i < argc - 2; ++i)
		{
			switch(i % 3)
			{
				case 0:
					begin = atoi(argv[i+2]);
					break;
				case 1:
					end = atoi(argv[i+2]);
					break;
				case 2:
					weight = atof(argv[i+2]);
					records.push_back(k3d::mesh_selection::record(begin, end, weight));
					break;
			}
		}

		std::ostringstream buffer;
		buffer << records;

		if(buffer.str() != expected_output)
			throw std::runtime_error("selection output [" + buffer.str() + "] does not match expected output [" + expected_output + "]"); 
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

