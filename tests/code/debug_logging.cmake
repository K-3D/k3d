# Test for debug-abuse in committed code

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^docs/sample_module/module.cpp$"
	"^k3dsdk/log.cpp$"
	"^modules/advanced_opengl_painters/attic/.*$"
	"^modules/development/.*$"
	"^modules/ngui/user_interface.cpp$"
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
		IF(SOURCE MATCHES "log[(][ \t]*[)][ \t]*<<[ \t]*debug")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} uses debug logging.")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using debug logging, which is reserved for development ... either convert debug logging statements to other log levels (such as info), or remove them altogether.")	
ENDIF()

