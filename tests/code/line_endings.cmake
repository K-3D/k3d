# Ensure that K-3D source files always use Unix line-endings

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^application/icon.rc$"
	"^distribution/win32/aqsisrc$"
	"^k3dsdk/gil/boost/gil/"
	"^share/geometry/knot.obj$"
	"^tests/meshes/mesh.modifier.PGPRemesh.bunny.ply$"
	"^tests/meshes/mesh.modifier.PGPRemesh.torus.ply$"
	"^tests/meshes/mesh.source.OBJMeshReader.polyhedron.obj$"
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

		EXECUTE_PROCESS(
			COMMAND file ${SOURCE_FILE}
			COMMAND grep -q "CRLF"
			RESULT_VARIABLE FOUND_CRLF
			)

		IF(FOUND_CRLF EQUAL 0)
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} uses DOS line endings.")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using DOS line endings.")	
ENDIF()

