# Test to ensure that committed code never uses printf() 

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"[.]svn"
	"[.]cu$"
	"[.]po$"
	"[.]sl$"
	"^k3dsdk/ftgl/src/FTFont/FTFontGlue.cpp$"
	"^k3dsdk/ftgl/src/FTGlyph/FTGlyphGlue.cpp$"
	"^k3dsdk/ftgl/src/FTLayout/FTLayoutGlue.cpp$"
	"^k3dsdk/sgi_tesselator/geom.c$"
	"^k3dsdk/xml.cpp$"
	"^modules/core/print_stdout.cpp$"
	"^modules/inotify/inotify-cxx.cpp$"
	"^modules/lsystem/l_parser.cpp$"
	"^modules/md2_io/md2.cpp$"
	"^modules/pdiff/CompareArgs.cpp$"
	"^modules/pdiff/Metric.cpp$"
	"^modules/polyhedron_sources/polyhedron.cpp$"
	"^modules/scalar/format_scalar.cpp$"
	"^share/shaders/k3d_tile.h$"
	"^tests/code.printf.cmake$"
	"^tests/CMakeLists.txt$"
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
		IF(SOURCE MATCHES "printf")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} uses 'printf' ... use k3d::log() for debugging and C++ iostreams for I/O .")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files using printf.")	
ENDIF()

