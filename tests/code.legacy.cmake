# Test to ensure that legacy data structures aren't used in new code

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^[.]hg/.*$"
	"^k3dsdk/geometry.cpp$"
	"^k3dsdk/geometry.h$"
	"^k3dsdk/iselectable.h$"
	"^k3dsdk/legacy_mesh.cpp$"
	"^k3dsdk/legacy_mesh_modifier.h$"
	"^k3dsdk/legacy_mesh_source.h$"
	"^k3dsdk/mesh.cpp$"
	"^k3dsdk/mesh.h$"
	"^k3dsdk/ngui/auto_property_page.cpp$"
	"^k3dsdk/ngui/document_state.cpp$"
	"^k3dsdk/properties.cpp$"
	"^k3dsdk/property_types.h$"
	"^k3dsdk/selectable.h$"
	"^k3dsdk/serialization_xml.cpp$"
	"^k3dsdk/type_registry.cpp$"
	"^modules/lsystem/l_parser.cpp$"
	"^modules/mesh/bevel_faces.cpp$"
	"^modules/mesh/bridge_edges.cpp$"
	"^modules/mesh/bridge_faces.cpp$"
	"^modules/mesh/cap_hole.cpp$"
	"^modules/mesh/collapse_edges.cpp$"
	"^modules/mesh/connect_vertices.cpp$"
	"^modules/mesh/dissolve_components.cpp$"
	"^modules/mesh/face_normals.cpp$"
	"^modules/mesh/fillet_edges.cpp$"
	"^modules/mesh/helpers.cpp$"
	"^modules/mesh/helpers.h$"
	"^modules/mesh/join_points.cpp$"
	"^modules/mesh/make_creases.cpp$"
	"^modules/mesh/weld.cpp$"
	"^modules/polyhedron/bevel_points.cpp$"
	"^modules/polyhedron/extrude_faces.cpp$"
	"^modules/polyhedron/helpers.cpp$"
	"^modules/polyhedron/helpers.h$"
	"^modules/polyhedron_sources/poly_cube.cpp$"
	"^modules/polyhedron_sources/poly_cushion.cpp$"
	"^modules/polyhedron_sources/poly_cylinder.cpp$"
	"^modules/polyhedron_sources/poly_sphere.cpp$"
	"^modules/polyhedron_sources/poly_torus.cpp$"
	"^modules/qslim/quadric_decimation.cpp$"
	"^modules/test/legacy_mesh_conversion.cpp$"
	"^modules/test/memory_pools.cpp$"
	"^tests/code.legacy.cmake$"
	"^tests/document.stress.read_all_properties.py$"
	"^tests/python.api.user_properties.py$"
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
		IF(SOURCE MATCHES "legacy::")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE}")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using the k3d::legacy namespace, which should never be used in new code.  Contact the mailing list to discuss alternatives.")	
ENDIF()

