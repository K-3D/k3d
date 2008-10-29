# Ensure that header-files have unique preprocessor guards

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^k3dsdk/Half/.*$"
	"^k3dsdk/expression/fparser.h$"
	"^k3dsdk/expression/fpconfig.h$"
	"^k3dsdk/expression/fptypes.h$"
	"^k3dsdk/ftgl/.*$"
	"^k3dsdk/glew/.*$"
	"^k3dsdk/sgi_tesselator/.*$"
	"^modules/pdiff/CompareArgs.h$"
	"^modules/pdiff/LPyramid.h$"
	"^modules/pdiff/Metric.h$"
	"^modules/pdiff/RGBAImage.h$"
	"^modules/qslim/Mx.*$"
	"^share/.*$"
	)

FILE(GLOB_RECURSE SOURCE_FILES *.h)
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
		STRING(TOUPPER "${RELATIVE_SOURCE_FILE}" GUARD_SYMBOL)
		STRING(REPLACE "/" "_" GUARD_SYMBOL "${GUARD_SYMBOL}")
		STRING(REPLACE "." "_" GUARD_SYMBOL "${GUARD_SYMBOL}")

		SET(OPEN_GUARD "#ifndef ${GUARD_SYMBOL}")
		SET(DEFINE_GUARD "#define ${GUARD_SYMBOL}")
		SET(CLOSE_GUARD "#endif // !${GUARD_SYMBOL}")

		FILE(READ ${SOURCE_FILE} SOURCE)

		IF(NOT SOURCE MATCHES "${OPEN_GUARD}")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} missing ${OPEN_GUARD}")
		ENDIF()

		IF(NOT SOURCE MATCHES "${DEFINE_GUARD}")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} missing ${DEFINE_GUARD}")
		ENDIF()

		IF(NOT SOURCE MATCHES "${CLOSE_GUARD}")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} missing ${CLOSE_GUARD}")
		ENDIF()
	ENDIF()

ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} problems with header file guards")	
ENDIF()

