QT4_WRAP_CPP(MOC_BUILT_SOURCES dialog.h thread.h)
QT4_WRAP_UI(UI_BUILT_SOURCES dialog.ui)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
#	${Aqsis_INCLUDE_DIRECTORIES}
#	${Aqsis_SOURCE_DIR}
#	${Aqsis_SOURCE_DIR}/libs/core
#	${Aqsis_SOURCE_DIR}/libs/core/api
#	${Aqsis_SOURCE_DIR}/libs/core/ddmanager
#	${Aqsis_SOURCE_DIR}/libs/core/geometry
#	${Aqsis_SOURCE_DIR}/libs/core/raytrace
	)

K3D_BUILD_QTUI_MODULE(k3d-qtui-aqsis EXTRA_SOURCES ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
K3D_CREATE_MODULE_PROXY(k3d-qtui-aqsis)
#TARGET_LINK_LIBRARIES(k3d-qtui-aqsis aqsis_core)
