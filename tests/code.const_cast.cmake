# Test to ensure that const_cast isn't abused 

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"[.]yaml$"
	"^application/k3d_main.cpp$"
	"^k3dsdk/document.cpp$"
	"^k3dsdk/ftgl/src/FTVectoriser.cpp$"
	"^k3dsdk/gil/boost/gil/extension/dynamic_image/variant.hpp$"
	"^k3dsdk/socket.cpp$"
	"^k3dsdk/system.cpp$"
	"^k3dsdk/triangulator.cpp$"
	"^modules/cuda/cuda_device_polyhedra.cpp$"
	"^modules/development/glsl_painter.cpp$"
	"^modules/development/tweak_points.cpp$"
	"^modules/gnome/uri_handler.cpp$"
	"^modules/lsystem/l_parser.cpp$"
	"^modules/ngui/user_interface.cpp$"
	"^modules/ngui_material_manager_panel/GroupContentPanel.cpp$"
	"^modules/ngui_material_manager_panel/MaterialContentPanel.cpp$"
	"^modules/ngui_material_manager_panel/MaterialGroup.cpp$"
	"^modules/ngui_material_manager_panel/panel.cpp$"
	"^modules/opencascade/opencascade_to_k3d.cpp$"
	"^modules/plot/least_squares_plot.cpp$"
	"^modules/plot/surface_plot.cpp$"
	"^modules/polyhedron_sources/poly_grid.cpp$"
	"^modules/python/engine.cpp$"
	"^modules/pyui/user_interface.cpp$"
	"^modules/qtui/user_interface.cpp$"
	"^modules/renderman_painters/scripted_painter.cpp$"
	"^renderframe/main.cpp$"
	"^tests/CMakeLists.txt$"
	"^tests/code.const_cast.cmake$"
	)

FILE(GLOB_RECURSE SOURCE_FILES "*")
LIST(SORT SOURCE_FILES)

FOREACH(SOURCE_FILE ${SOURCE_FILES})
	FILE(RELATIVE_PATH RELATIVE_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE_FILE})

	SET(TEST_FILE 1)
	FOREACH(SKIP_FILE ${SKIP_FILES})
		IF(RELATIVE_SOURCE_FILE MATCHES "${SKIP_FILE}")
			SET(TEST_FILE 0)
		ENDIF()
	ENDFOREACH()

	IF(TEST_FILE)
		FILE(READ ${SOURCE_FILE} SOURCE)
		IF(SOURCE MATCHES "const_cast")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE}")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using const_cast, which should almost never be necessary.  Contact the mailing list to discuss exceptions.")	
ENDIF()

