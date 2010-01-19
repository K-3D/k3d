# Test to ensure that legacy data structures aren't used in new code

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^[.]hg/.*$"
	"^k3dsdk/iselectable.h$"
	"^k3dsdk/ngui/document_state.cpp$"
	"^k3dsdk/selectable.h$"
	"^k3dsdk/serialization_xml.cpp$"
	"^modules/polyhedron/bevel_faces.cpp$"
	"^modules/polyhedron/dissolve_components.cpp$"
	"^modules/polyhedron/fillet_edges.cpp$"
	"^tests/code.legacy.cmake$"
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

