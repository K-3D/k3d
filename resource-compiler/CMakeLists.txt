PROJECT(k3d-resource-compiler)

INCLUDE_DIRECTORIES(
	${Boost_INCLUDE_DIRS}
	)
ADD_EXECUTABLE(k3d-resource-compiler
	main.cpp
	)
TARGET_LINK_LIBRARIES(k3d-resource-compiler
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	)

