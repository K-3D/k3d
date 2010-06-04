INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${Aqsis_INCLUDE_DIRECTORIES}
	${Aqsis_SOURCE_DIR}
	${Aqsis_SOURCE_DIR}/libs/core
	${Aqsis_SOURCE_DIR}/libs/core/api
	${Aqsis_SOURCE_DIR}/libs/core/ddmanager
	${Aqsis_SOURCE_DIR}/libs/core/geometry
	${Aqsis_SOURCE_DIR}/libs/core/raytrace
	)

K3D_BUILD_MODULE(k3d-aqsis)
K3D_CREATE_MODULE_PROXY(k3d-aqsis)
TARGET_LINK_LIBRARIES(k3d-aqsis aqsis_core)
