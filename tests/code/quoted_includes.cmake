# Ensure that K-3D header files are always #included using angle-brackets instead of quotation marks

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"[.]hg/"
	"[.]sl$"
	"^application/main.cpp$"
	"^application/winmain.cpp$"
	"^k3dsdk/expression/fparser.cpp$"
	"^k3dsdk/expression/fpoptimizer.cpp$"
	"^k3dsdk/ftgl/src/"
	"^k3dsdk/gil/boost/gil/"
	"^k3dsdk/glew/glew.c$"
	"^k3dsdk/glew/glxew.h$"
	"^k3dsdk/sgi_tesselator/"
	"^modules/"
	"^share/shaders/.*[.]h$"
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
		IF(SOURCE MATCHES "#include[ \t]*\"")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} uses #include \"...\" ... use #include <...> for all K-3D header files.")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using #include \"...\".")	
ENDIF()

