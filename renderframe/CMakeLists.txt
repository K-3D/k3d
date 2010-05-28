PROJECT(renderframe)

INCLUDE_DIRECTORIES(
	${k3d_SOURCE_DIR}
	${k3dsdk_BINARY_DIR}
	${Boost_INCLUDE_DIRS}
	${K3D_GLIBMM_INCLUDE_DIRS}
	${K3D_SIGC_INCLUDE_DIRS}
	)

LINK_DIRECTORIES(
	${K3D_GLIBMM_LIB_DIRS}
	${K3D_SIGC_LIB_DIRS}
	)

IF(WIN32 AND K3D_COMPILER_GCC)
	# For some reason, building with optimizations enabled causes link problems with boost::regex and auto-import
	SET_SOURCE_FILES_PROPERTIES(main.cpp PROPERTIES COMPILE_FLAGS -O0)
ENDIF()

ADD_EXECUTABLE(k3d-renderframe
	main.cpp
	)
TARGET_LINK_LIBRARIES(k3d-renderframe
	k3dsdk
	${Boost_REGEX_LIBRARY}
	)
IF(WIN32 AND K3D_COMPILER_GCC)
	SET_TARGET_PROPERTIES(k3d-renderframe PROPERTIES LINK_FLAGS -mwindows)
ENDIF()

INSTALL(TARGETS k3d-renderframe DESTINATION bin)

