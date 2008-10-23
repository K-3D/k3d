# Ensure that filenames are all lower-case (excepting third-party sources)

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^.*CMakeLists.txt$"
	"^.*[.]svn/.*$"
	"^AUTHORS$"
	"^COPYING$"
	"^CTestConfig.cmake$"
	"^INSTALL$"
	"^README$"
	"^cmake/modules/K3D.*$"
	"^distribution/osx/bundle/Info.plist.in$"
	"^docs/doxygen/Doxyfile.in$"
	"^k3dsdk/Half/.*$"
	"^k3dsdk/ftgl/.*$"
	"^k3dsdk/sgi_tesselator/README$"
	"^modules/qslim/Mx.*$"
	"^modules/pdiff/CompareArgs.*$"
	"^modules/pdiff/LPyramid.*$"
	"^modules/pdiff/Metric.*$"
	"^modules/pdiff/README.txt$"
	"^modules/pdiff/RGBAImage.*$"
	"^po/POTFILES.in$"
	"^po/POTFILES.skip$"
	"^share/fonts/.*[.]ttf$"
	"^share/ngui/rasterized/.*[.]png$"
	"^share/ngui/scalable/.*[.]svg$"
	"^share/scripts/.*$"
	"^tests/.*$"
	)

FILE(GLOB_RECURSE SOURCE_FILES *)

FOREACH(SOURCE_FILE ${SOURCE_FILES})
	FILE(RELATIVE_PATH RELATIVE_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE_FILE})

	SET(TEST_FILE 1)
	FOREACH(SKIP_FILE ${SKIP_FILES})
		IF(RELATIVE_SOURCE_FILE MATCHES "${SKIP_FILE}")
			SET(TEST_FILE 0)
		ENDIF()
	ENDFOREACH()

	IF(TEST_FILE)
		STRING(TOLOWER "${RELATIVE_SOURCE_FILE}" LOWER_CASE_SOURCE_FILE)

		IF(NOT LOWER_CASE_SOURCE_FILE STREQUAL RELATIVE_SOURCE_FILE)
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} must be all-lower-case")
		ENDIF()
	ENDIF()

ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} invalid filenames.")	
ENDIF()

