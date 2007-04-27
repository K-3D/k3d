# Disallow in-source build
STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" K3D_IN_SOURCE)
IF(K3D_IN_SOURCE)
	MESSAGE(FATAL_ERROR "K-3D requires an out of source build. Please create a separate build directory and run 'cmake path_to_k3d [options]' there.")
ENDIF(K3D_IN_SOURCE)

