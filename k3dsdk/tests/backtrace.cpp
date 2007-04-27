#include <k3dsdk/log.h>
#include <iostream>

void foo()
{
	std::cout << k3d::backtrace << std::endl;
}

void bar(int)
{
	foo();
}

int main()
{
	bar(1);
	return 0;
}

