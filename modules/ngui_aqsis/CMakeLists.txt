# Build Aqsis
INCLUDE(ExternalProject)
FIND_PROGRAM(GIT_COMMAND git)

ExternalProject_Add(aqsis
	DOWNLOAD_COMMAND
		git clone git://aqsis.git.sourceforge.net/gitroot/aqsis/aqsis
	CMAKE_GENERATOR
		${CMAKE_GENERATOR}
	CMAKE_ARGS
		-DBUILD_SHARED_LIBS:BOOL=ON
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
		-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
	INSTALL_COMMAND ""
	)

ExternalProject_Get_Property(aqsis BINARY_DIR)

ADD_LIBRARY(aqsis_core SHARED IMPORTED)
SET_TARGET_PROPERTIES(aqsis_core PROPERTIES
  IMPORTED_LOCATION "${BINARY_DIR}/libs/core/libaqsis_core${CMAKE_SHARED_LIBRARY_SUFFIX}"
  IMPORTED_IMPLIB "${BINARY_DIR}/libs/core/libaqsis_core.lib"
  )

# Build the module
K3D_BUILD_NGUI_MODULE(k3d-ngui-aqsis)
K3D_CREATE_MODULE_PROXY(k3d-ngui-aqsis)
ADD_DEPENDENCIES(k3d-ngui-aqsis aqsis)
TARGET_LINK_LIBRARIES(k3d-ngui-aqsis aqsis_core)
