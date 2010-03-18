#include <fenv.h>
#include <iostream>

int main(int, char*[])
{
	std::cerr << "<DartMeasurement name=\"FE_DOWNWARD\" type=\"text/string\">" << FE_DOWNWARD << "</DartMeasurement>\n";
	std::cerr << "<DartMeasurement name=\"FE_TONEAREST\" type=\"text/string\">" << FE_TONEAREST << "</DartMeasurement>\n";
	std::cerr << "<DartMeasurement name=\"FE_TOWARDZERO\" type=\"text/string\">" << FE_TOWARDZERO << "</DartMeasurement>\n";
	std::cerr << "<DartMeasurement name=\"FE_UPWARD\" type=\"text/string\">" << FE_UPWARD << "</DartMeasurement>\n";
	std::cerr << "<DartMeasurement name=\"fegetround\" type=\"text/string\">" << fegetround() << "</DartMeasurement>\n";

	return 0;
}

